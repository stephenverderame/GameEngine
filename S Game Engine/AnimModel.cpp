#include "AnimModel.h"
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
struct importer {
	Assimp::Importer imp;
};
glm::mat4 assimpToGlm(aiMatrix4x4 mat) {
	glm::mat4 m;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] = mat[i][j];
		}
	}
	return m;
}
glm::vec3 assimpToGlm(aiVector3D vec) {
	glm::vec3 v;
	v.x = vec.x;
	v.y = vec.y;
	v.z = vec.z;
	return v;
}
glm::quat assimpToGlm(aiQuaternion q) {
	glm::quat qt;
	qt.w = q.w;
	qt.x = q.x;
	qt.y = q.y;
	qt.z = q.z;
	return qt;
}
aiNodeAnim * findNodeAnim(aiAnimation * anim, std::string nodeName) {
	for (int i = 0; i < anim->mNumChannels; ++i) {
//		printf("Anim node name:%s\n", anim->mChannels[i]->mNodeName.C_Str());
		if (nodeName.compare(anim->mChannels[i]->mNodeName.C_Str()) == 0)
			return anim->mChannels[i];
	}
//	return nullptr;
	printf("Could not find animation: %s\n", nodeName.c_str());
	return anim->mChannels[0];
}
int findLastPosition(double time, aiNodeAnim * node) {
	for (int i = 0; i < node->mNumPositionKeys - 1; ++i)
		if (time < node->mPositionKeys[i + 1].mTime) return i;
	return -1;
}
int findLastRotation(double time, aiNodeAnim * node) {
	for (int i = 0; i < node->mNumRotationKeys - 1; ++i)
		if (time < node->mRotationKeys[i + 1].mTime) return i;
	return -1;
}
int findLastScaling(double time, aiNodeAnim * node) {
	for (int i = 0; i < node->mNumScalingKeys - 1; ++i)
		if (time < node->mRotationKeys[i + 1].mTime) return i;
}
glm::vec3 Animation::interpolatePosition(double time, void * animationNode)
{
	aiNodeAnim * node = reinterpret_cast<aiNodeAnim*>(animationNode);
	if (node->mNumPositionKeys == 1)
		return assimpToGlm(node->mPositionKeys[0].mValue);
	int pos = findLastPosition(time, node);
	double dt = node->mPositionKeys[pos + 1].mTime - node->mPositionKeys[pos].mTime;
	double factor = (time - node->mPositionKeys[pos].mTime) / dt;
	glm::vec3 diff = assimpToGlm(node->mPositionKeys[pos + 1].mValue - node->mPositionKeys[pos].mValue);
	diff.x *= factor;
	diff.y *= factor;
	diff.z *= factor;
	return diff;
}

glm::quat Animation::interpolateRotation(double time, void * animationNode)
{
	aiNodeAnim * node = reinterpret_cast<aiNodeAnim*>(animationNode);
	if (node->mNumRotationKeys == 1)
		return assimpToGlm(node->mRotationKeys[0].mValue);
	int pos = findLastRotation(time, node);
	double dt = node->mRotationKeys[pos + 1].mTime - node->mRotationKeys[pos].mTime;
	double factor = (time - node->mRotationKeys[pos].mTime) / dt;
	return glm::slerp(assimpToGlm(node->mRotationKeys[pos].mValue), assimpToGlm(node->mRotationKeys[pos + 1].mValue), (float)factor);
}

