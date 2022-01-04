#include <rendersystem/Model.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

void Model::Draw(Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale));
	model = glm::rotate(model, glm::radians(angle), axis);

	shader.setMat4("model", model);
	shader.setVec3("material.ambient", 0.0f, 0.0f, 0.0f);
	shader.setFloat("material.shininess", 32.0f);

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	std::cout << "processing meshes" << std::endl;

	processNode(scene->mRootNode, scene);

	std::cout << "meshes processed" << std::endl;

}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	std::cout << "processed " << node->mNumMeshes << " meshes" << std::endl;
	if (node->mNumChildren != 0) {
		std::cout << "processing " << node->mNumChildren << " children" << std::endl;

		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	if (!mesh->HasNormals() || !mesh->HasPositions() || !mesh->HasTextureCoords(0)) {
		std::cout << "Cannot Load Model, missing one of normals/positions/texturecoords" << std::endl;
	}

	auto aiVec3ToGlm = [](const aiVector3t<float>& in) {
		return glm::vec3(in.x, in.y, in.z);
	};

	// cet vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex v;
		
		v.Position = aiVec3ToGlm(mesh->mVertices[i]);
		v.Normal = aiVec3ToGlm(mesh->mNormals[i]);
		v.TexCoords.x = mesh->mTextureCoords[0][i].x;
		v.TexCoords.y = mesh->mTextureCoords[0][i].y;

		vertices.push_back(v);
	}
		
	// Set indices (note: guaranteed triangles due to aiProcess_Triangulate)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());

		std::vector<Texture> specMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specMaps.begin(), specMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

unsigned int Model::TextureFromFile(std::string path) {
	unsigned int texId;

	glGenTextures(1, &texId);

	int width, height, c;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(
		path.c_str(),
		&width,
		&height,
		&c,
		0);

	if (data == nullptr) {
		std::cout << "failed to load texture from " << path << std::endl;
		return -1;
	}
	else {
		GLenum format;
		if (c == 1) format = GL_RED;
		else if (c == 3) format = GL_RGB;
		else if (c == 4) format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texId;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}


		if (!skip) {
			auto fullpath = 
				directory +
				"/" +
				std::string(str.C_Str());

			Texture texture;
			texture.id = Model::TextureFromFile(fullpath);
			texture.type = typeName;
			texture.path = std::string(str.C_Str());
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

void Model::Rotate(float theta) {
	angle += theta;
}