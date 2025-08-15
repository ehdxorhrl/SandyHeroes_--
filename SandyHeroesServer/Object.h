#pragma once
#include <memory>
#include <DirectXMath.h>
#include "MeshColliderComponent.h"
#include "XMathUtil.h"

class Component;
class MeshColliderComponent;

enum MonsterType {
	None = 0,
	Hit_Dragon,
	Shot_Dragon,
	Bomb_Dragon,
	Strong_Dragon,
	Super_Dragon = 14 - 2
};

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

	UINT id() const;

	void set_id(const long long id);

	//getter
	// 변환행렬 및 각 벡터
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;

	// 월드행렬 및 각 벡터
	XMFLOAT4X4 world_matrix() const;
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;
	XMFLOAT3 old_position() const;

	//UINT id() const;

	XMFLOAT3 velocity() const;
	std::string tag() const;
	Object* child() const;
	Object* sibling() const;
	bool is_ground() const;
	bool is_dead() const;
	bool is_player() const;
	bool is_movable() const { return is_movable_; }

	CollideType collide_type() const;

	void ApplyGravity(float elapsed_time);

	void Destroy();

	Object* PopDeadChild();

	XMFLOAT3 local_scale() const { return local_scale_; }
	XMFLOAT3 local_rotation() const { return local_rotation_; }
	XMFLOAT3 local_position() const { return local_position_; }	//로컬 좌표계
	MonsterType monster_type() const { return type_; }


	//setter
	// 변환행렬 및 각 벡터
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_position_vector(float x, float y, float z);
	void set_local_scale(const XMFLOAT3& value);
	void set_local_rotation(const XMFLOAT3& value);
	void set_local_position(const XMFLOAT3& value);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);
	void set_old_position(const XMFLOAT3& value);
	void set_is_player() { is_player_ = true; }	

	// 월드행렬의 setter는 지원하지 않는다.(상위노드의 의해 업데이트 되기 때문)

	void set_name(const std::string& value);
	void set_velocity(const XMFLOAT3& value);

	void set_is_ground(bool on_ground);
	void set_is_dead(bool is_dead);
	void set_is_movable(bool value);
	void set_tag(const std::string& value);
	void set_monster_type(MonsterType type);

	void AddChild(Object* object);
	void AddSibling(Object* object);
	void AddComponent(Component* component);

	Object* FindFrame(const std::string& name);
	void PrintFrameNamesRecursive(int depth);
	Object* GetHierarchyRoot();

	std::string name() const;

	// 노드를 순회하며 world_matrix를 업데이트한다.(최상위 노드의 경우 인자에 nullptr을 넣으면 된다)
	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform);

	virtual void Update(float elapsed_time);

	void Rotate(float pitch, float yaw, float roll);
	void Scale(float value);				// transform 의 스케일을 value로 균등 변환한다.

	void OnDestroy(std::function<void(Object*)> func);

	void ChangeChild(Object* src, const std::string& dst_name, bool is_delete);

	static Object* DeepCopy(Object* value, Object* parent = nullptr);

	void set_collide_type(bool ground_check, bool wall_check);

	void set_collide_type(const CollideType& collide_type);

	Object* GetCopy() const { return new Object(*this); }; // 총알을 위한 복사

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
	// 오브젝트의 변환행렬
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	XMFLOAT3 local_scale_{ 1.f, 1.f, 1.f };
	XMFLOAT3 local_rotation_{}; //오일러각
	XMFLOAT3 local_position_{};

	XMFLOAT3 old_position_{};

	Object* parent_ = nullptr;
	Object* child_ = nullptr;
	Object* sibling_ = nullptr;

	// 오브젝트에 추가된 모든 컴포넌트의 리스트
	std::list<std::unique_ptr<Component>> component_list_;

	std::string name_ = "None";
	std::string tag_ = "None_Tag";
	MonsterType type_;

	//물리 관련 변수들
	XMFLOAT3 velocity_{ 0,0,0 };
	const float gravity_ = { -9.8f };

	bool is_dead_ = false;	//죽은 오브젝트인가?	
	bool is_player_ = false;

	UINT dead_frame_count_ = 0;	//오브젝트가 죽은 후 프레임 카운트

	bool is_ground_ = false;

	bool is_movable_ = false;	//이동 가능한가? 

	CollideType collide_type_ = { false, false };	//지면 체크, 벽 체크

	bool is_in_view_sector_ = false; // 카메라 절두체 컬링을 통과한 섹터에 있는가?

	std::function<void(Object*)> on_destroy_func_ = nullptr;	//오브젝트가 파괴될 때 호출되는 함수

private:
	// 오브젝트의 실제 월드 행렬(즉, 상위노드의 변환이 전부 적용된)
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();
	static UINT kObjectNextId;
	UINT id_ = 0;
};

