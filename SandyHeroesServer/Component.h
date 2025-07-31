#pragma once

class Object;

// 오브젝트에 기능을 추가해주는 클래스의 기초 클래스
// 
class Component
{
public:
	Component() = default;
	Component(Object* owner);
	Component(Component&& other) = default; //이동 생성자

	//복사 생성된 컴포넌트는 owner를 재지정해야함(의도 하지않은 오브젝트에 연결방지)
	Component(const Component& other);
	virtual ~Component() {}

	void set_owner(Object* owner);
	Object* owner() const;

	/*
	컴포넌트의 복사본을 리턴(이 클래스를 상속받는 클래스에서 반드시 작성해야한다.)
	오브젝트 복사시 기존 오브젝트가 갖는 컴포넌트도 복사하기 위해 필요
	작성 예시
	Component* GetCopy() override
	{
		return new MeshComponent(*this);
	}
	*/
	virtual Component* GetCopy() = 0;
	virtual void Update(float elapsed_time) {};

	Object* hierarchy_root();

protected:
	Object* owner_ = nullptr;
	Object* hierarchy_root_ = nullptr; // 오브젝트의 계층 구조에서 루트 오브젝트를 가리킴
};

