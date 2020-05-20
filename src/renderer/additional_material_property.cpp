#include <renderer/additional_material_property.hpp>
#include <renderer/program_uniforms.hpp>

void renderer::additional_material_property_f::bind_property(const renderer::program_uniforms& uniforms)
{
	gl::glUniform1f(uniforms.find_location(property_name_hash), value);
}