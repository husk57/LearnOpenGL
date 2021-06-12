#ifndef model_hpp
#define model_hpp

#include <vector>
#include <mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool state);
class Model
{
    std::vector<Mesh> meshes;
    std::vector<Tex> textures_loaded;
    std::string directory;
    bool isFlip;
    unsigned int skybox;
    public:
        Model(const char *path, bool state, unsigned int cubemap)
        {
            this->isFlip = state;
            this->skybox = cubemap;
            loadModel(path);
            std::cout << "LOADED: " << path << std::endl;
        }
        
        void Draw(Shader &shader)
        {
            for(unsigned int i = 0; i < meshes.size(); i++) {
                meshes[i].Draw(shader, this->skybox);
            }
        }
    private:
        void loadModel(std::string path) {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
                return;
            }
            directory = path.substr(0, path.find_last_of('/'));
            processNode(scene->mRootNode, scene);
        };
        void processNode(aiNode *node, const aiScene *scene) {
            for(unsigned int i = 0; i < node->mNumMeshes; i++) {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
            for(unsigned int i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }
        };
        Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Tex> textures;
            
            for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;
                // process vertex positions, normals and texture coordinates
                glm::vec3 vec;
                vec.x = mesh->mVertices[i].x;
                vec.y = mesh->mVertices[i].y;
                vec.z = mesh->mVertices[i].z;
                
                vertex.position = vec;
                
                if (mesh->HasNormals()) {
                vec.x = mesh->mNormals[i].x;
                vec.y = mesh->mNormals[i].y;
                vec.z = mesh->mNormals[i].z;
                vertex.normal = vec;
                }
                
                if(mesh->mTextureCoords[0]) {
                    glm::vec2 vecTex;
                    vecTex.x = mesh->mTextureCoords[0][i].x;
                    vecTex.y = mesh->mTextureCoords[0][i].y;
                    vertex.texCoord = vecTex;
                }
                else {
                    vertex.texCoord = glm::vec2(0.0f, 0.0f);
                }
                //
                vertices.push_back(vertex);
            }
            
        // process indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        //
        // process material
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<Tex> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), std::make_move_iterator(diffuseMaps.begin()), std::make_move_iterator(diffuseMaps.end()));
            std::vector<Tex> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), std::make_move_iterator(specularMaps.begin()), std::make_move_iterator(specularMaps.end()));
        }
        //
        return Mesh(vertices, textures, indices);
    }

        std::vector<Tex> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
            std::vector<Tex> textures;
                    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
                    {
                        aiString str;
                        mat->GetTexture(type, i, &str);
                        bool skip = false;
                        for(unsigned int j = 0; j < textures_loaded.size(); j++)
                        {
                            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                            {
                                textures.push_back(textures_loaded[j]);
                                skip = true;
                                break;
                            }
                        }
                        if(!skip)
                        {
                            Tex texture;
                            texture.id = TextureFromFile(str.C_Str(), this->directory, this->isFlip);
                            texture.type = typeName;
                            texture.path = str.C_Str();
                            textures.push_back(texture);
                            textures_loaded.push_back(texture);
                        }
                    }
                    return textures;
                }
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool state)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(state);
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else {
            format = GL_RGBA;
        }
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
