#include "Model.h"
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
void Model::loadModel(const void * s)
{
	const aiScene * scene = reinterpret_cast<const aiScene*>(s);
	processNode(scene->mRootNode, scene);
}

void Model::processNode(void * n, const void * s)
{
	aiNode * node = reinterpret_cast<aiNode*>(n);
	const aiScene * scene = reinterpret_cast<const aiScene*>(s);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene);
}

Mesh * Model::processMesh(void * ai_mesh, const void * _scene)
{
	aiMesh * mesh = reinterpret_cast<aiMesh*>(ai_mesh);
	const aiScene * scene = reinterpret_cast<const aiScene*>(_scene);
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		vertex v;
		v.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0])
			v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			v.texCoord = glm::vec2(0.f);
		if (mesh->mTangents)
			v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		else
			v.tangent = glm::vec3(0.f);
		vertices.push_back(v);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
	aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
	auto diffuseList = loadMaterialTexture(material, aiTextureType_DIFFUSE);
	auto specList = loadMaterialTexture(material, aiTextureType_SPECULAR);
	auto normList = loadMaterialTexture(material, aiTextureType_HEIGHT);
	textures.insert(textures.end(), diffuseList.begin(), diffuseList.end());
	textures.insert(textures.end(), specList.begin(), specList.end());
	textures.insert(textures.end(), normList.begin(), normList.end());
	return new Mesh(vertices, indices, textures, ibo, maxInstances);
}

std::vector<Texture*> Model::loadMaterialTexture(void * material, int type)
{
	aiMaterial * mat = reinterpret_cast<aiMaterial*>(material);
	aiTextureType t = (aiTextureType)type;
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(t); ++i) {
		aiString str;
		mat->GetTexture(t, i, &str);
		std::string filename = str.C_Str();
		if(filename.find('\\') != filename.npos)
			filename = filename.substr(filename.find_last_of('\\'));
		std::string texPath = texDirectory + '\\' + filename;
		Texture * tex;
		auto it = std::find(loadedTextures.begin(), loadedTextures.end(), str.C_Str());
		if (it == loadedTextures.end()) {
			tex = new Texture(texPath.c_str());
			textureType tt;
			switch (t) {
			case aiTextureType_SPECULAR:
				tt = textureType::specular;
				break;
			case aiTextureType_HEIGHT:
				tt = textureType::normal;
				break;
			case aiTextureType_AMBIENT:
				tt = textureType::height;
				break;
			default:
				tt = textureType::diffuse;
				break;
			}
			tex->setTextureType(tt);			
		}
		else 
			tex = new Texture(textures[it - loadedTextures.begin()]);
		textures.push_back(tex);
		loadedTextures.push_back(str.C_Str());
	}
	return textures;
}

Model::Model(const char * path, const char * texDirectory)
{
	if (texDirectory != nullptr)
		this->texDirectory = texDirectory;
	else {
		std::string pr = path;
		this->texDirectory = pr.substr(0, pr.find_last_of('\\'));
	}
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	loadModel(scene);
}

Model::Model(int resourceId, int resourceType)
{
	Resources res(resourceId, resourceType);
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFileFromMemory(res.data, res.length, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	loadModel(scene);
}
Model::Model(const char * path, std::vector<glm::mat4> & instanceTransforms, const char * texDirectory)
{
	instanced = true;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceTransforms.size(), instanceTransforms.data(), GL_DYNAMIC_DRAW);
	instanceMap = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	maxInstances = instanceTransforms.size();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (texDirectory != nullptr)
		this->texDirectory = texDirectory;
	else {
		std::string pr = path;
		this->texDirectory = pr.substr(0, pr.find_last_of('\\'));
	}
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	loadModel(scene);
}


Model::~Model()
{
	for (Mesh * m : meshes)
		delete m;
	if (ibo != nullbuf) {
		glDeleteBuffers(1, &ibo);
	}
}

void Model::draw(const Shader * s) const
{
	//Shader is used in Engine code
	s->setMat4("model", calcModel());
	s->setFloat("shininess", shininess);
	for (Mesh * m : meshes)
		m->draw(s);
}
bool Model::updateInstance(unsigned int index, glm::mat4 & mat) {
	assert(index < maxInstances && "Index out of bounds!");
	if (ibo == nullbuf) return false;
	memcpy((glm::mat4*)instanceMap + index, glm::value_ptr(mat), sizeof(glm::mat4));
	return true;
}

Mesh::Mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures, unsigned int ibo, unsigned int instances) : indices(indices), textures(textures), ibo(ibo), instances(instances)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));
	glEnableVertexAttribArray(3);
	if (ibo != nullbuf) {
		glBindBuffer(GL_ARRAY_BUFFER, ibo);
		glVertexAttribPointer(4, 16, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 0);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(4, 1);
	}

}

void Mesh::draw(const Shader * s) const
{
	int diffCount = 0, specCount = 0, normCount = 0;
	for (int i = 0; i < textures.size(); ++i) {
		std::string type;
		int count;
		int id = 0;
		bool unimplemented = false;
		switch (textures[i]->getTextureType()) {
		case textureType::specular:
			type = "specular";
			count = specCount++;
			id = 1;
			break;
		case textureType::diffuse:
			type = "diffuse";
			id = 0;
			count = diffCount++;
			break;
		case textureType::normal:
			type = "normal";
			id = 2;
			count = normCount++;
			break;
		default:
			unimplemented = true;
		}
		if (unimplemented) continue;
		std::stringstream uniform;
		uniform << "texture_" << type; // << count;
//		s->setInt(uniform.str().c_str(), i);
		textures[i]->setId(id);
		textures[i]->bind();
	}
	if (normCount) s->setBool("useNormalMap", true);
	if (specCount) s->setBool("useSpecMap", true);
	glBindVertexArray(vao);
	if(ibo == nullbuf) glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	else glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0, instances);
	if (normCount) s->setBool("useNormalMap", false);
	if (specCount) s->setBool("useSpecMap", false);

}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	for (Texture * t : textures)
		delete t;
}
