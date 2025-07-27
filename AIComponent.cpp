#include "stdafx.h"
#include "AIComponent.h"
#include "Object.h"

AIComponent::AIComponent(Object* owner, Object* player) : Component(owner), player_(player)
{
    Selector* root = new Selector();

    Sequence* chase_sequence = new Sequence();
    ConditionNode* detect_player = new ConditionNode([this]() { return CanSeePlayer(); });
    ActionNode* chase_player = new ActionNode([this]() { return ChasePlayer(); });
    chase_sequence->children.push_back(detect_player);
    chase_sequence->children.push_back(chase_player);

    ActionNode* idle = new ActionNode([this]() { return Idle(); });

    root->children.push_back(chase_sequence);
    root->children.push_back(idle);

    behavior_tree_root_ = root;
}

AIComponent::AIComponent(const AIComponent& other) : Component(other.owner_)
{

}

AIComponent::~AIComponent()
{
	delete behavior_tree_root_; // �޸� ���� ����
}

Component* AIComponent::GetCopy()
{
	return new AIComponent(*this);
}

void AIComponent::Update(float elapsed_time)
{
	if (behavior_tree_root_) {
		behavior_tree_root_->Run();
	}
}

void AIComponent::SetBehaviorTree(BTNode* root)
{
	behavior_tree_root_ = root;
}

bool AIComponent::CanSeePlayer() { // �ӽ� ����
    XMFLOAT3 monster_pos = owner_->position_vector();
    XMFLOAT3 player_pos = player_->position_vector();

    // �� �� ������ ���� ���� ���
    XMFLOAT3 diff = xmath_util_float3::Subtract(player_pos, monster_pos);

    // �Ÿ� ���
    float distance = xmath_util_float3::Length(diff);

    return distance < detection_range_;
}

bool AIComponent::ChasePlayer() { // �ӽ� ����
    XMFLOAT3 monster_pos = owner_->position_vector();
    XMFLOAT3 player_pos = player_->position_vector();

    // �÷��̾ ���ϴ� ���� ��� (xz ���)
    XMFLOAT3 direction = { player_pos.x - monster_pos.x, 0.f, player_pos.z - monster_pos.z };
    direction = xmath_util_float3::Normalize(direction); // ����ȭ

    // ���� ���� ���� (xz ���)
    XMFLOAT3 new_look = direction;

    // �ӵ� ���� (xz ��� �̵���)
    XMFLOAT3 velocity = xmath_util_float3::ScalarProduct(new_look, speed_);
    velocity.y = owner_->velocity().y; // y�� �ӵ��� ���� �ý��ۿ��� ����
    owner_->set_velocity(velocity);

    // ������ �ݿ��� ȸ�� ��� ���� (���� ����)
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f }; // ���� ���� ����
    XMFLOAT3 right = xmath_util_float3::CrossProduct(up, new_look);
    right = xmath_util_float3::Normalize(right);

    XMFLOAT4X4 rotation_matrix = {
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        new_look.x, new_look.y, new_look.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // ��ȯ ��� ������Ʈ (��ġ�� ����, ȸ���� ����)
    XMFLOAT4X4 current_transform = owner_->transform_matrix();
    XMFLOAT3 current_pos = { current_transform._41, current_transform._42, current_transform._43 };
    rotation_matrix._41 = current_pos.x;
    rotation_matrix._42 = current_pos.y; // y�� ��ġ�� ���� �ý��ۿ� �ñ�
    rotation_matrix._43 = current_pos.z;
    owner_->set_transform_matrix(rotation_matrix);

    return true;
}

bool AIComponent::Idle() {
    // ��� ����: �ӵ��� 0���� ����
    owner_->set_velocity(XMFLOAT3{ 0, 0, 0 });
    return true;
}