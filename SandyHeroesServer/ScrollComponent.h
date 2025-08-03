#pragma once
#include "Component.h"

enum class ScrollType
{
	kSprinter,
	kNinja,
	kWeaponMaster,
	kFlameMaster,
	kAcidMaster,
	kElectricMaster,
	kFlameFrenzy,
	kAcidFrenzy,
	kElectricFrenzy,
	kHardenedSkin,
	None
};

class ScrollComponent :
    public Component
{
public:
    ScrollComponent() = default;
    ScrollComponent(Object* owner) : Component(owner) {}
    virtual ~ScrollComponent() = default;

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;


	ScrollType type() const 
	{ 
		return type_; 
	}

	void set_direction(const XMFLOAT3& direction) 
	{ 
		direction_ = direction; 
	}
	void set_moved_distance(float distance) 
	{ 
		moved_distance_ = distance; 
	}
	void set_type(ScrollType type) 
	{ 
		type_ = type; 
	}
	void set_is_active(bool is_active)
	{
		is_active_ = is_active;
	}

private:
	XMFLOAT3 direction_{};  // 정규화된 이동 방향
	float moved_distance_ = 0.f;
	bool is_active_ = false;
	ScrollType type_ = ScrollType::None;

};


