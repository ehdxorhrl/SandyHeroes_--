#pragma once
#include <memory>

class Component;

struct CollideType
{
	bool ground_check = false;	
	bool wall_check = false;	
};

class Object : public std::enable_shared_from_this<Object>
{
public:
	Object();
	Object(const std::string& name);
	virtual ~Object();

	Object(const Object& other);

	//getter
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;
	bool is_player() const { return is_player_; }

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
	XMFLOAT3 local_position() const { return local_position_; }	

	bool is_movable() const { return is_movable_; }
	bool is_in_view_sector() const { return is_in_view_sector_; } 

	float life_time() const { return life_time_; } 

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
	void set_is_in_view_sector(bool value) { is_in_view_sector_ = value; } 

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
			if (component->GetTypeInfo()->IsKindOf(&T::kTypeInfo))
				return std::static_pointer_cast<T>(component);
		}
		return nullptr;
	}

	template<class T>
	static std::list<std::shared_ptr<T>> GetComponents(std::shared_ptr<Object> object)
	{
		std::list<std::shared_ptr<T>> r_value;
		for (auto& component : object->component_list_)
		{
			if (component->GetTypeInfo()->IsKindOf(&T::kTypeInfo))
			{
				r_value.push_back(std::static_pointer_cast<T>(component));
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
			component = GetComponentInChildren<T>(object->sibling_);
			if (component)
				return component;
		}
		if (object->child_)
			return GetComponentInChildren<T>(object->child_);

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
			auto sibling_components = GetComponentsInChildren<T>(object->sibling_);
			r_value.splice(r_value.end(), sibling_components);
		}
		if (object->child_)
		{
			auto child_components = GetComponentsInChildren<T>(object->child_);
			r_value.splice(r_value.end(), child_components);
		}

		return r_value;
	}

protected:
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	XMFLOAT3 local_scale_{ 1.f, 1.f, 1.f };	
	XMFLOAT3 local_rotation_{};
	XMFLOAT3 local_position_{};

	std::weak_ptr<Object> parent_;
	std::shared_ptr<Object> child_;
	std::shared_ptr<Object> sibling_;

	std::list<std::shared_ptr<Component>> component_list_;

	std::string name_ = "None";
	std::string tag_ = "None_Tag";	
	bool is_player_ = false;

	float life_time_ = 0.f;	
	bool is_ground_ = false;	

	CollideType collide_type_ = { false, false };	

	bool is_movable_ = false;	

	bool is_in_view_sector_ = false; 

	std::function<void(Object*)> on_destroy_func_ = nullptr;	
private:
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	static UINT kObjectNextId;
	UINT id_ = 0;
	
};

