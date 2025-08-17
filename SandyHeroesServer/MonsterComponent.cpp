#include "stdafx.h"
#include "MonsterComponent.h"
#include "Object.h"
#include "AnimatorComponent.h"
#include "AnimationState.h"
#include "MovementComponent.h"
#include "SessionManager.h"
#include "User.h"
#include "AIComponent.h"
#include "AStar.h"
//#include "ShotDragonAnimationState.h"

MonsterComponent::MonsterComponent(Object* owner) : Component(owner)
{
    
}

MonsterComponent::MonsterComponent(const MonsterComponent& other) : Component(other),
shield_(other.shield_), hp_(other.hp_), attack_force_(other.attack_force_), 
target_(other.target_), scene_(other.scene_)
{
}

MonsterComponent::~MonsterComponent()
{

    if(ai_) delete ai_; // AIComponent 메모리 해제
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

    if (is_pushed_) {
        push_timer_ -= elapsed_time;
        if (push_timer_ > 0.f) {
            if (auto mv = Object::GetComponentInChildren<MovementComponent>(owner_)) {
                mv->Stop(); // 그 사이엔 속도 0으로
            }
            return; // 이 프레임은 이동/AI 중단
        }
        is_pushed_ = false;
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

    if (!ai_) {
        ai_ = new AIComponent(owner_);
        owner_->AddComponent(ai_);
        RebuildBehaviorTree_();
    }

}

void MonsterComponent::UpdateTargetPath()
{
    auto base_scene = dynamic_cast<BaseScene*>(scene_);
    const auto& current_stage_node_buffer = kStageNodeBuffers[base_scene->stage_clear_num()];

    Node* start_node = nullptr;
    Node* goal_node = nullptr;

    float start_min_distance_sq = FLT_MAX;
    float goal_min_distance_sq = FLT_MAX;

    for (const auto& node : current_stage_node_buffer)
    {
        float start_distance_sq = xmath_util_float3::LengthSq(node.position - owner_->world_position_vector());
        if (start_distance_sq < start_min_distance_sq)
        {
            start_min_distance_sq = start_distance_sq;
            start_node = const_cast<Node*>(&node);
        }

        float target_distance_sq = xmath_util_float3::LengthSq(node.position - target_->world_position_vector());
        if (target_distance_sq < goal_min_distance_sq)
        {
            goal_min_distance_sq = target_distance_sq;
            goal_node = const_cast<Node*>(&node);
        }
    }

    path_ = a_star::AStar(start_node, goal_node);
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
    mi.animation_track = 0;

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
        owner_->set_is_dead(true);
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

void MonsterComponent::set_max_shield(float value)
{
    max_shield_ = value;
    if (shield_ > max_shield_) {
        shield_ = max_shield_; // shield가 max_shield를 초과하지 않도록 조정
	}
}

void MonsterComponent::set_shield(float value)
{
    shield_ = value;
}

void MonsterComponent::set_max_hp(float value)
{
    max_hp_ = value;
    if (hp_ > max_hp_) {
        hp_ = max_hp_; // hp가 max_hp를 초과하지 않도록 조정
	}
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

Object* MonsterComponent::target() const
{
    return target_;
}

Scene* MonsterComponent::scene() const
{
    return scene_;
}

bool MonsterComponent::IsDead() const
{
    return hp_ <= 0;
}

void MonsterComponent::set_scene(Scene* value)
{
    scene_ = value;
}

void MonsterComponent::RebuildBehaviorTree_()
{
    if (!ai_) {
        ai_ = new AIComponent(owner_); // 또는 make_unique 사용
    }

    BTNode* root = nullptr;
 
    switch (owner_->monster_type()) {
    case MonsterType::Strong_Dragon:
        root = Build_Strong_Dragon_Tree(owner_);
        //std::cout << "Build_Strong_Dragon_Tree 생성 완료" << std::endl;
        break;
    case MonsterType::Hit_Dragon:  
        root = Build_Hit_Dragon_Tree(owner_);  
        std::cout << "Build_Hit_Dragon_Tree 생성 완료" << std::endl;
        break;
    case MonsterType::Bomb_Dragon:  
        root = Build_Bomb_Dragon_Tree(owner_);
        std::cout << "Build_Bomb_Dragon_Tree 생성 완료" << std::endl;
        break;
    case MonsterType::Shot_Dragon:  
        root = Build_Shot_Dragon_Tree(owner_);
        std::cout << "Build_Shot_Dragon_Tree 생성 완료" << std::endl;
        break;
    case MonsterType::Super_Dragon:
        root = Build_Super_Dragon_Tree(owner_);
        break;

    default:
        break;
    }

    ai_->SetBehaviorTree(root);
}