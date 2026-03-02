import sys

def modify_server_gun():
    with open('SandyHeroesServer/GunComponent.cpp', 'r', encoding='ansi') as f:
        content = f.read()

    # 1. Replace Update loop
    old_update = """    for (const auto& bullet : fired_bullet_list_)
    {
		auto bullet_position = bullet->world_position_vector();
        if(xmath_util_float3::LengthSq(bullet_position - owner_->world_position_vector()) > 10000.f)
        {
            bullet->set_is_dead(true);
		}
    }"""
    
    new_update = """    for (auto it = fired_bullet_list_.begin(); it != fired_bullet_list_.end(); )
    {
        if (auto bullet = it->lock())
        {
            auto locked_owner = owner_.lock();
            if (locked_owner)
            {
                auto bullet_position = bullet->world_position_vector();
                if(xmath_util_float3::LengthSq(bullet_position - locked_owner->world_position_vector()) > 10000.f)
                {
                    GameFramework::Instance()->GetScene()->DeleteObject(bullet);
                    it = fired_bullet_list_.erase(it);
                    continue;
                }
            }
            ++it;
        }
        else
        {
            it = fired_bullet_list_.erase(it);
        }
    }"""
    content = content.replace(old_update, new_update)

    # 2. Replace FireBullet Signature
    content = content.replace('bool GunComponent::FireBullet(XMFLOAT3 direction, Object* bullet_model, Scene* scene, int id)', 
                              'bool GunComponent::FireBullet(XMFLOAT3 direction, std::shared_ptr<Object> bullet_model, Scene* scene, int id)')

    # 3. Replace owner_ inside FireBullet with locked_owner
    content = content.replace('owner_->world_position_vector()', 'locked_owner->world_position_vector()')

    # 4. Replace bullet initialization inside FireBullet
    content = content.replace('Object* bullet = bullet_model->GetCopy();', 
                              'auto locked_owner = owner_.lock();\\n            if (!locked_owner) return false;\\n\\n            std::shared_ptr<Object> bullet = bullet_model;')
    
    # 4.1 Actually the replace string literal was broken in previous code so I use raw string or \n properly
    content = content.replace('auto locked_owner = owner_.lock();\\n            if (!locked_owner) return false;\\n\\n            std::shared_ptr<Object> bullet = bullet_model;', 
                              'auto locked_owner = owner_.lock();\n            if (!locked_owner) return false;\n\n            std::shared_ptr<Object> bullet = bullet_model;')

    # 5. Replace MovementComponent initialization inside FireBullet
    content = content.replace('MovementComponent* movement = new MovementComponent(bullet);',
                              'auto movement = std::make_shared<MovementComponent>(bullet.get());')

    # 6. Replace OnDestroy closure inside FireBullet
    old_ondestroy = """            std::function<void(Object*)> on_destroy_func = [this](Object* bullet) {
                fired_bullet_list_.remove(bullet);
            };"""
    new_ondestroy = """            std::function<void(Object*)> on_destroy_func = [this](Object* bullet_ptr) {
                fired_bullet_list_.remove_if([bullet_ptr](const std::weak_ptr<Object>& wp) {
                    return wp.lock().get() == bullet_ptr;
                });
            };"""
    content = content.replace(old_ondestroy, new_ondestroy)

    with open('SandyHeroesServer/GunComponent.cpp', 'w', encoding='ansi') as f:
        f.write(content)

modify_server_gun()