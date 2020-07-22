#include <renderer/render_system.hpp>
#include <glbinding/gl/gl.h>
#include <renderer/camera.hpp>
#include <renderer/punctual_light.hpp>
#include <renderer/local_punctual_light.hpp>
#include <renderer/ambient_light.hpp>
#include <renderer/projections.hpp>
#include <renderer/lookat.hpp>
#include <renderer/participating_medium.hpp>
#include <transforms/transform.hpp>



renderer::render_system::render_system(
    util::string_table& strings, 
    asset::asset_loader& assets,
    assimp_vram_loader& vram_loader,
    core::glfw_context& glfw,
    core::startup_config& config)
    : _strings(strings)
    , _assets(assets)
    , _vram_loader(vram_loader)
    , _glfw(glfw)
    , _config(config)      
{
    using namespace gl;

    glViewport(0, 0, glfw.width(), glfw.height());

    if (_config.backface_culling())
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto& fuv_tex = _assets.get_texturef_untonemapped("assets/Fuv.hdr");

    _fuv_table_dimensions[0] = fuv_tex.width;
    _fuv_table_dimensions[1] = fuv_tex.height;
    _fuv_table_texture = _vram_loader.load_texturef(fuv_tex, false);
}


void renderer::render_system::initialize(ecs::state& state)
{
    using namespace transforms;
    using namespace gl;
}


