#pragma once
#include "AnimationSet.h"

class Object;
class Mesh;
class Material;

class ModelInfo
{
public:
	ModelInfo() {}
	ModelInfo(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);
	~ModelInfo();

	std::string model_name() const;

	void LoadModelInfoFromFile(const std::string& file_name, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);

	Object* LoadFrameInfoFromFile(std::ifstream& file, std::vector<std::unique_ptr<Mesh>>& meshes,
		std::vector<std::unique_ptr<Material>>& materials);

	void LoadAnimationInfoFromFile(std::ifstream& file);

	Object* GetInstance();

	Object* hierarchy_root() const;

private:
	std::string model_name_ = "None";

	Object* hierarchy_root_ = nullptr;

	//각 모델 정보는 모델에 맞는 애니메이션 정보들을 가지고 있을수 있다. 
	//이것들은 AnimatorComponent에 연결되어 작동한다.
	std::vector<std::unique_ptr<AnimationSet>> animation_sets_;
	//위 애니메이션 set에 저장된 애니메이션 정보의 프레임 이름 정보다. 
	//이름 순서와 애니메이션의 transform matrix 순서가 일치하기 때문에 이를 통해 애니메이션을 적용한다.
	std::vector<std::string> frame_names_;
	//애니메이션이 적용되는 뼈대의 루트본 이름(루트모션 애니메이션 및 제자리 애니메이션을 위해 필요함)
	std::string root_bone_name_;
};

