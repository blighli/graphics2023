// Created by lonelyinnovator on 2023/2/22 0:04.
/**
 * Load all meshes in model
 */

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "file_process.h"
#include "user_exception.hpp"


/**
 * Load texture from file.
 * @param path filename
 * @param directory file directory
 * @param gamma gamma
 * @return texture id
 */
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);


/**
 * Load all meshes, textures etc. in model.
 * Examples:
 *   Model model(path, seg_path);
 *   model.draw(shader);
 */
class Model {
 public:
  /**
   * Constructor, expects a filepath to a 3D model.
   * @param model_path model file path
   * @param segPath
   * @param gamma
   */
  explicit Model(const std::string &model_path, const std::string &segPath = "", bool gamma = false);

  /**
   * Deconstruct
   */
  ~Model();

  /**
   * Draws the model, and thus all its meshes.
   * @param shader shader to draw model
   */
  void Draw(Shader &shader);

  /**
   * Delete object of all mesh
   */
  void DeleteAllMeshesObject();

  /**
   * Get loaded textures in model.
   * @return loaded textures in model.
   */
  [[nodiscard]] const std::vector<Texture> &GetTexturesLoaded() const;

  /**
   * Get all meshes in model.
   * @return all meshes in model
   */
  [[nodiscard]] const std::vector<Mesh> &GetMeshes() const;

  /**
   * Get model directory.
   * @return model directory
   */
  [[nodiscard]] const std::string &GetDirectory() const;

  /**
   * Get model name(e.g. 21).
   * @return model name
   */
  [[nodiscard]] const std::string &GetName() const;

  /**
   * Get model suffix(e.g. .off).
   * @return model suffix
   */
  [[nodiscard]] const std::string &GetSuffix() const;

  /**
   * Get gamma_correction.
   * @return gamma_correction
   */
  [[nodiscard]] bool GetGammaCorrection() const;

 private:

  /**
   * loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
   * @param path model path
   */
  void LoadModel(const std::string &path);

  /**
   * processes a node in a recursive fashion.
   * Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
   * @param node ASSIMP node
   * @param scene ASSIMP scene
   */
  void ProcessNode(aiNode *node, const aiScene *scene);

  /**
   * Process mesh and save in the array meshes.
   * @param mesh ASSIMP mesh
   * @param scene ASSIMP scene
   * @return
   */
  Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

  /**
   * checks all material textures of a given type and loads the textures if they're not loaded yet.
   * the required info is returned as a Texture struct.
   * @param mat ASSIMP material
   * @param type texture type
   * @param type_name texture type name
   * @return texture array
   */
  std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string type_name);

  // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  std::vector<Texture> textures_loaded_{};
  // model meshes
  std::vector<Mesh> meshes_{};
  // model dir
  std::string directory_{};
  // model name
  std::string name_{};
  // model suffix (.off, .obj, ...)
  std::string suffix_{};

  bool gamma_correction_{true};
};


#endif //MODEL_H
