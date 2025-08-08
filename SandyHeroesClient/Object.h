#pragma once

class Component;

struct CollideType
{
	bool ground_check = false;	//지면 체크가 필요한가?	
	bool wall_check = false;	//벽 체크가 필요한가?
};

// Scene에 등장하는 모든 오브젝트의 조상 클래스
// 자식과 형제 노드를 가진 트리구조

class Object
{
public:
	Object();
	Object(const std::string& name);
	virtual ~Object();

	//복사 생성자(child, sibling도 복사하는 깊은복사)
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
	bool is_dead() const;	//죽은 오브젝트인가?
	UINT dead_frame_count() const { return dead_frame_count_; }	//죽은 후 프레임 카운트

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
	void set_is_dead(bool is_dead);

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

	void AddChild(Object* object);
	void AddSibling(Object* object);
	void AddComponent(Component* component);

	Object* FindFrame(const std::string& name);
	Object* GetHierarchyRoot();

	void DeleteChild(const std::string& name);
	void KillChild(const std::string& name);
	Object* PopDeadChild();	// 죽은 자식 오브젝트를 찾아서 자식 노드에서 제거하고 반환한다.
	void ChangeChild(Object* src, const std::string& dst_name, bool is_delete = true);

	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 

	virtual void Update(float elapsed_time);

	void Rotate(float pitch, float yaw, float roll);
	void Scale(float value);				

	// Applies the func to the object and all its descendants in the hierarchy.
	void EnableFuncInHeirachy(std::function<void(Object*, void*)> func, void* value);

	void OnDestroy(std::function<void(Object*)> func);
	void Destroy();
	void AddDeadFrameCount(UINT frame_count);

	static Object* DeepCopy(Object* value, Object* parent = nullptr);

	template<class T>
	static T* GetComponent(Object* object)
	{
		for (auto& component : object->component_list_)
		{
			if (dynamic_cast<T*>(component.get()))
				return static_cast<T*>(component.get());
		}
		return nullptr;
	}

	template<class T>
	static std::list<T*> GetComponents(Object* object)
	{
		std::list<T*> r_value;
		for (auto& component : object->component_list_)
		{
			if (dynamic_cast<T*>(component.get()))
			{
				r_value.push_back(static_cast<T*>(component.get()));
			}
		}
		return r_value;
	}

	template<class T>
	static T* GetComponentInChildren(Object* object)
	{
		T* component = GetComponent<T>(object);
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
	static std::list<T*> GetComponentsInChildren(Object* object)
	{
		std::list<T*> component_list = GetComponents<T>(object);
		std::list<T*> r_value;

		for (T* component : component_list)
		{
			r_value.push_back(component);
		}

		if (object->sibling_)
		{
			component_list = GetComponentsInChildren<T>(object->sibling_);
			for (T* component : component_list)
			{
				r_value.push_back(component);
			}
		}
		if (object->child_)
		{
			component_list = GetComponentsInChildren<T>(object->child_);
			for (T* component : component_list)
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

	Object* parent_ = nullptr;
	Object* child_ = nullptr;
	Object* sibling_ = nullptr;

	std::list<std::unique_ptr<Component>> component_list_;

	std::string name_ = "None";
	std::string tag_ = "None_Tag";	//오브젝트를 "분류"하기 위한 태그 값 ex) "Player", "HitDragon", "ShotDragon"

	bool is_dead_ = false;	//죽은 오브젝트인가?	
	bool is_player_ = false;
	UINT dead_frame_count_ = 0;	//오브젝트가 죽은 후 프레임 카운트

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

