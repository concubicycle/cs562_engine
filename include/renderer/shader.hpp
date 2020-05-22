#ifndef __SHADER_HPP_
#define __SHADER_HPP_

#include <glbinding/gl/gl.h>
#include <spdlog/spdlog.h>

namespace renderer
{    
    class shader 
    {
        static constexpr unsigned int GPU_INFOLOG_BUFFER_SIZE = 512;     


    public:
        shader(gl::GLenum type, const std::string& contents);
        ~shader();

        void from_string(const std::string& shader_string);
        void compile();

        gl::GLuint id() const { return _id; }
        gl::GLenum type() const { return _type; }
        bool is_compiled() const { return _is_compiled; }

    private:
        gl::GLuint _id;
        const gl::GLenum _type;
        bool _is_compiled = false;
                
        void validate_shader();
    };
}

#endif