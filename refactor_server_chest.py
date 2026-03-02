import sys

def fix_chest():
    h_file = 'SandyHeroesServer/ChestComponent.h'
    with open(h_file, 'r', encoding='ansi') as f:
        data = f.read()
    data = data.replace('void HendleCollision(Object* other_object, int chest_num);', 'void HendleCollision(std::shared_ptr<Object> other_object, int chest_num);')
    data = data.replace('Object* scroll_object_{ nullptr };', 'std::weak_ptr<Object> scroll_object_{};')
    with open(h_file, 'w', encoding='ansi') as f:
        f.write(data)

    cpp_file = 'SandyHeroesServer/ChestComponent.cpp'
    with open(cpp_file, 'r', encoding='ansi') as f:
        data = f.read()

    data = data.replace('void ChestComponent::HendleCollision(Object* other_object, int chest_num)', 'void ChestComponent::HendleCollision(std::shared_ptr<Object> other_object, int chest_num)')
    
    data = data.replace('Object* scroll = scroll_model_->GetInstance();', 'std::shared_ptr<Object> scroll = scroll_model_->GetInstance();')
    
    old_transform = 'scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());'
    new_transform = 'auto locked_owner = owner_.lock();
	if(locked_owner) scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());'
    data = data.replace(old_transform, new_transform)

    data = data.replace('Object::GetComponent<ScrollComponent>(scroll)', 'Object::GetComponent<ScrollComponent>(scroll.get())')
    data = data.replace('scroll_model_->hierarchy_root()', 'scroll_model_->hierarchy_root().get()')

    data = data.replace('if (!scroll_object_)', 'auto scroll_obj = scroll_object_.lock();
	if (!scroll_obj)')
    data = data.replace('Object::GetComponent<ScrollComponent>(scroll_object_)', 'Object::GetComponent<ScrollComponent>(scroll_obj.get())')
    
    data = data.replace('scroll_object_->set_is_dead(true);', 'scene_->DeleteObject(scroll_obj);')
    data = data.replace('scroll_object_ = nullptr;', 'scroll_object_.reset();')

    with open(cpp_file, 'w', encoding='ansi') as f:
        f.write(data)

fix_chest()
