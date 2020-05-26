#include <renderer/render_system.hpp>
#include <glbinding/gl/gl.h>
#include <renderer/camera.hpp>
#include <renderer/punctual_light.hpp>
#include <renderer/ambient_light.hpp>
#include <transforms/transform.hpp>


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    using namespace gl;

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



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
    , _geometry_pass(assets.get_text(geometry_pass_vert), assets.get_text(geometry_pass_frag))
    , _lighting_pass(assets.get_text(lighting_pass_vert), assets.get_text(lighting_pass_frag))
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


    // configure g-buffer framebuffer
    // ------------------------------    
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _glfw.width(), _glfw.height(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    // normal color buffer
    //glGenTextures(1, &gNormal);
    //glBindTexture(GL_TEXTURE_2D, gNormal);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _glfw.width(), _glfw.height(), 0, GL_RGB, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    //// color + specular color buffer
    //glGenTextures(1, &gAlbedoSpec);
    //glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _glfw.width(), _glfw.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    gl::GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _glfw.width(), _glfw.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Frame buffer incomplete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::render_system::update(ecs::state& state)
{
    using namespace transforms;
    using namespace gl;    

    state.each<transform, camera>([&](transform& t, camera& c) {       

        // 1. geometry pass: render scene's geometry/color data into gbuffer
       // -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _geometry_pass.bind();
        bind_camera_uniforms(_geometry_pass, t, c);
        
        state.each<transform, model_instance>([&](transform& t, model_instance& mi) {
            for (size_t i = 0; i < mi.model.mesh_count; ++i)
            {
                draw_mesh(t, mi.model.meshes[i]);
            }
        });
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
        // -----------------------------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _lighting_pass.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        /*
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        //_lighting_pass.set_uniform("camera_position", t.world_position());
        */

        renderQuad();


        // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
       // ----------------------------------------------------------------------------------
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        //glBlitFramebuffer(0, 0, _glfw.width(), _glfw.height(), 0, 0, _glfw.width(), _glfw.height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

       /* handle_cam_background(c);
        _default.bind();
        bind_camera_uniforms(_default, t, c);
        set_light_uniforms(state, _default, c);
        draw_scene(state);
        _default.unbind();*/
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


void renderer::render_system::draw_mesh(const transforms::transform& transform, const opengl_mesh& mesh)
{
    using namespace gl;

    Eigen::Matrix4f adjoint_transpose = transform.local_to_world().matrix().adjoint().transpose();

    bind_material(mesh.material, _geometry_pass);

    _geometry_pass.set_uniform("model", transform.local_to_world().matrix());
    _geometry_pass.set_uniform("adjoint_transpose", adjoint_transpose);
    
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
    Eigen::Matrix3f mat3 = cam.view.rotation().matrix();
    Eigen::Matrix4f mat4 = Eigen::Matrix4f::Identity();
    mat4.block(0, 0, 3, 3) = mat3;

    _skybox.bind();
    glDepthMask(GL_FALSE);

    _skybox.set_uniform("view", mat4);
    _skybox.set_uniform("projection", cam.projection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glBindVertexArray(cam.skybox_cubemap->vao);        
    glDrawArrays(GL_TRIANGLES, 0, 36);    
    glBindVertexArray(0);    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
    _skybox.unbind();
}