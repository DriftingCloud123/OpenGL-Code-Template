#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.h>
#include <Shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model 
{
public:
    // 模型数据
    vector<Texture> textures_loaded;	// 存放所有已加载的纹理，避免重复加载
    vector<Mesh>    meshes;             // 存放所有网格
    string directory;                   // 存放模型目录
    bool gammaCorrection;               // 是否进行gamma校正

    // 构造函数
    // 传入模型文件路径，用于加载模型
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // 画出其所有meshes，即画出整个模型
    void Draw(Shader &shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
    
private:
    // 从文件中加载assimp扩展支持的模型，并将结果网格存储在meshes向量中
    void loadModel(string const &path)
    {
        // assimp导入器，用于读文件
        Assimp::Importer importer;
        // 读入文件
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // 检查错误
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // 检索该文件路径的目录路径
        directory = path.substr(0, path.find_last_of('/'));

        // 递归处理assimp的根节点，处理所有子节点
        processNode(scene->mRootNode, scene);
    }

    // 递归处理节点
    void processNode(aiNode *node, const aiScene *scene)
    {
        // 将该节点的所有网格都加入到meshes数组中
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // node对象只包括绘图索引的索引，实际的网格数据在scene中
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // 当处理完所有网格后，递归处理该节点的子节点
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    // 递归处理网格
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // 将要填充的数据
        vector<Vertex> vertices;        // 顶点，包括位置、法线、纹理坐标、切线、双切线
        vector<unsigned int> indices;   // 索引
        vector<Texture> textures;       // 纹理

        // 遍历所有网格的顶点，对这些顶点进行处理
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // 我们声明了一个用于存放assimp数据的占位符向量，因为assimp使用自己的向量类，不能直接转换为glm的vec3类，所以我们先把数据转移到这个占位符glm::vec3中。
            // 位置
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // 法线坐标
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // 纹理坐标
            if(mesh->mTextureCoords[0]) // 检查网格是否有纹理坐标？
            {
                // 纹理坐标
                glm::vec2 vec;
                // Assimp 允许一个顶点拥有最多 8 套不同的 UV 纹理坐标. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // 将顶点放到vertices向量中
            vertices.push_back(vertex);
        }

        // 遍历所有的面，并储存了面的索引到indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // 处理所有面的索引，并将他们存放到indices向量中
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

        // 处理纹理
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse  (漫反射纹理):   texture_diffuseN
        // specular (镜像反射纹理): texture_specularN
        // normal   (法线纹理):     texture_normalN
        // height   (高度纹理):     texture_heightN

        // 漫反射纹理
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 镜像纹理
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 法线纹理
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 高度纹理
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // 从提取的数据中创建一个mesh对象，并返回
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        // 遍历该类型的所有纹理
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;   // 纹理路径
            mat->GetTexture(type, i, &str);
            // 检查是否已经被加载过，如果被加载过则跳过
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;    // 同样路径下的纹理已经被加载过了，所以跳过
                    break;
                }
            }
            if(!skip)
            {   // 如果纹理还没有被加载过，则加载它
                Texture texture;

                texture.id = TextureFromFile(str.C_Str(), this->directory); // 纹理ID
                texture.type = typeName;        // 纹理类型
                texture.path = str.C_Str();     // 纹理路径

                textures.push_back(texture);    // 将纹理加入到textures向量中
                // 将加载的纹理路径名称存储起来，以确保不会重复加载
                textures_loaded.push_back(texture);
            }
        }
        // 返回所有暂存的指定类型纹理
        return textures;
    }
};


inline unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif