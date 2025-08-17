#include "stdafx.h"
#include "AIComponent.h"
#include "Object.h"
#include "MonsterComponent.h"
#include "MovementComponent.h"
#include "SessionManager.h"
#include "BaseScene.h"
#include "AStar.h"

std::unordered_map<int, int> AIComponent::s_node_owner_;
std::unordered_map<int, int> AIComponent::s_desire_next_;

AIComponent::AIComponent(Object* owner) : Component(owner)
{
}

AIComponent::AIComponent(const AIComponent& other) : Component(other.owner_)
{

}

AIComponent::~AIComponent()
{
    delete behavior_tree_root_; // 메모리 누수 방지

    if (last_owned_node_id_ != -1) AIComponent::s_node_owner_[last_owned_node_id_] = 0;
    AIComponent::s_desire_next_.erase(owner_->id());
}

Component* AIComponent::GetCopy()
{
    return new AIComponent(*this);
}

void AIComponent::Update(float elapsed_time)
{
    if (behavior_tree_root_) {
        behavior_tree_root_->Run(elapsed_time);
    }
}

void AIComponent::SetBehaviorTree(BTNode* root)
{
    if (behavior_tree_root_) delete behavior_tree_root_;
    behavior_tree_root_ = root;
}



bool AIComponent::Move_To_Target(float elapsed_time) 
{
    auto* monstercomponent = Object::GetComponentInChildren<MonsterComponent>(owner_);
    if (!monstercomponent) return false;

    auto movement = Object::GetComponentInChildren<MovementComponent>(owner_);

    if (movement)
    {
        XMFLOAT3 look = owner_->look_vector();
        look.y = 0.f;
        look = xmath_util_float3::Normalize(look);

        constexpr float kAstarCoolTime = 0.2f;
        astar_delta_cool_time_ += elapsed_time;
        if (astar_delta_cool_time_ > kAstarCoolTime)
        {
            astar_delta_cool_time_ = 0.f;

            auto base_scene = dynamic_cast<BaseScene*>(monstercomponent->scene());
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
                float target_distance_sq = xmath_util_float3::LengthSq(node.position - monstercomponent->target()->world_position_vector());
                if (target_distance_sq < goal_min_distance_sq)
                {
                    goal_min_distance_sq = target_distance_sq;
                    goal_node = const_cast<Node*>(&node);
                }
            }
            path_ = a_star::AStar(start_node, goal_node);
            monstercomponent->UpdateTargetPath();

            for (const auto& node : path_)
            {
                node->position.y = 0.f;
            }

            current_node_idx_ = 0;
            if (current_node_ == path_[0])
            {
                if (current_node_ == path_[0])
                {
                    ++current_node_idx_;
                }
            }
        }

        XMFLOAT3 direction;
        if (path_.size() <= current_node_idx_ || path_.size() <= 2 )
        {
            direction = monstercomponent->target()->world_position_vector() - owner_->world_position_vector();
        }
        else
        {
            auto position_xz = owner_->world_position_vector();
            position_xz.y = 0.f;

            direction = path_[current_node_idx_]->position - position_xz;
            float distance = xmath_util_float3::Length(direction);

            if (distance < 0.2f)
            {
                current_node_ = path_[current_node_idx_];
                ++current_node_idx_;
            }
        }

        direction.y = 0.f;
        direction = xmath_util_float3::Normalize(direction);
        float angle = xmath_util_float3::AngleBetween(look, direction);
        if (angle > XM_PI / 180.f * 5.f)
        {
            XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
            if (cross.y < 0)
            {
                angle = -angle;
            }
            angle = XMConvertToDegrees(angle);
            owner_->Rotate(0.f, angle, 0.f);
        }

        movement->MoveXZ(direction.x, direction.z, 5.f);

        auto velocity_xz = movement->velocity();
        velocity_xz.y = 0.f;
        float speed = xmath_util_float3::Length(velocity_xz);

        if (speed > 0) {
            Send_Move_Packet(elapsed_time, speed);
        }
    }
    return true;
}

bool AIComponent::Rotate_To_Target(float elapsed_time, Object* target) {

    auto* monster = Object::GetComponentInChildren<MonsterComponent>(owner_);
    if (!monster) return false;

    auto* movement = Object::GetComponentInChildren<MovementComponent>(owner_);
    if (!movement) return false;

    XMFLOAT3 look = owner_->look_vector();
    look.y = 0.f;
    look = xmath_util_float3::Normalize(look);
    XMFLOAT3 direction = target->world_position_vector() - owner_->world_position_vector();
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

    Send_Move_Packet(elapsed_time, 0);

    return true;
}

void AIComponent::Send_Move_Packet(float elapsed_time, float speed)
{

    sc_packet_monster_move mm{};
    mm.size = sizeof(sc_packet_monster_move);
    mm.type = S2C_P_MONSTER_MOVE;
    mm.id = owner_->id();
    mm.speed = speed;
    
    XMFLOAT4X4 xf;
    const XMFLOAT4X4& mat = owner_->transform_matrix();
    XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
    memcpy(mm.matrix, &xf, sizeof(float) * 16);
    
    switch (owner_->monster_type()) {
    case MonsterType::Strong_Dragon: 
        mm.animation_track = 3; break;
    case MonsterType::Hit_Dragon:   
        mm.animation_track = 6; break;
    case MonsterType::Bomb_Dragon:   
        mm.animation_track = 5; break;
    default:                         
        mm.animation_track = 3; break;
    }
    
    const auto& users = SessionManager::getInstance().getAllSessions();
    for (auto& u : users) {
        u.second->do_send(&mm);
    }

}