void renderer::render_system::update(ecs::state& state)
{
    using namespace transforms;
    using namespace gl;

    state.each<transform, punctual_light>([&](transform& t, punctual_light& pl) {
        render_shadowmap(state, t, pl);
    });

    state.each<transform, directional_light>([&](transform& t, directional_light& dl) {
        render_shadowmap_directional(state, t, dl);
    });

    glViewport(0, 0, _glfw.width(), _glfw.height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   state.each<transform, camera>([&](transform& t, camera& c) {
        _gbuffer.bind();
        handle_cam_background(c);
        _geometry_pass.bind();
        bind_camera_uniforms(_geometry_pass, t, c);
        draw_scene(state, _geometry_pass);
        _gbuffer.unbind();
                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _lighting_pass.bind();
        _gbuffer.bind_textures();
        _lighting_pass.set_uniform("camera_position", t.world_position());
        set_light_uniforms(state, _lighting_pass, c);

        _fsq.draw();

        draw_local_lights(state, t, c);
        draw_airlight(state, t, c);
    });
}


void renderer::render_system::set_light_uniforms(
    ecs::state& state, 
    const renderer::shader_program& shader, 
    renderer::camera& cam)
{
    using namespace transforms;
    using namespace gl;

    GLint light_count = 0;
    GLint shadowmap_texture_unit = 4;
    state.each<transform, punctual_light>([&](transform& t, punctual_light& pl) {
        std::string point_light_str = "point_lights[" + std::to_string(light_count) + "]";

        shader.set_uniform(point_light_str + ".color", pl.color);
        shader.set_uniform(point_light_str + ".position", t.world_position());
        shader.set_uniform(point_light_str + ".reference_distance", pl.reference_distance);
        shader.set_uniform(point_light_str + ".light_view", pl.light_view);
        shader.set_uniform(point_light_str + ".light_view_back", pl.light_view_back);

        // bind shadow map
        auto location = shader.uniform_location(point_light_str + ".shadow_map");
        glActiveTexture(GL_TEXTURE0 + shadowmap_texture_unit);
        glBindTexture(GL_TEXTURE_2D, pl.shadowmap_framebuffer.texture(0));
        glUniform1i(location, shadowmap_texture_unit);

        shadowmap_texture_unit++;
        light_count++;
    });

    GLint directional_light_count = 0;
    state.each<transform, directional_light>([&](transform& t, directional_light& dl) {
        std::string direction_light_str = "directional_lights[" + std::to_string(directional_light_count) + "]";

        shader.set_uniform(direction_light_str + ".color", dl.color);
        shader.set_uniform(direction_light_str + ".direction", dl.direction);
        shader.set_uniform(direction_light_str + ".light_view", dl.light_view);
        shader.set_uniform(direction_light_str + ".light_projection", dl.light_projection);
        
        auto location = shader.uniform_location(direction_light_str + ".shadow_map");
        glActiveTexture(GL_TEXTURE0 + shadowmap_texture_unit);
        glBindTexture(GL_TEXTURE_2D, *(dl.filter_output_texture.texture_id));
        glUniform1i(location, shadowmap_texture_unit);

        shadowmap_texture_unit++;
        directional_light_count++;
    });
    
    shader.set_uniform("point_light_count", light_count);
    shader.set_uniform("directional_light_count", directional_light_count);

    state.each<ambient_light>([&](ambient_light& al) {
        shader.set_uniform("ambient_light", al.color);
        shader.set_uniform("use_ambient_light", true);
    });

    state.each<camera>([&](camera& cam) {
        if (cam.skydome_texture && cam.background == background_type::skydome)
        {
            shader.set_uniform("use_skydome_light", true);
            shader.set_uniform("skydome_size", cam.skydome_size);

            auto location = shader.uniform_location("skydome_light");            
            glActiveTexture(GL_TEXTURE0 + shadowmap_texture_unit);
            glBindTexture(GL_TEXTURE_2D, *cam.skydome_texture);
            glUniform1i(location, shadowmap_texture_unit++);

            location = shader.uniform_location("skydome_irradiance_map");
            glActiveTexture(GL_TEXTURE0 + shadowmap_texture_unit);
            glBindTexture(GL_TEXTURE_2D, *cam.skydome_irradiance_map_texture);
            glUniform1i(location, shadowmap_texture_unit++);
            
            shader.bind_uniform_block("HammersleyBlock", _hammersley_block.bindpoint());
        }
    });
}


void renderer::render_system::draw_scene(
    ecs::state& state, 
    const shader_program& program)
{
    using namespace gl;
    using namespace transforms;       

    state.each<transform, model_instance>([&](transform& t, model_instance& mi) {
        for (size_t i = 0; i < mi.model.mesh_count; ++i)
        {            
            draw_mesh(t, mi.model.meshes[i], program);
        }
    });    
}

void renderer::render_system::handle_cam_background(
    const renderer::camera& cam)
{
    using namespace gl;

    switch (cam.background)
    {
    case background_type::cubemap:
        glClearColor(cam.clear_color[0], cam.clear_color[1], cam.clear_color[2], cam.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_skybox(cam);
        break;
    case background_type::skydome:
        glClearColor(cam.clear_color[0], cam.clear_color[1], cam.clear_color[2], cam.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_skydome(cam);
        break;
    case background_type::color:
    default:
        glClearColor(cam.clear_color[0], cam.clear_color[1], cam.clear_color[2], cam.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;
    }
}


void renderer::render_system::draw_mesh(
    const transforms::transform& transform, 
    const opengl_mesh& mesh,
    const shader_program& program)
{
    using namespace gl;
        
    bind_material(mesh.material, program);

    program.set_uniform("model", transform.local_to_world().matrix());
    program.set_uniform("adjoint_transpose", transform.adjoint_transpose());

    glBindVertexArray(mesh.vao);
    if (mesh.ebo)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, mesh.index_count);
    }
    glBindVertexArray(0);
}

void renderer::render_system::bind_material(
    const opengl_material& material, 
    const shader_program& program)
{
    bind_texture(material.diffuse_texture);
    bind_texture(material.metalness_texture);
    bind_texture(material.normal_texture);
    bind_texture(material.roughness_texture);
    bind_texture(material.ambient_occlusion_texture);

    program.set_uniform("fresnel_color", material.fresnel_color);    
    program.set_uniform("roughness", material.roughness);
    program.set_uniform("metalness", material.metalness);
}

void renderer::render_system::bind_texture(
    const opengl_texture& texture)
{
    if (texture.texture_id)
    {
        glActiveTexture(texture.texture_index);
        glBindTexture(gl::GL_TEXTURE_2D, *texture.texture_id);
    }
}

void renderer::render_system::bind_camera_uniforms(
    const shader_program& shader,
    const transforms::transform& transform,
    const camera& cam)
{
    shader.set_uniform("projection", cam.projection);
    shader.set_uniform("view", cam.view.matrix());
    shader.set_uniform("camera_position", transform.world_position());
}

void renderer::render_system::draw_skybox(
    const camera& cam)
{
    using namespace gl;

    auto texture_id = *cam.skybox_cubemap->cubemap.texture_id;
    auto vao = cam.skybox_cubemap->vao;

    Eigen::Matrix3f mat3 = cam.view.rotation().matrix();
    Eigen::Matrix4f mat4 = Eigen::Matrix4f::Identity();
    mat4.block(0, 0, 3, 3) = mat3;

    _skybox.bind();
    glDepthMask(GL_FALSE);

    _skybox.set_uniform("view", mat4);
    _skybox.set_uniform("projection", cam.projection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);    
    glBindVertexArray(0);    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
    _skybox.unbind();
}

void renderer::render_system::draw_skydome(
    const camera& cam)
{
    using namespace gl;

    auto texture_id = *cam.skydome_texture;
    auto vao = skydome_mesh.get_vao();
    auto ebo = skydome_mesh.get_ebo();
    auto index_count = skydome_mesh.get_index_count();

    Eigen::Matrix3f mat3 = cam.view.rotation().matrix();
    Eigen::Matrix4f mat4 = Eigen::Matrix4f::Identity();
    mat4.block(0, 0, 3, 3) = mat3;

    _skydome.bind();
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    _skydome.set_uniform("view", mat4);
    _skydome.set_uniform("projection", cam.projection);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    _skydome.unbind();
}

// CAREFUL: This is a performance bottleneck. TODO: Instanced rendering for light spheres. 
void renderer::render_system::draw_local_lights(
    ecs::state& state, 
    const transforms::transform& camera_transform,
    const camera& cam)
{
    using namespace transforms;
    using namespace gl;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    _local_light_pass.bind();
   
    glBindVertexArray(_sphere.get_vao());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sphere.get_ebo());

    Eigen::Vector2f gbuffer_dimensions(_glfw.width(), _glfw.height());

    _local_light_pass.set_uniform(0, cam.projection);
    _local_light_pass.set_uniform(1, cam.view.matrix());
    _local_light_pass.set_uniform(6, camera_transform.world_position());

    state.each<transform, local_punctual_light>([&](transform& t, local_punctual_light& lpl) {

        if (lpl.radius_dirty())
        {
            t.scale() = Eigen::Vector3f(lpl.radius(), lpl.radius(), lpl.radius());
            lpl.set_clean();
        }

        _local_light_pass.set_uniform(2, std::as_const(t).model_matrix());
        _local_light_pass.set_uniform(3, std::as_const(t).world_position());
        _local_light_pass.set_uniform(4, lpl.color);
        _local_light_pass.set_uniform(5, lpl.radius());
        _local_light_pass.set_uniform(7, gbuffer_dimensions);
        
        glDrawElements(GL_TRIANGLES, _sphere.get_index_count(), GL_UNSIGNED_INT, 0);
    });
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _local_light_pass.unbind();
        
    glEnable(GL_DEPTH_TEST);
}

void renderer::render_system::draw_airlight(
    ecs::state& state, 
    const transforms::transform& camera_transform, 
    const camera& cam)
{
    using namespace transforms;
    using namespace gl;

    auto* pm = state.first<participating_medium>();
    if (!pm) return;

    auto& participating_medium_component = pm->get_component<participating_medium>();

    _airlight.bind();

    _airlight.set_uniform("beta", participating_medium_component.beta);
    _airlight.set_uniform("initial_intensity", participating_medium_component.initial_intensity);
    _airlight.set_uniform("use_single_scattering", participating_medium_component.use_single_scattering);

    _airlight.set_uniform("projection", cam.projection);
    _airlight.set_uniform("view", cam.view.matrix());
    _airlight.set_uniform("width", (float)_glfw.width());
    _airlight.set_uniform("height", (float)_glfw.height());
    _airlight.set_uniform("eye_position", camera_transform.world_position());
    _airlight.set_uniform("F_table_resolution", _fuv_table_dimensions[0]);
    
    auto location = _airlight.uniform_location("gPosition");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.texture(1));
    glUniform1i(location, 0);

    location = _airlight.uniform_location("gNormal");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _gbuffer.texture(1));
    glUniform1i(location, 1);

    location = _airlight.uniform_location("F_table");
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _fuv_table_texture);
    glUniform1i(location, 4);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);

    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);

    state.each<transform, directional_light>([&](transform& t, directional_light& dl) {
        Eigen::Matrix4f light_view_inverse = dl.light_view.inverse();
        Eigen::Matrix4f light_projection_inverse = dl.light_projection.inverse();

        _airlight.set_uniform("light_view_inverse", light_view_inverse);
        _airlight.set_uniform("light_projection_inverse", light_projection_inverse);
        _airlight.set_uniform("light_position", t.world_position());
        _airlight.set_uniform("light_view", dl.light_view);

        // bind shadow map
        auto location = _airlight.uniform_location("shadow_map");
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, dl.shadowmap_framebuffer.texture(0));
        glUniform1i(location, 3);

        // draw airlight to FBO
        _post_process_buffer.bind();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        dl.airlight_mesh.draw();
        _post_process_buffer.unbind();

        // blend FBO into 
        _draw_texture.bind();

        location = _airlight.uniform_location("color");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _post_process_buffer.texture(0));
        glUniform1i(location, 0);

        _fsq.draw();
    });    
        
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _airlight.unbind();
}

