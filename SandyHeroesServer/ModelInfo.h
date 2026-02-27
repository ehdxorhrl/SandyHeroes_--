#pragma once
#include "AnimationSet.h"

class Object;
class Mesh;
class Material;
struct Texture;

class ModelInfo
{
public:
	ModelInfo() {}
	ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);
	~ModelInfo();

	std::string model_name() const;

	void LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);

	std::shared_ptr<Object> LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials, std::vector<std::unique_ptr<Texture>>& textures);

	void LoadAnimationInfoFromFile(std::ifstream& file);

	std::shared_ptr<Object> GetInstance() const;

	std::shared_ptr<Object> hierarchy_root() const;

	void set_hierarchy_root(std::shared_ptr<Object> root);
	void set_model_name(const std::string& value) { model_name_ = value; }

private:
	std::string model_name_ = "None";

	std::shared_ptr<Object> hierarchy_root_ = nullptr;

	std::vector<std::unique_ptr<AnimationSet>> animation_sets_;
	std::vector<std::string> frame_names_;
	std::string root_bone_name_;
};