glm::vec3 Animation::interpolateScaling(double time, void * animationNode)
{
	aiNodeAnim * node = reinterpret_cast<aiNodeAnim*>(animationNode);
	if (node->mNumScalingKeys == 1)
		return assimpToGlm(node->mScalingKeys[0].mValue);
	int pos = findLastScaling(time, node);
	double dt = node->mScalingKeys[pos + 1].mTime - node->mScalingKeys[pos].mTime;
	double factor = (time - node->mScalingKeys[pos].mTime) / dt;
	glm::vec3 diff = assimpToGlm(node->mScalingKeys[pos + 1].mValue - node->mScalingKeys[pos].mValue);
	diff.x *= factor;
	diff.y *= factor;
	diff.z *= factor;
	return diff;
}
int Animation::findBone(std::string name)
{
	for (int i = 0; i < bones.size(); ++i) {
		if (bones[i].name.compare(name) == 0) return i;
	}
	printf("Could not find: %s\n", name.c_str());
	return -1;
}
void Animation::populateBoneTransforms(double time)
{
	aiAnimation * anim = reinterpret_cast<aiAnimation*>(animation);
	double tps = anim->mTicksPerSecond > 0 ? anim->mTicksPerSecond : 25; //ticks per second is 0 if not specefied
	double ticks = time * tps;
	float t = fmod(ticks, anim->mDuration);
	readNodeHeirarchy(t, root, glm::mat4());
	for (int i = 0; i < bones.size(); ++i)
		boneTransforms[i] = bones[i].finalTransformation; 
}
void Animation::readNodeHeirarchy(double time, void * node, glm::mat4 parentTransform)
{
	aiNode * n = reinterpret_cast<aiNode*>(node);
	std::string nodeName(n->mName.C_Str());
//	printf("node name: %s\n", n->mName.C_Str());
	glm::mat4 nodeTransformation = assimpToGlm(n->mTransformation);
	aiNodeAnim * nodeAnim = findNodeAnim((aiAnimation*)animation, nodeName);
	assert(nodeAnim != nullptr && "Could not find animation node!");
	glm::vec3 pos = interpolatePosition(time, nodeAnim);
	glm::vec3 scale = interpolatePosition(time, nodeAnim);
	glm::quat rot = interpolateRotation(time, nodeAnim);
	
	glm::mat4 posMat = glm::translate(glm::mat4(), pos);
	glm::mat4 rotMat = glm::mat4_cast(rot);
	glm::mat4 sclMat = glm::scale(glm::mat4(), scale);

	glm::mat4 globalTransform = posMat * rotMat * sclMat;


	globalTransform = assimpToGlm(n->mTransformation) * parentTransform;// * /*globalTransform */ assimpToGlm(n->mTransformation);
	int i = findBone(nodeName);
	if (i == -1) printf("Could not find bone!\n");
	else
		bones[i].finalTransformation = globalInverse * globalTransform * bones[i].offsetMatrix; // globalInverse * globalTransform * bones[i].offsetMatrix;
	for (int j = 0; j < n->mNumChildren; ++j)
		readNodeHeirarchy(time, n->mChildren[j], globalTransform);

}

Animation::Animation(glm::mat4 gi, std::vector<bone> b, void * root, void * anim) : globalInverse(gi),
root(root), animation(anim)
{
	timer = clock();
	bones.insert(bones.end(), b.begin(), b.end());
	boneTransforms.resize(b.size());
	timer.reset();
	aiAnimation * a  = reinterpret_cast<aiAnimation*>(anim);
	name = a->mName.C_Str();
}

void Animation::update()
{
	populateBoneTransforms(timer.getElapsed<units::secs>());
}

void Animation::bufferData(const Shader * shader) const
{
	//shader is used in engine code
	shader->setMArray("bones", boneTransforms.data(), boneTransforms.size());
}

AnimMesh::AnimMesh(std::vector<animVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures) : vertices(vertices), indices(indices), textures(textures)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(animVertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(animVertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(animVertex), (void*)offsetof(animVertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(animVertex), (void*)offsetof(animVertex, texCoord));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(animVertex), (void*)offsetof(animVertex, bone_ids));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(animVertex), (void*)offsetof(animVertex, bone_weights));
	glEnableVertexAttribArray(4);
}

void AnimMesh::draw(const Shader * s) const
{
	int diffCount = 0, specCount = 0;
	for (int i = 0; i < textures.size(); ++i) {
		std::string type;
		int count;
		switch (textures[i]->getTextureType()) {
		case textureType::specular:
			type = "specular";
			count = specCount++;
			break;
		default:
			type = "diffuse";
			count = diffCount++;
			break;
		}
		std::stringstream uniform;
		uniform << "texture_" << type; // << count;
		s->setInt(uniform.str().c_str(), i);
		textures[i]->setId(i);
		textures[i]->bind();
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

AnimMesh::~AnimMesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	for (Texture * t : textures)
		delete t;
}

void AnimModel::loadModel(const void * s)
{
	const aiScene * scene = reinterpret_cast<const aiScene*>(s);
	inverseTransform = assimpToGlm(scene->mRootNode->mTransformation.Inverse());
	processNode(scene->mRootNode, scene);
}

void AnimModel::processNode(void * n, const void * s)
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

AnimMesh * AnimModel::processMesh(void * ai_mesh, const void * _scene)
{
	aiMesh * mesh = reinterpret_cast<aiMesh*>(ai_mesh);
	const aiScene * scene = reinterpret_cast<const aiScene*>(_scene);
	std::vector<animVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		animVertex v;
		ZeroMemory(&v, sizeof(v));
		v.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0])
			v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			v.texCoord = glm::vec2(0.f);
		vertices.push_back(v);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
