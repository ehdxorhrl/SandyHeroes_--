import sys

def modify_client_gun():
    with open('SandyHeroesClient/GunComponent.cpp', 'r', encoding='ansi') as f:
        content = f.read()

    # Add GameFramework.h include
    content = content.replace('#include "SoundComponent.h"\\n', '#include "SoundComponent.h"\\n#include "GameFramework.h"\\n')
    content = content.replace('#include "SoundComponent.h"\\n'.replace('\\\\n', '\\n'), '#include "SoundComponent.h"\\n#include "GameFramework.h"\\n'.replace('\\\\n', '\\n'))
    content = content.replace('#include "SoundComponent.h"\n', '#include "SoundComponent.h"\n#include "GameFramework.h"\n')

    # 1. Replace Update loop
    old_update = """    for (const auto& bullet : fired_bullet_list_)
    {
        auto bullet_position = bullet->world_position_vector();
        if (xmath_util_float3::Length(bullet_position - owner_->world_position_vector()) > 100.f)
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
                if (xmath_util_float3::Length(bullet_position - locked_owner->world_position_vector()) > 100.f)
                {
                    GameFramework::Instance()->scene()->DeleteObject(bullet);
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
    content = content.replace('bool GunComponent::FireBullet(XMFLOAT3 direction, Object* bullet_model, Scene* scene)', 
                              'bool GunComponent::FireBullet(XMFLOAT3 direction, std::shared_ptr<Object> bullet_model, Scene* scene)')

    # 3. Replace bullet initialization and owner lock inside FireBullet
    old_init = """        Object* bullet = bullet_model;
        bullet->set_is_movable(true);
        XMFLOAT3 bullet_look = xmath_util_float3::Normalize(bullet->look_vector());"""
    new_init = """        auto locked_owner = owner_.lock();
        if (!locked_owner) return false;

        std::shared_ptr<Object> bullet = bullet_model;
        bullet->set_is_movable(true);
        XMFLOAT3 bullet_look = xmath_util_float3::Normalize(bullet->look_vector());"""
    content = content.replace(old_init, new_init)
    
    # 3.1 Replace root initialization inside FireBullet because owner_ is used early
    content = content.replace('auto root = owner_->GetHierarchyRoot();',
                              'auto locked_root_owner = owner_.lock();\n        if(!locked_root_owner) return false;\n        auto root = locked_root_owner->GetHierarchyRoot();')

    # 4. Replace owner_ inside FireBullet with locked_owner
    content = content.replace('owner_->world_position_vector()', 'locked_owner->world_position_vector()')

    # 5. Replace MovementComponent initialization inside FireBullet
    content = content.replace('MovementComponent* movement = new MovementComponent(bullet);',
                              'auto movement = std::make_shared<MovementComponent>(bullet.get());')

    # 6. Replace OnDestroy closure inside FireBullet
    old_ondestroy = """        std::function<void(Object*)> on_destroy_func = [this](Object* bullet) {
            fired_bullet_list_.remove(bullet);
        };"""
    new_ondestroy = """        std::function<void(Object*)> on_destroy_func = [this](Object* bullet_ptr) {
            fired_bullet_list_.remove_if([bullet_ptr](const std::weak_ptr<Object>& wp) {
                return wp.lock().get() == bullet_ptr;
            });
        };"""
    content = content.replace(old_ondestroy, new_ondestroy)

    with open('SandyHeroesClient/GunComponent.cpp', 'w', encoding='ansi') as f:
        f.write(content)

modify_client_gun()