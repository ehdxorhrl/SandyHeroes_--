#pragma once
#include <memory>

class Component;

struct CollideType
{
	bool ground_check = false;	//지면 체크가 필요한가?	
	bool wall_check = false;	//벽 체크가 필요한가?
};

// Scene에 등장하는 오브젝트 클래스
// 자식과 형제 노드를 가진 트리구조

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	Object(const std::string& name);
	virtual ~Object();

	Object(const Object& other);

	//getter
	// 변환행렬 및 각 벡터
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;
	bool is_player() const { return is_player_; }

	// 월드행렬 및 각 벡터
	XMFLOAT4X4 world_matrix() const;
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;

	UINT id() const;
	std::string name() const;
	std::string tag() const;
	Object* child() const;
	Object* sibling() const;
	Object* parent() const;
	bool is_ground() const;

	CollideType collide_type() const;

	XMFLOAT3 local_scale() const { return local_scale_; }
	XMFLOAT3 local_rotation() const { return local_rotation_; }
	XMFLOAT3 local_position() const { return local_position_; }	//로컬 좌표계

	bool is_movable() const { return is_movable_; }
	bool is_in_view_sector() const { return is_in_view_sector_; } // 카메라 절두체 컬링을 통과한 섹터에 있는가?

	float life_time() const { return life_time_; } // 오브젝트의 생존 시간

	//setter
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_position_vector(float x, float y, float z);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);

	void set_name(const std::string& value);
	void set_id(const long long id);
	void set_tag(const std::string& value);
	void set_is_ground(bool on_ground);
	void set_collide_type(bool ground_check, bool wall_check);
	void set_collide_type(const CollideType& collide_type);

	void set_local_scale(const XMFLOAT3& value); 
	void set_local_rotation(const XMFLOAT3& value); 
	void set_local_position(const XMFLOAT3& value); 
	void set_is_player() { is_player_ = true; }

	void set_is_movable(bool value); 
	void set_is_in_view_sector(bool value) { is_in_view_sector_ = value; } // 카메라 절두체 컬링을 통과한 섹터에 있는가?

	//SRT 정보를 transform_matrix_로 초기화한다.
	void ResetSRTFromTransformMatrix();

	void AddChild(std::shared_ptr<Object> object);
	void AddSibling(std::shared_ptr<Object> object);
	void AddComponent(std::shared_ptr<Component> component);

	std::shared_ptr<Object> FindFrame(const std::string& name);
	std::shared_ptr<Object> GetHierarchyRoot();

	void DeleteChild(const std::string& name);
	void ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete = true);

	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 

	virtual void Update(float elapsed_time);

	void Rotate(float pitch, float yaw, float roll);
	void Scale(float value);				

	// Applies the func to the object and all its descendants in the hierarchy.
	void EnableFuncInHeirachy(std::function<void(Object*, void*)> func, void* value);

	void OnDestroy(std::function<void(Object*)> func);
	void Destroy();
	static std::shared_ptr<Object> DeepCopy(const std::shared_ptr<Object>& value, const std::shared_ptr<Object>& parent = nullptr);

	template<class T>
	static std::shared_ptr<T> GetComponent(std::shared_ptr<Object> object)
	{
		for (auto& component : object->component_list_)
		{
			if (std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(component))
				return res;
		}
		return nullptr;
	}

	template<class T>
	static std::list<std::shared_ptr<T>> GetComponents(std::shared_ptr<Object> object)
	{
		std::list<std::shared_ptr<T>> r_value;
		for (auto& component : object->component_list_)
		{
			if (std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(component))
			{
				r_value.push_back(res);
			}
		}
		return r_value;
	}

	template<class T>
	static std::shared_ptr<T> GetComponentInChildren(std::shared_ptr<Object> object)
	{
		std::shared_ptr<T> component = GetComponent<T>(object);
		if (component)
			return component;

		if (object->sibling_)
		{
			component = GetComponentInChildren<T>(object->sibling_.get());
			if (component)
				return component;
		}
		if (object->child_)
			return GetComponentInChildren<T>(object->child_.get());

		return nullptr;
	}

	template<class T>
	static std::list<std::shared_ptr<T>> GetComponentsInChildren(std::shared_ptr<Object> object)
	{
		std::list<std::shared_ptr<T>> component_list = GetComponents<T>(object);
		std::list<std::shared_ptr<T>> r_value;

		for (auto& component : component_list)
		{
			r_value.push_back(component);
		}

		if (object->sibling_)
		{
			auto sibling_components = GetComponentsInChildren<T>(object->sibling_.get());
			for (auto& component : sibling_components)
			{
				r_value.push_back(component);
			}
		}
		if (object->child_)
		{
			auto child_components = GetComponentsInChildren<T>(object->child_.get());
			for (auto& component : child_components)
			{
				r_value.push_back(component);
			}
		}

		return r_value;
	}

protected:
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	XMFLOAT3 local_scale_{ 1.f, 1.f, 1.f };	
	XMFLOAT3 local_rotation_{}; //오일러각
	XMFLOAT3 local_position_{};

	std::weak_ptr<Object> parent_;
	std::shared_ptr<Object> child_;
	std::shared_ptr<Object> sibling_;

	std::list<std::shared_ptr<Component>> component_list_;

	std::string name_ = "None";
	std::string tag_ = "None_Tag";	//오브젝트를 "분류"하기 위한 태그 값 ex) "Player", "HitDragon", "ShotDragon"
	bool is_player_ = false;

	float life_time_ = 0.f;	//오브젝트의 생존 시간

	//물리 관련 변수들
	bool is_ground_ = false;	//지면에 닿아있는가?

	//충돌 체크 관련 변수
	CollideType collide_type_ = { false, false };	//지면 체크, 벽 체크

	bool is_movable_ = false;	//이동 가능한가? 

	bool is_in_view_sector_ = false; // 카메라 절두체 컬링을 통과한 섹터에 있는가?

	std::function<void(Object*)> on_destroy_func_ = nullptr;	//오브젝트가 파괴될 때 호출되는 함수

private:
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	static UINT kObjectNextId;
	UINT id_ = 0;
	
};

