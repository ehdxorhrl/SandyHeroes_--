#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"
#include "MovementComponent.h"
#include "SessionManager.h"
#include "User.h"
//#include "ShotDragonAnimationState.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), attack_force_(other.attack_force_), 
target_(other.target_), scene_(other.scene_)
{
}

Component* MonsterComponent::GetCopy()
{
    return new MonsterComponent(*this);
}

void MonsterComponent::Update(float elapsed_time)
{
    if (is_dead_animationing_)
    {
        return;
    }

    if (hp_ <= 0)
    {
        auto animator = Object::GetComponentInChildren<AnimatorComponent>(owner_);
        if (!animator)
        {
            std::string temp = owner_->name() + "의 MonsterComponent 죽음 애니메이션 출력 과정에서 문제가 생겼습니다.";
            std::wstring debug_str;
            debug_str.assign(temp.begin(), temp.end());

            OutputDebugString(debug_str.c_str());
            return;
        }
        auto animation_state = animator->animation_state();
        if (animation_state)
        {
            if (animation_state->GetDeadAnimationTrack() == -1)
            {
                // 죽는 애니메이션이 없으면 그냥 죽는다.
                owner_->set_is_dead(true);
                return;
            }
            // 죽는 애니메이션으로 전환
            animation_state->ChangeAnimationTrack(animation_state->GetDeadAnimationTrack(), owner_, animator);
            animation_state->set_animation_loop_type(1); // Once
            is_dead_animationing_ = true;
            return;
        }
    }

    //TODO: 몬스터의 행동을 결정하는 AI 추가
    //EX) ai->Update(owner_, elapsed_time);
    if (target_)
    {
        auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
        if (movement)
        {
            XMFLOAT3 look = owner_->look_vector();
            look.y = 0.f;
            look = xmath_util_float3::Normalize(look);
            XMFLOAT3 direction = target_->world_position_vector() - owner_->world_position_vector();
            direction.y = 0.f;
            direction = xmath_util_float3::Normalize(direction);
            float angle = xmath_util_float3::AngleBetween(look, direction);
            if (angle > XM_PI / 180.f * 5.f)
            {
                //회전 방향 연산
                XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
                if (cross.y < 0)
                {
                    angle = -angle;
                }
                angle = XMConvertToDegrees(angle);
                owner_->Rotate(0.f, angle, 0.f);
            }

            if (owner_->tag() == "Shot_Dragon")
            {
                auto animator = Object::GetComponent<AnimatorComponent>(owner_);
                auto animation_state = animator->animation_state();
                //animation_state->ChangeAnimationTrack((int)ShotDragonAnimationTrack::kAttack, owner_, animator);
                //animation_state->set_animation_loop_type(0); // Loop


                sc_packet_monster_move mm;
                mm.size = sizeof(sc_packet_monster_move);
                mm.type = S2C_P_MONSTER_MOVE;
                mm.id = owner_->id();
                XMFLOAT4X4 xf;
                const XMFLOAT4X4& mat = owner_->transform_matrix();
                XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
                memcpy(mm.matrix, &xf, sizeof(float) * 16);

                const auto& users = SessionManager::getInstance().getAllSessions();
                for (auto& u : users) {
                    u.second->do_send(&mm);
                }
                return;
            }
            if (owner_->tag() == "Strong_Dragon")
            {
                return;
            }

            movement->MoveXZ(direction.x, direction.z, 5.f);

            movement->set_max_speed_xz(3.5f);

            auto velocity_xz = movement->velocity();
            velocity_xz.y = 0.f;
            float speed = xmath_util_float3::Length(velocity_xz);

            sc_packet_monster_move mm;
            mm.size = sizeof(sc_packet_monster_move);
            mm.type = S2C_P_MONSTER_MOVE;
            mm.id = owner_->id();
            mm.speed = speed;
            XMFLOAT4X4 xf;
            const XMFLOAT4X4& mat = owner_->transform_matrix();
            XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
            memcpy(mm.matrix, &xf, sizeof(float) * 16);

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mm);
            }
        }
        for (auto& [type, effect] : status_effects_)
        {
            if (!effect.IsActive()) continue;

            effect.elapsed += elapsed_time;

            if (type == StatusEffectType::Fire)
            {
                float dps = effect.fire_damage * 0.9f;
                HitDamage(dps * elapsed_time);
            }
            else if (type == StatusEffectType::Electric)
            {
                auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
                if (movement)
                {
                    if (!electric_slow_applied_)
                    {
                        original_speed_ = movement->max_speed_xz();
                        movement->set_max_speed_xz(original_speed_ * 0.70f);
                        electric_slow_applied_ = true;
                    }
                }
            }
        }
        auto& electric = status_effects_[StatusEffectType::Electric];
        if (!electric.IsActive() && electric_slow_applied_)
        {
            auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);
            if (movement)
            {
                movement->set_max_speed_xz(original_speed_);
                electric_slow_applied_ = false;
            }
        }
    }

    if (!target_)
    {
        float min_distance_sq = FLT_MAX;
        Object* nearest_player = nullptr;

        const auto& sessions = SessionManager::getInstance().getAllSessions();
        for (const auto& pair : sessions)
        {
            const auto& session = pair.second;
            Object* player = session->get_player_object();
            if (!player || player->is_dead())
                continue;

            float dist_sq = xmath_util_float3::LengthSq(player->world_position_vector() - owner_->world_position_vector());
            if (dist_sq < min_distance_sq)
            {
                min_distance_sq = dist_sq;
                nearest_player = player;
            }
        }

        if (nearest_player)
        {
            set_target(nearest_player);
        }
    }

}

