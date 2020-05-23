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

    _default.bind();

    state.each<transform, camera>([&](transform& t, camera& c) {        
                
        handle_cam_background(_default, c);
        bind_camera_uniforms(_default, t, c);
        set_light_uniforms(state, _default, c);

        draw_scene(state);
    });

    _default.unbind();
}


void renderer::render_system::set_light_uniforms(ecs::state& state, const renderer::shader_program& shader, renderer::camera& cam)
{
    using namespace transforms;
    using namespace gl;

    unsigned int light_count = 0;
    state.each<transform, punctual_light>([&](transform& t, punctual_light& pl) {        

        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].color", pl.color);
        shader.set_uniform("point_lights[" + std::to_string(light_count) + "].position", t.position());
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