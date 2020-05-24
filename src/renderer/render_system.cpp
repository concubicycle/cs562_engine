#include <renderer/render_system.hpp>
#include <glbinding/gl/gl.h>
#include <renderer/camera.hpp>
#include <renderer/punctual_light.hpp>
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
    , _default(assets.get_text(default_vert), assets.get_text(default_frag))
    , _skybox(assets.get_text(skybox_vert), assets.get_text(skybox_frag))
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

    state.each<transform, camera>([&](transform& t, camera& c) {        
                
        handle_cam_background(_default, c);

        _default.bind();
        bind_camera_uniforms(_default, t, c);
        set_light_uniforms(state, _default, c);
        draw_scene(state);
        _default.unbind();
    });
}


void renderer::render_system::set_light_uniforms(ecs::state& state, const renderer::shader_program& shader, renderer::camera& cam)
{
    using namespace transforms;
    using namespace gl;

    unsigned int light_count = 0;
    state.each<transform, punctual_light>([&](transform& t, punctual_light& pl) {        

        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].color", pl.color);
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].position", t.world_position());
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].intensity", pl.intensity);
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].radius", pl.intensity);

        light_count++;        
    });    
    
    shader.set_uniform("point_light_count", (GLint)(light_count));

    state.each<ambient_light>([&](ambient_light& al) {
        shader.set_uniform("ambient_light", al.color); // only one will bind
        });
}

void renderer::render_system::draw_scene(ecs::state& state)
{
    using namespace gl;
    using namespace transforms;
       

    state.each<transform, model_instance>([&](transform& t, model_instance& mi) {
        for (size_t i = 0; i < mi.model.mesh_count; ++i)
        {            
            draw_mesh(t, mi.model.meshes[i]);
        }
    });    
}

void renderer::render_system::handle_cam_background(
    const renderer::shader_program& shader,
    const renderer::camera& cam)
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


void renderer::render_system::draw_mesh(const transforms::transform& transform, const opengl_mesh& mesh)
{
    using namespace gl;

    bind_material(mesh.material, _default);

    _default.set_uniform("model", transform.local_to_world().matrix());
    _default.set_uniform("model_inverse", transform.local_to_world().inverse().matrix());
    
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

    glDepthMask(GL_FALSE);
      
    
    Eigen::Matrix3f mat3 = cam.view.rotation().matrix();
    Eigen::Matrix4f mat4 = Eigen::Matrix4f::Identity();
    mat4.block(0, 0, 3, 3) = mat3;

    _skybox.bind();
    _skybox.set_uniform("view", mat4);
    _skybox.set_uniform("projection", cam.projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glBindVertexArray(cam.skybox_cubemap->vao);        
    glDrawArrays(GL_TRIANGLES, 0, 36);    
    glBindVertexArray(0);    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
}