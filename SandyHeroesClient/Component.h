#pragma once
#include <memory>

class Object;

// 컴포넌트의 런타임 타입 정보 노드(Unreal UClass / Unity RTTI와 동일한 구조)
// 각 클래스가 정적 노드 하나를 가지며 base 포인터로 부모 체인을 형성한다.
struct ComponentTypeInfo
{
	const char* name;
	const ComponentTypeInfo* base;   // 루트(Component)는 nullptr

	// 부모 체인을 따라가며 target과 같은 노드가 있으면 is-a 관계이다.
	bool IsKindOf(const ComponentTypeInfo* target) const
	{
		for (const ComponentTypeInfo* t = this; t; t = t->base)
			if (t == target) return true;
		return false;
	}
};

// Component를 상속받는 클래스 본문(첫 public: 아래 권장)에 한 줄 추가한다.
// 생성자는 손대지 않는다. C++20의 static inline 멤버라 .cpp 정의가 필요 없다.
#define DECLARE_COMPONENT(Self, Parent)                                          \
	static inline const ComponentTypeInfo kTypeInfo{ #Self, &Parent::kTypeInfo }; \
	const ComponentTypeInfo* GetTypeInfo() const override { return &kTypeInfo; }

class Component : public std::enable_shared_from_this<Component>
{
public:
	static inline const ComponentTypeInfo kTypeInfo{ "Component", nullptr };
	virtual const ComponentTypeInfo* GetTypeInfo() const { return &kTypeInfo; }

	Component() {};
	Component(Object* owner);
	Component(const std::shared_ptr<Object>& owner);
	Component(Component&& other) = default;

	Component(const Component& other);
	virtual ~Component() {}

	void set_owner(Object* owner);
	std::shared_ptr<Object> owner() const;

	/*
	Component* GetCopy() override 
	{ 
		return new MeshComponent(*this); 
	}
	*/
	virtual Component* GetCopy() = 0;
	virtual void Update(float elapsed_time) {}

	Object* hierarchy_root();

protected:
	std::weak_ptr<Object> owner_;
	std::weak_ptr<Object> hierarchy_root_; 
};

