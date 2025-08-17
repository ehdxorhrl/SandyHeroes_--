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
    delete behavior_tree_root_; // �޸� ���� ����

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


bool AIComponent::Move_To_Target(float elapsed_time) {
    auto* monster = Object::GetComponentInChildren<MonsterComponent>(owner_);
    if (!monster) return false;

    auto* movement = Object::GetComponentInChildren<MovementComponent>(owner_);
    if (!movement) return false;

    // (A) �纸 ���̸� ����
    if (yield_timer_ > 0.f) {
        yield_timer_ -= elapsed_time;
        movement->Stop();
        return true;
    }

    XMFLOAT3 look = owner_->look_vector(); look.y = 0.f;
    look = xmath_util_float3::Normalize(look);

    // ���� A* ���� ��� ����(��� ���� �Ϸ� ��)�� 'ù ����' ���
    constexpr float kAstarCoolTime = 0.2f;
    astar_delta_cool_time_ += elapsed_time;
    if (astar_delta_cool_time_ > kAstarCoolTime) {
        astar_delta_cool_time_ = 0.f;

        auto base_scene = dynamic_cast<BaseScene*>(monster->scene());
        const auto& nodes = kStageNodeBuffers[base_scene->stage_clear_num()];
        Node* start_node = nullptr; Node* goal_node = nullptr;
        float bestStart = FLT_MAX, bestGoal = FLT_MAX;

        for (const auto& n : nodes) {
            float dS = xmath_util_float3::LengthSq(n.position - owner_->world_position_vector());
            if (dS < bestStart) { bestStart = dS; start_node = const_cast<Node*>(&n); }
            float dG = xmath_util_float3::LengthSq(n.position - monster->target()->world_position_vector());
            if (dG < bestGoal) { bestGoal = dG; goal_node = const_cast<Node*>(&n); }
        }

        path_ = a_star::AStar(start_node, goal_node);
        monster->UpdateTargetPath();
        for (auto* n : path_) n->position.y = 0.f;

        // ---- [NEW] ���� current_node_�� ������ '���� ����� ���' ���� ����
        if (!current_node_ && start_node) {
            current_node_ = start_node;
            AIComponent::s_node_owner_[current_node_->id] = owner_->id();
            last_owned_node_id_ = current_node_->id;
        }

        current_node_idx_ = 0;
        if (!path_.empty() && current_node_ == path_[0]) ++current_node_idx_;
    }

    // ���� ���� ��� ����
    XMFLOAT3 direction{};
    int my_current_id = current_node_ ? current_node_->id : -1;

    if (path_.size() <= static_cast<size_t>(current_node_idx_)) {
        direction = monster->target()->world_position_vector() - owner_->world_position_vector();
    }
    else {
        // ----- ���� ���
        Node* next = path_[current_node_idx_];
        int next_id = next ? next->id : -1;

        // (B) "���� ������ next_id�� ���� �ʹ�" ���
        if (next_id != -1) AIComponent::s_desire_next_[owner_->id()] = next_id;

        // (C) ���� Ž��: next�� �ٸ� ���� ���� ����̰�,
        //     �� �ٸ� ���� '����'�� �� ���� ����� -> ���� ����
        if (next_id != -1) {
            auto itOcc = AIComponent::s_node_owner_.find(next_id);
            if (itOcc != AIComponent::s_node_owner_.end() && itOcc->second != 0 && itOcc->second != owner_->id()) {
                int other = itOcc->second;
                int other_desire = -1;
                auto itDes = AIComponent::s_desire_next_.find(other);
                if (itDes != AIComponent::s_desire_next_.end()) other_desire = itDes->second;

                if (other_desire == my_current_id && my_current_id != -1) {
                    // �켱����: id�� ū ���� �纸(���ϸ� �ݴ�� �ٲ㵵 ��)
                    if (owner_->id() > other) {
                        // �� ĭ �ڷ�(backoff): ���� ��尡 ������ �ڷ� ������, ������ ��� ����
                        if (current_node_idx_ > 0) {
                            --current_node_idx_;
                            current_node_ = path_[current_node_idx_];
                            // ���� ����
                            if (last_owned_node_id_ != -1) AIComponent::s_node_owner_[last_owned_node_id_] = 0;
                            AIComponent::s_node_owner_[current_node_->id] = owner_->id();
                            last_owned_node_id_ = current_node_->id;
                        }
                        movement->Stop();
                        yield_timer_ = 0.5f; // 0.3~0.7 ���̷� Ʃ�� ����
                        return true;
                    }
                }
            }
        }

        // ���� ����
        auto pos_xz = owner_->world_position_vector(); pos_xz.y = 0.f;
        direction = next->position - pos_xz;

        // ��� ���� ó��(���� ����)
        float dist = xmath_util_float3::Length(direction);
        if (dist < 0.2f) {
            current_node_ = next;
            if (last_owned_node_id_ != -1) AIComponent::s_node_owner_[last_owned_node_id_] = 0;
            AIComponent::s_node_owner_[current_node_->id] = owner_->id();
            last_owned_node_id_ = current_node_->id;

            ++current_node_idx_;
        }
    }

    // ȸ�� + �̵�(����)
    direction.y = 0.f;
    direction = xmath_util_float3::Normalize(direction);
    float angle = xmath_util_float3::AngleBetween(look, direction);
    if (angle > XM_PI / 180.f * 5.f) {
        XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
        if (cross.y < 0) angle = -angle;
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
        //ȸ�� ���� ����
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
    case MonsterType::Strong_Dragon: mm.animation_track = 3; break;
    case MonsterType::Hit_Dragon:    mm.animation_track = 6; break;
    case MonsterType::Bomb_Dragon:   mm.animation_track = 5; break;
    default:                         mm.animation_track = 3; break;
    }
    
    const auto& users = SessionManager::getInstance().getAllSessions();
    for (auto& u : users) {
        u.second->do_send(&mm);
    }

}