//	std::map<std::string, int> boneMap;
	std::vector<int> vertexBones(mesh->mNumVertices, 0);
	std::vector<bone> bones(mesh->mNumBones);
	for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
		aiBone * _bone = mesh->mBones[i];
//		boneMap.insert(std::make_pair(_bone->mName.C_Str(), i));
		for (unsigned int j = 0; j < _bone->mNumWeights; ++j) {
			decltype(auto) weight = _bone->mWeights[j];
			unsigned int vertexIndex = weight.mVertexId;
//			assert(vertexBones[vertexIndex] < BONES_PER_VERTEX && "Bones per vertex limit overflow!");
			if (vertexBones[vertexIndex] >= BONES_PER_VERTEX) continue;
			vertices[vertexIndex].bone_ids[vertexBones[vertexIndex]] = i;
			vertices[vertexIndex].bone_weights[vertexBones[vertexIndex]++] = weight.mWeight;
		}
		bones[i] = bone{ std::string(_bone->mName.C_Str()), assimpToGlm(_bone->mOffsetMatrix), glm::mat4() };
	}
	animations.push_back(std::make_unique<Animation>(inverseTransform, bones, scene->mRootNode, scene->mAnimations[0]));
	aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
	auto diffuseList = loadMaterialTexture(material, aiTextureType_DIFFUSE, "t_diffuse");
	auto specList = loadMaterialTexture(material, aiTextureType_SPECULAR, "t_specular");
	auto normList = loadMaterialTexture(material, aiTextureType_HEIGHT, "t_normal");
	textures.insert(textures.end(), diffuseList.begin(), diffuseList.end());
	textures.insert(textures.end(), specList.begin(), specList.end());
	textures.insert(textures.end(), normList.begin(), normList.end());
	return new AnimMesh(vertices, indices, textures);
}

std::vector<Texture*> AnimModel::loadMaterialTexture(void * material, int type, const char * name)
{
	aiMaterial * mat = reinterpret_cast<aiMaterial*>(material);
	aiTextureType t = (aiTextureType)type;
	std::vector<Texture*> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(t); ++i) {
		aiString str;
		mat->GetTexture(t, i, &str);
		std::string filename = str.C_Str();
		if (filename.find('\\') != filename.npos)
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

AnimModel::AnimModel(const char * path, const char * texDirectory) : activeAnim(0)
{
	std::string pr(path);
	if (texDirectory != nullptr)
		this->texDirectory = texDirectory;
	else
		this->texDirectory = pr.substr(0, pr.find_last_of('\\'));
	assimpImporter = std::make_unique<importer>();
	const aiScene * scene = assimpImporter->imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	loadModel(scene);
}

AnimModel::AnimModel(int resourceId, int resourceType) : activeAnim(0)
{
	Resources res(resourceId, resourceType);
	assimpImporter = std::make_unique<importer>();
	const aiScene * scene = assimpImporter->imp.ReadFileFromMemory(res.data, res.length, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	loadModel(scene);
}

AnimModel::~AnimModel()
{
	for (AnimMesh * m : meshes)
		delete m;
}

void AnimModel::draw(const Shader * s) const
{
	//Shader is used in Engine code
	s->setMat4("model", calcModel());
	animations[activeAnim]->update();
	animations[activeAnim]->bufferData(s);
	for (AnimMesh * m : meshes)
		m->draw(s);
}

void AnimModel::activeAnimation(size_t i)
{
	activeAnim = i;
	animations[i]->reset();
}

void AnimModel::activeAnimation(std::string name)
{
	for(size_t i = 0; i < animations.size(); ++i)
		if (animations[i]->getName() == name) {
			activeAnimation(i);
			return;
		}
	assert("Animation not found");
}