void MonsterComponent::InitAfterOwnerSet() {
    if (!owner_) return;

    sc_packet_monster_info mi;
    mi.size = sizeof(sc_packet_monster_info);
    mi.type = S2C_P_MONSTER_INFO;
   
    mi.attack_force = attack_force_;
    XMFLOAT4X4 xf;
    const XMFLOAT4X4& mat = owner_->transform_matrix();
    XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
    memcpy(mi.matrix, &xf, sizeof(float) * 16);
    
    mi.id = owner_->id();
    mi.max_hp = hp_;
    mi.max_shield = shield_;
    mi.monster_type = static_cast<int32_t>(owner_->monster_type());

    //std::cout << mi.type << std::endl;
    //std::cout << mi.attack_force << std::endl;
    //std::cout << mi.max_hp << std::endl;
    //std::cout << mi.max_shield << std::endl;
    std::cout << mi.monster_type << std::endl;
    //std::cout << "id: " << mi.id << std::endl;
    //std::cout << owner_->position_vector().x << std::endl;
    //std::cout << owner_->position_vector().y << std::endl;
    //std::cout << owner_->position_vector().z << std::endl;
    
    const auto& users = SessionManager::getInstance().getAllSessions();
    for (auto& u : users) {
        u.second->do_send(&mi);
    }
}

void MonsterComponent::HitDamage(float damage)
{
    if (shield_ > 0)
    {
        //std::cout << "전: " << shield_ << std::endl;
        shield_ -= damage;
        if (shield_ < 0)
        {
            hp_ += shield_; // shield가 음수면 hp에 더해줌
            shield_ = 0;
        }
        //std::cout << "후: " << shield_ << std::endl;
    }
    else
    {
        hp_ -= damage;
    }
    if (hp_ <= 0)
    {
        hp_ = 0;
        if (scene_)
        {
            BaseScene* base_scene = dynamic_cast<BaseScene*>(scene_);
            if (base_scene)
            {
                base_scene->add_catch_monster_num();
            }
        }
    }
    

    sc_packet_monster_damaged md;
    md.size = sizeof(sc_packet_monster_damaged);
    md.type = S2C_P_MONSTER_DAMAGED;
    md.id = owner_->id();
    md.hp = hp_;
    md.shield = shield_;

    const auto& users = SessionManager::getInstance().getAllSessions();
    for (auto& u : users) {
        u.second->do_send(&md);
    }
}

void MonsterComponent::ApplyStatusEffect(StatusEffectType type, float duration, float damage, 
    bool flame_frenzy, bool acid_frenzy, bool electric_frenzy)
{
    status_effects_[type] = { duration, 0.f, damage , flame_frenzy, acid_frenzy, electric_frenzy };

}

void MonsterComponent::set_shield(float value)
{
    shield_ = value;
}

void MonsterComponent::set_hp(float value)
{
    hp_ = value;
}

void MonsterComponent::set_attack_force(float value)
{
    attack_force_ = value;
}

void MonsterComponent::set_target(Object* target)
{
    target_ = target;
}

void MonsterComponent::set_is_pushed(bool is_pushed)
{
    is_pushed_ = is_pushed;
}

void MonsterComponent::set_push_timer(float value)
{
    push_timer_ = value;
}

float MonsterComponent::shield() const
{
    return shield_;
}

float MonsterComponent::hp() const
{
    return hp_;
}

float MonsterComponent::max_hp() const
{
    return max_hp_;
}

float MonsterComponent::max_shield() const
{
    return max_shield_;
}

float MonsterComponent::attack_force() const
{
    return attack_force_;
}

bool MonsterComponent::IsDead() const
{
    return hp_ <= 0;
}

void MonsterComponent::set_scene(Scene* value)
{
    scene_ = value;
}
