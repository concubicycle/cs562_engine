#include <renderer/render_system.hpp>
#include <glbinding/gl/gl.h>
#include <renderer/camera.hpp>
#include <renderer/punctual_light.hpp>
#include <renderer/local_punctual_light.hpp>
#include <renderer/ambient_light.hpp>
#include <transforms/transform.hpp>



renderer::render_system::render_system(
    util::string_table& strings, 
    asset::asset_loader& assets,
    core::glfw_context& glfw,
    core::startup_config& config)
    : _strings(strings)
    , _assets(assets)
    , _glfw(glfw)
    , _config(config)
    , _sphere(4, 4)
    , _default(assets.get_text(default_vert), assets.get_text(default_frag))
    , _skybox(assets.get_text(skybox_vert), assets.get_text(skybox_frag))
    , _geometry_pass(assets.get_text(geometry_pass_vert), assets.get_text(geometry_pass_frag))
    , _lighting_pass(assets.get_text(lighting_pass_vert), assets.get_text(lighting_pass_frag))
    , _local_light_pass(assets.get_text(local_light_pass_vert), assets.get_text(local_light_pass_frag))
    , _gbuffer(
        glfw.width(),
        glfw.height(),
        texture_description(
            gl::GLenum::GL_COLOR_ATTACHMENT0,
            glfw.width(),
            glfw.height(),
            gl::GLenum::GL_RGBA32F,
            gl::GLenum::GL_RGBA,
            gl::GLenum::GL_FLOAT),
        texture_description(
            gl::GLenum::GL_COLOR_ATTACHMENT1,
            glfw.width(),
            glfw.height(),
            gl::GLenum::GL_RGBA32F,
            gl::GLenum::GL_RGBA,
            gl::GLenum::GL_FLOAT),
        texture_description(
            gl::GLenum::GL_COLOR_ATTACHMENT2,
            glfw.width(),
            glfw.height(),
            gl::GLenum::GL_RGBA,
            gl::GLenum::GL_RGBA,
            gl::GLenum::GL_FLOAT))
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
}

void renderer::render_system::update(ecs::state& state)
{
    using namespace transforms;
    using namespace gl;    

    state.each<transform, camera>([&](transform& t, camera& c) {
                
        _gbuffer.bind();
        handle_cam_background(c);
        _geometry_pass.bind();
        bind_camera_uniforms(_geometry_pass, t, c);        
        draw_scene(state, _geometry_pass);        
        _gbuffer.unbind();
                
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _lighting_pass.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _gbuffer.texture(0));        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _gbuffer.texture(1));
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _gbuffer.texture(2));
        
        _lighting_pass.set_uniform("camera_position", t.world_position());
        _lighting_pass.set_uniform("specular", Eigen::Vector3f(0.5f, 0.5f, 0.5f));
        _lighting_pass.set_uniform("shininess", 0.5f);        
        set_light_uniforms(state, _lighting_pass, c);

        _fsq.draw();

        draw_local_lights(state, t, c);
    });
}


void renderer::render_system::set_light_uniforms(ecs::state& state, const renderer::shader_program& shader, renderer::camera& cam)
{
    using namespace transforms;
    using namespace gl;

    GLint light_count = 0;
    state.each<transform, punctual_light>([&](transform& t, punctual_light& pl) {
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].color", pl.color);
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].position", t.world_position());
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].intensity", pl.intensity);
        light_count++;        
    });    
    
    shader.set_uniform("point_light_count", light_count);

    state.each<ambient_light>([&](ambient_light& al) {
        shader.set_uniform("ambient_light", al.color); // only one will bind
    });
}

void renderer::render_system::draw_scene(ecs::state& state, const shader_program& program)
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

void renderer::render_system::handle_cam_background(const renderer::camera& cam)
{
    using namespace gl;

    switch (cam.background)
    {
    case background_type::color:
        glClearColor(cam.clear_color[0], cam.clear_color[1], cam.clear_color[2], cam.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        break;
    case background_type::cubemap:
        glClearColor(cam.clear_color[0], cam.clear_color[1], cam.clear_color[2], cam.clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_skybox(cam);
    default:
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

void renderer::render_system::bind_material(const opengl_material& material, const shader_program& program)
{
    bind_texture(material.diffuse_texture);
    bind_texture(material.metalness_texture);
    bind_texture(material.normal_texture);
    bind_texture(material.roughness_texture);
    bind_texture(material.ambient_occlusion_texture);

    program.set_uniform("specular", material.specular);
    program.set_uniform("shininess", material.shininess);
}

void renderer::render_system::bind_texture(const opengl_texture& texture)
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

void renderer::render_system::draw_skybox(const camera& cam)
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

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}