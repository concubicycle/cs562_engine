#include <vector>

#include <renderer/assimp_vram_loader.hpp>
#include <asset/assimp_model.hpp>

#include <assimp/scene.h>

#include <renderer/shapes.hpp>






renderer::assimp_vram_loader::assimp_vram_loader(asset::asset_loader& loader)
	: _loader(loader)
{
}

renderer::opengl_model renderer::assimp_vram_loader::load_model(const asset::assimp_scene_asset& scene)
{
	auto cached = _model_cache.find(scene.aiscene);
	if (cached != _model_cache.end())
		return cached->second;	

	asset::assimp_model model_asset(scene.aiscene, _loader);
	opengl_model& result = _model_cache[scene.aiscene];

	for (size_t mesh_index = 0; mesh_index < model_asset.mesh_count(); ++mesh_index)
	{
		const auto& mesh = model_asset.mesh_at(mesh_index);
		const auto& material = model_asset.material_at(mesh_index);

		auto ogl_mesh = load_mesh(mesh);
		ogl_mesh.material = load_material(material);
		result.add_mesh(ogl_mesh);
	}

	return result;
}

renderer::opengl_model renderer::assimp_vram_loader::load_model(
	const asset::assimp_scene_asset& scene, 
	const nlohmann::json& mesh_material_array)
{
	throw std::runtime_error("No support for custom material representation yet.");
}

renderer::opengl_material renderer::assimp_vram_loader::load_material(const nlohmann::json& json, size_t index)
{
	throw std::runtime_error("No support for custom material representation yet.");	
}

renderer::opengl_material renderer::assimp_vram_loader::load_material(const asset::assimp_material& material)
{
	opengl_material result;
	
	result.roughness = material.roughness;
	result.metalness = material.metalness > 0.5f ? true : false;
	//result.fresnel_color = material.base_color.block<3, 1>(0, 0);

	auto has = [&](aiTextureType tt) { return material.material_textures.find(tt) != material.material_textures.end(); };
	auto get = [&](aiTextureType tt) { return material.material_textures.at(tt); };
	auto do_textype = [&](opengl_texture& tex, aiTextureType tt) {
		if (has(tt))
			tex.texture_id = load_texture(*get(tt));
	};
	
	do_textype(result.diffuse_texture, aiTextureType::aiTextureType_DIFFUSE);
	do_textype(result.metalness_texture, aiTextureType::aiTextureType_METALNESS);
	do_textype(result.normal_texture, aiTextureType::aiTextureType_NORMALS);
	do_textype(result.roughness_texture, aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS);
	do_textype(result.ambient_occlusion_texture, aiTextureType::aiTextureType_AMBIENT_OCCLUSION);

	return result;
}

renderer::opengl_mesh renderer::assimp_vram_loader::load_mesh(const asset::assimp_mesh& mesh)
{
    using namespace gl;

    renderer::opengl_mesh result;
	result.index_count = mesh.index_count();

    // Create OpenGL representation
    glGenVertexArrays(1, &result.vao);
    glGenBuffers(1, &result.vbo);
    glGenBuffers(1, &result.ebo);

    // Copy data to VRAM
    glBindVertexArray(result.vao);
    glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_data_bytes(), mesh.vertex_data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_data_bytes(), mesh.index_data(), GL_STATIC_DRAW);

    // Register data layout
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, texture_coords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, bitangent));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(asset::assimp_vertex), (void*)offsetof(asset::assimp_vertex, color));

    glBindVertexArray(0);

	return result;
}

gl::GLuint renderer::assimp_vram_loader::load_texture(const asset::texture_asset& texture_asset)
{
	using namespace gl;

	gl::GLuint result;
	auto texture_channels_gl = num_channels_to_gltype(texture_asset.channels);

	// Create OpenGL representation
	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);

	// Copy data to VRAM
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		texture_channels_gl, 
		texture_asset.width, 
		texture_asset.height, 
		0, 
		texture_channels_gl, 
		GL_UNSIGNED_BYTE, 
		texture_asset.data.get());

	// OpenGL texture settings
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return result;
}

gl::GLuint renderer::assimp_vram_loader::load_texturef(const asset::texture_assetf& texture_asset, bool mipmap)
{
	using namespace gl;

	auto texture_channels_gl = num_channels_to_gltype(texture_asset.channels);
	gl::GLuint result;

	// Create OpenGL representation
	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		texture_channels_gl,
		texture_asset.width,
		texture_asset.height,
		0,
		texture_channels_gl,
		GL_FLOAT,
		texture_asset.data.get());

	// OpenGL texture settings
	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return result;
}


renderer::opengl_cubemap renderer::assimp_vram_loader::load_cubemap(
	const asset::texture_asset& left, 
	const asset::texture_asset& top, 
	const asset::texture_asset& front, 
	const asset::texture_asset& bottom, 
	const asset::texture_asset& right, 
	const asset::texture_asset& back)
{
	using namespace gl;

	//  GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515, // decimal value: 34069
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516, // decimal value: 34070
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517, // decimal value: 34071
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518, // decimal value: 34072
	//	GL_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519, // decimal value: 34073
	//	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A, // decimal value: 34074
	std::vector<const asset::texture_asset*> faces
	{
		&right,
		&left,
		&top,
		&bottom,
		&back,
		&front		
	};

	renderer::opengl_cubemap result;
	result.cubemap.texture_id = load_cubemap(faces);

	glGenVertexArrays(1, &result.vao);
	glGenBuffers(1, &result.vbo);
	glBindVertexArray(result.vao);
	glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shapes::CubeVertices), &shapes::CubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Eigen::Vector3f), nullptr);

	return result;
}

gl::GLuint renderer::assimp_vram_loader::load_cubemap(std::vector<const asset::texture_asset*> faces)
{
	using namespace gl;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		
	for (size_t i = 0; i < faces.size(); i++)
	{
		const asset::texture_asset& texture_asset = *faces[i];
		auto format = num_channels_to_gltype(texture_asset.channels);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			format,
			texture_asset.width,
			texture_asset.height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			texture_asset.data.get());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}



gl::GLenum renderer::assimp_vram_loader::num_channels_to_gltype(int num_channels)
{
	using namespace gl;
	GLenum texture_channels_gl;
	switch (num_channels)
	{
	case 1:
		return GL_RED;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;

	default:
		throw std::runtime_error("Error: Unexpected amount of channels while loading texture.");
	}
}