void renderer::render_system::render_shadowmap(
    ecs::state& state, 
    transforms::transform& t, 
    punctual_light& pl)
{
    using namespace gl;
    using namespace transforms;

    pl.shadowmap_framebuffer.bind();
    _dual_paraboloid_shadow.bind();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    // front facing
    glViewport(0, 0, pl.shadow_map_resolution, pl.shadow_map_resolution);
    Eigen::Translation3f translate(t.world_position());
    affine_transform view_transform = translate * affine_transform::Identity();
    pl.light_view = view_transform.matrix().inverse();    
    _dual_paraboloid_shadow.set_uniform("view", pl.light_view);
    draw_scene_shadowmap(state, _dual_paraboloid_shadow);

    // back facing
    glViewport(pl.shadow_map_resolution, 0, pl.shadow_map_resolution, pl.shadow_map_resolution);
    Eigen::Quaternionf rotate(Eigen::AngleAxis<float>(util::Pi, Eigen::Vector3f::UnitY()));
    view_transform = translate * rotate * affine_transform::Identity();
    pl.light_view_back = view_transform.matrix().inverse();
    _dual_paraboloid_shadow.set_uniform("view", pl.light_view_back);
    draw_scene_shadowmap(state, _dual_paraboloid_shadow);

    _dual_paraboloid_shadow.unbind();
    pl.shadowmap_framebuffer.unbind();
    
    // gaussian blur
    auto width = pl.shadow_map_resolution * 2;
    auto height = pl.shadow_map_resolution;

    // horizontal
    _gaussian_horizontal.bind();
    auto src_img_loc = _gaussian_horizontal.uniform_location("input_image");
    auto dst_img_loc = _gaussian_horizontal.uniform_location("output_image");
    glBindImageTexture(0, pl.shadowmap_framebuffer.texture(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, *(pl.filter_output_texture.texture_id), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform1i(src_img_loc, 0);
    glUniform1i(dst_img_loc, 1);
    glDispatchCompute(width / 128, height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);    

    // vertical
    _gaussian_vertical.bind();
    src_img_loc = _gaussian_vertical.uniform_location("input_image");
    dst_img_loc = _gaussian_vertical.uniform_location("output_image");
    glBindImageTexture(0, *(pl.filter_output_texture.texture_id), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, pl.shadowmap_framebuffer.texture(0), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform1i(src_img_loc, 0);
    glUniform1i(dst_img_loc, 1);
    glDispatchCompute(width, height / 128, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    _gaussian_vertical.unbind();
}

void renderer::render_system::render_shadowmap_directional(
    ecs::state& state, 
    transforms::transform& t, 
    directional_light& dl)
{
    using namespace gl;
    using namespace transforms;

    dl.shadowmap_framebuffer.bind();
    _directional_shadow.bind();

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    glViewport(0, 0, dl.shadow_map_resolution, dl.shadow_map_resolution);
    Eigen::Vector3f position = t.world_position();
    Eigen::Vector3f center = position + dl.direction;
    
    dl.light_view = lookat(position, center, Eigen::Vector3f(0, 0, -1));

    _directional_shadow.set_uniform("view", dl.light_view);
    _directional_shadow.set_uniform("projection", dl.light_projection);
    draw_scene_shadowmap(state, _directional_shadow);

    _directional_shadow.unbind();
    dl.shadowmap_framebuffer.unbind();

    // gaussian blur
    auto width = dl.shadow_map_resolution;
    auto height = dl.shadow_map_resolution;

    // horizontal
    _gaussian_horizontal.bind();
    auto src_img_loc = _gaussian_horizontal.uniform_location("input_image");
    auto dst_img_loc = _gaussian_horizontal.uniform_location("output_image");
    glBindImageTexture(0, dl.shadowmap_framebuffer.texture(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, *(dl.filter_intermediate_texture.texture_id), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform1i(src_img_loc, 0);
    glUniform1i(dst_img_loc, 1);
    glDispatchCompute(width / 128, height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // vertical
    _gaussian_vertical.bind();
    src_img_loc = _gaussian_vertical.uniform_location("input_image");
    dst_img_loc = _gaussian_vertical.uniform_location("output_image");
    glBindImageTexture(0, *(dl.filter_intermediate_texture.texture_id), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, *(dl.filter_output_texture.texture_id), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform1i(src_img_loc, 0);
    glUniform1i(dst_img_loc, 1);
    glDispatchCompute(width, height / 128, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    _gaussian_vertical.unbind();
}

void renderer::render_system::draw_scene_shadowmap(ecs::state& state, shader_program& shader)
{
    using namespace gl;
    using namespace transforms;

    bool is_front_face_culling = false;

    state.each<transform, model_instance>([&](transform& t, model_instance& mi) {
        if (mi.is_closed_shape && !is_front_face_culling)
        {
            //glCullFace(GL_FRONT);
            is_front_face_culling = true;
        }
        else if (!mi.is_closed_shape)
        {
            //glCullFace(GL_BACK);
            is_front_face_culling = false;
        }

        for (size_t i = 0; i < mi.model.mesh_count; ++i)
            draw_mesh(t, mi.model.meshes[i], shader);
        });

    glCullFace(GL_BACK);
}


renderer::opengl_mesh renderer::render_system::load_icosphere()
{
    auto& asset = _assets.get_assimp_scene("assets/3d_shapes/icosphere.glb");
    return _vram_loader.load_mesh(asset::assimp_mesh(asset.aiscene->mMeshes[0]));
}

