import sys
import re

def refactor_chest_client():
    h_file = 'SandyHeroesClient/ChestComponent.h'
    with open(h_file, 'r', encoding='ansi') as f:
        data = f.read()
    data = data.replace('virtual void HendleCollision(Object* other_object) override;', 'virtual void HendleCollision(std::shared_ptr<Object> other_object) override;')
    data = data.replace('Object* scroll_object_{ nullptr };', 'std::weak_ptr<Object> scroll_object_{};')
    with open(h_file, 'w', encoding='ansi') as f:
        f.write(data)

    c_file = 'SandyHeroesClient/ChestComponent.cpp'
    with open(c_file, 'r', encoding='ansi') as f:
        data = f.read()

    data = data.replace('void ChestComponent::HendleCollision(Object* other_object)', 'void ChestComponent::HendleCollision(std::shared_ptr<Object> other_object)')

    # Handle `owner_` references by injecting `locked_owner`
    # Replace the body of HendleCollision
    old_body_hc = """	if (is_open_)
	{
		return;
	}

	is_open_ = true;
	
    if (!animator_)
    {
		animator_ = Object::GetComponent<AnimatorComponent>(owner_);
        if (!animator_)
        {
            OutputDebugString(L"ChestComponent: animator_ is nullptr!");
			return;
        }
    }   

	animator_->animation_state()->ChangeAnimationTrack(
		(int)ChestAnimationTrack::kCloseToOpen, owner_, animator_);

	FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	// ƼŬ  ѱ
	auto chest_particle = Object::GetComponent<ParticleComponent>(owner_);
	if (chest_particle)
	{
		chest_particle->set_loop(true);
	}

	// ũ 1 
	Object* scroll = scroll_model_->GetInstance();
	//ũ ȸ  ġ ʱȭ( -> )
	scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f)); //  ̵

	scroll_object_ = scroll;
	scene_->AddObject(scroll);"""
    
    new_body_hc = """	if (is_open_)
	{
		return;
	}

	is_open_ = true;
	
	auto locked_owner = owner_.lock();
	if (!locked_owner) return;

    if (!animator_)
    {
		animator_ = Object::GetComponent<AnimatorComponent>(locked_owner);
        if (!animator_)
        {
            OutputDebugString(L"ChestComponent: animator_ is nullptr!");
			return;
        }
    }   

	animator_->animation_state()->ChangeAnimationTrack(
		(int)ChestAnimationTrack::kCloseToOpen, locked_owner.get(), animator_);

	FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);
	if (chest_particle)
	{
		chest_particle->set_loop(true);
	}

	std::shared_ptr<Object> scroll = scroll_model_->GetInstance();
	scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll.get());
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f));

	scroll_object_ = scroll;
	scene_->AddObject(scroll);"""
    # Regex replacing because comments might be different
    data = re.sub(r'if \(is_open_\)[\s\S]*?scene_->AddObject\(scroll\);', new_body_hc, data, count=1)

    # Replace the body of OpenChest
    new_body_oc = """	if (is_open_)
	{
		return;
	}

	is_open_ = true;

	auto locked_owner = owner_.lock();
	if (!locked_owner) return;

	if (!animator_)
	{
		animator_ = Object::GetComponent<AnimatorComponent>(locked_owner);
		if (!animator_)
		{
			OutputDebugString(L"ChestComponent: animator_ is nullptr!");
			return;
		}
	}

	animator_->animation_state()->ChangeAnimationTrack(
		(int)ChestAnimationTrack::kCloseToOpen, locked_owner.get(), animator_);

	FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);
	if (chest_particle)
	{
		chest_particle->set_loop(true);
	}

	set_scroll_model(scroll_model);

	std::shared_ptr<Object> scroll = scroll_model_->GetInstance();
	scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll.get());
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f));
	scroll_comp->set_type(static_cast<ScrollType>(scroll_type));

	std::cout << "scroll_type: " << static_cast<int>(scroll_comp->type()) << '
';

	scroll_object_ = scroll;
	scene_->AddObject(scroll);"""
    # Just replace from OpenChest start
    data = re.sub(r'if \(is_open_\)[\s\S]*?scene_->AddObject\(scroll\);', new_body_oc, data, count=1)

    # In TakeScroll
    data = data.replace('if (!scroll_object_)', 'auto scroll_obj = scroll_object_.lock();
	if (!scroll_obj)')
    data = data.replace('Object::GetComponent<ScrollComponent>(scroll_object_)', 'Object::GetComponent<ScrollComponent>(scroll_obj.get())')
    data = data.replace('scroll_object_->set_is_dead(true);', 'GameFramework::Instance()->scene()->DeleteObject(scroll_obj);')
    data = data.replace('scroll_object_ = nullptr;', 'scroll_object_.reset();')

    # Also replace owner_ in TakeScroll
    data = data.replace('auto chest_particle = Object::GetComponent<ParticleComponent>(owner_);', 
                        'auto locked_owner = owner_.lock();
	if(!locked_owner) return type;
	auto chest_particle = Object::GetComponent<ParticleComponent>(locked_owner);')

    with open(c_file, 'w', encoding='ansi') as f:
        f.write(data)

refactor_chest_client()
