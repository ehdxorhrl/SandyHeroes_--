#pragma once
#include "stdafx.h"
#include "GameFramework.h"
#include "Component.h"
#include "SessionManager.h"
#include "AnimatorComponent.h"
#include "MonsterComponent.h"
#include "PlayerComponent.h"
#include "MovementComponent.h"
#include "BoxColliderComponent.h"
#include "WallColliderComponent.h"
#include "User.h"
#include "Object.h"

class BaseScene;

struct BombState {
    float acc = 0.f;
    bool prepared = false;
    float fuse = 2.f;
};

struct ShotState {
	std::list<Object*> fired_thorn_list;
    bool hit_someone = false;
    float acc = 0.f;
    float fuse = 0.83f;
    bool attacked = false;
};

class BTNode {
public:
    virtual ~BTNode() {}
    virtual bool Run(float elapsed_time) = 0;
};

class Selector : public BTNode {
public:
    std::vector<BTNode*> children;
    ~Selector() override { for (auto* c : children) delete c; }
    bool Run(float elapsed_time) override {
        for (auto child : children) {
            if (child->Run(elapsed_time)) {
                return true;
            }
        }
        return false;
    }
};

class Sequence : public BTNode {
public:
    std::vector<BTNode*> children;
    ~Sequence() override { for (auto* c : children) delete c; }
    bool Run(float elapsed_time) override {
        for (auto child : children) {
            if (!child->Run(elapsed_time)) {
                return false;
            }
        }
        return true;
    }
};

class ConditionNode : public BTNode {
public:
    std::function<bool(float)> condition;

    explicit ConditionNode(std::function<bool(float)> cond) : condition(std::move(cond)) {}
    explicit ConditionNode(std::function<bool()> cond)
        : condition([cond = std::move(cond)](float) { return cond(); }) {}

    bool Run(float elapsed_time) override {
        return condition(elapsed_time);
    }
};

class ActionNode : public BTNode {
public:
    std::function<bool(float)> action;

    explicit ActionNode(std::function<bool(float)> act) : action(std::move(act)) {}
    explicit ActionNode(std::function<bool()> act)
        : action([act = std::move(act)](float) { return act(); }) {}

    bool Run(float elapsed_time) override {
        return action(elapsed_time);
    }
};

class AIComponent :
    public Component
{
public:
    AIComponent(Object* owner);
    AIComponent(const AIComponent& other);
    ~AIComponent();

    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void SetBehaviorTree(BTNode* root);

    bool Move_To_Target(float dt);                       // ��� ��Ÿ��/����(�̵�/ȸ��/��Ŷ)

private:
    BTNode* behavior_tree_root_ = nullptr;
    int current_node_idx_{ 0 };
    Node* current_node_{ nullptr };
    float astar_delta_cool_time_{ 0.0f };
    std::vector<Node*> path_;
};

static Object* GetCurrentTarget(Object* self) {
    auto* monster_component = Object::GetComponentInChildren<MonsterComponent>(self);
    return monster_component->target();
}

static Object* Set_Target(Object* self) {
    auto* monster_component = Object::GetComponentInChildren<MonsterComponent>(self);

    float min_distance_sq = FLT_MAX;
    Object* nearest_player = nullptr;

    const auto& sessions = SessionManager::getInstance().getAllSessions();
    for (const auto& pair : sessions)
    {
        const auto& session = pair.second;
        Object* player = session->get_player_object();
        if (!player || player->is_dead())
            continue;

        float dist_sq = xmath_util_float3::LengthSq(player->world_position_vector() - self->world_position_vector());
        if (dist_sq < min_distance_sq)
        {
            min_distance_sq = dist_sq;
            nearest_player = player;
        }
    }

    if (nearest_player)
    {
        monster_component->set_target(nearest_player);
    }

    return nearest_player;
}

static bool InRangeXZ(Object* self, Object* target, float r) { // ���� ���
    if (!self || !target) return false;
    auto d = target->world_position_vector() - self->world_position_vector();
    d.y = 0.f;
    return xmath_util_float3::LengthSq(d) <= r * r;
}

template<typename MonsterPtr>
std::unique_ptr<AIComponent> CreateMonsterAI(MonsterPtr monster) {
    std::unique_ptr<Node> root;
    switch (monster->monster_type()) {
    case MonsterType::Hit_Dragon:
        root = Build_Hit_Dragon_Tree(); break;
    case MonsterType::Shot_Dragon:
        root = Build_Shot_Dragon_Tree(); break;
    case MonsterType::Bomb_Dragon:
        root = Build_Bomb_Dragon_Tree(); break;
    case MonsterType::Strong_Dragon:
        root = Build_Strong_Dragon_Tree(); break;
    //case MonsterType::Fly_Dragon:
    //    root = BuildFlyingTree(); break;
    default:
        break;
    }
    auto ai = std::make_unique<AIComponent>(monster);
    ai->SetBehaviorTree(std::move(root));
    return ai;
}


static BTNode* Build_Bomb_Dragon_Tree(Object* self)
{
    auto state = std::make_shared<BombState>();

    auto move_to_player = [self, state](float elapsed_time)->bool {
        if (state->prepared) return false; // prepare_to_explode ���۽� �߰� X

        auto* target = Set_Target(self);
        if (!target) return false; // Ÿ���� ������ ����
        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;

        bool is_range = InRangeXZ(self, target, 1.0f);
        if (is_range) return false; // �����ȿ� Ÿ���� ������ ������������ �Ѿ��

        //�ƴϸ� Ÿ�ٹ������� �̵�
        return ai->Move_To_Target(elapsed_time); // ��� ���������� true
    };

    auto prepare_to_explode = [self, state](float elapsed_time) -> bool { // acc�� �����ð�, fuse�� �غ�ð�

        state->acc += elapsed_time;
        if (state->acc >= state->fuse) { state->acc = 0.f; return true; }

        if (!state->prepared) {
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = self->id();
            mca.loop_type = 0;
            mca.animation_track = 2; // kGoingToExplode

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }
            state->prepared = true;
        }

        return false;
    };  

    auto explode = [self]() -> bool {
        std::cout << "�����Ϸ�" << std::endl;
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = self->id();
        mca.loop_type = 1;
        mca.animation_track = 3; // kExplode

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }

        for (const auto& player : users)
        {
            const auto& player_object = player.second->get_player_object();
            if (InRangeXZ(self, player_object, 2.0f)) {
                auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player_object);
                auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                playercomp->HitDamage(monstercomp->attack_force());
            }
        }
        
        self->set_is_dead(true);
        return true;
    };

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();
    {   
        auto* chase = new Sequence();
        chase->children.push_back(new ActionNode(move_to_player));
        root->children.push_back(chase);

        auto* attack = new Sequence();
        attack->children.push_back(new ActionNode(prepare_to_explode));
        attack->children.push_back(new ActionNode(explode));
        root->children.push_back(attack);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(50);
    monstercomp->set_shield(150);
    monstercomp->set_hp(100);
    //auto* movement = Object::GetComponentInChildren<MovementComponent>(self);

    return root;
}


static BTNode* Build_Shot_Dragon_Tree(Object* self)
{
    auto state = std::make_shared<ShotState>();

    // update thorn projectile list
    auto thorn_update = [self, state](float elapsed_time) -> bool {
        auto& fired_thorns = state->fired_thorn_list;
        const auto& users = SessionManager::getInstance().getAllSessions();

        for (auto it = fired_thorns.begin(); it != fired_thorns.end();) {
            // �浹 �˻�
            auto box = Object::GetComponentInChildren<BoxColliderComponent>(*it);
            for (const auto& user : users) // �÷��̾� �浹�˻�
            {
                if (user.second->get_player_object()->is_dead()) {
                    continue;   // �÷��̾ �׾����� �ǳʶٱ�
                }    
                auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(user.second->get_player_object());
                if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                if (box->animated_box().Intersects(player_box->animated_box())) {
                    auto playercomp = Object::GetComponentInChildren<PlayerComponent>(user.second->get_player_object());
                    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                    playercomp->HitDamage(monstercomp->attack_force());

                    //TODO: ���� ���� ��Ŷ ����
                    sc_packet_object_set_dead osd;
                    osd.size = sizeof(sc_packet_object_set_dead);
                    osd.type = S2C_P_OBJECT_SET_DEAD;
                    osd.id = (*it)->id();

                    for (auto& u : users) {
                        u.second->do_send(&osd);
                    }
                    //std::cout << "�����ϴ� thorn_id: " << osd.id << std::endl;

                    // ���� ����
                    (*it)->set_is_dead(true);
                    state->hit_someone = true;
                }
            }

            {
                auto movement = Object::GetComponentInChildren<MovementComponent>(*it);
                XMFLOAT3 velocity = movement->velocity();

                XMFLOAT3 position = (*it)->world_position_vector();
                constexpr float kGroundYOffset = 0.75f;
                position.y += kGroundYOffset;
                XMVECTOR ray_origin = XMLoadFloat3(&position);
                position.y -= kGroundYOffset;


                XMVECTOR ray_direction = XMLoadFloat3(&velocity);
                ray_direction = XMVectorSetY(ray_direction, 0);
                ray_direction = XMVector3Normalize(ray_direction);

                if (0 == XMVectorGetX(XMVector3Length(ray_direction))) {
                }

                bool is_collide = false;
                float distance{ std::numeric_limits<float>::max() };
                int a = 0;
                constexpr float MAX_DISTANCE = 0.5f;

                BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
                int stage_num = base_scene->stage_clear_num();

                for (auto& mesh_collider : base_scene->stage_wall_collider_list(stage_num))
                {
                    ++a;
                    float t{};
                    if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
                    {
                        if (t < distance)
                        {
                            distance = t;
                        }
                    }
                }
                if (stage_num - 1 >= 0)
                {
                    for (auto& mesh_collider : base_scene->stage_wall_collider_list(stage_num-1))
                    {
                        ++a;
                        float t{};
                        if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
                        {
                            if (t < distance)
                            {
                                distance = t;
                            }

                        }
                    }
                }
                if (distance < MAX_DISTANCE)
                    is_collide = true;

                //OutputDebugString(std::wstring(L"MeshColliderComponent Count: " + std::to_wstring(a) + L"\n").c_str());

                if (is_collide)
                {
                    //TODO: ���� ���� ��Ŷ ����
                    sc_packet_object_set_dead osd;
                    osd.size = sizeof(sc_packet_object_set_dead);
                    osd.type = S2C_P_OBJECT_SET_DEAD;
                    osd.id = (*it)->id();

                    for (auto& u : users) {
                        u.second->do_send(&osd);
                    }
                    std::cout << "�����ϴ� thorn_id: " << osd.id << std::endl;

                    // ���� ����
                    (*it)->set_is_dead(true);
                    state->hit_someone = true;
                }
            }

            if (state->hit_someone) {
                // ���� ������Ʈ �ı� ������ ������ ó��, ����Ʈ������ ��� ����
                it = fired_thorns.erase(it);
                state->hit_someone = false;
            }
            else {
                ++it;
            }
        }
        return true;
    };


    // ȸ��
    auto rotate = [self](float elapsed_time) -> bool {
        auto* target = Set_Target(self);
        if (!target) return false; // Ÿ���� ������ ����

        auto movement = Object::GetComponentInChildren<MovementComponent>(self);

        XMFLOAT3 look = self->look_vector();
        look.y = 0.f;
        look = xmath_util_float3::Normalize(look);
        XMFLOAT3 direction = target->world_position_vector() - self->world_position_vector();
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
            self->Rotate(0.f, angle, 0.f);
        }

        sc_packet_monster_move mm;
        mm.size = sizeof(sc_packet_monster_move);
        mm.type = S2C_P_MONSTER_MOVE;
        mm.id = self ->id();
        mm.speed = 0;
        XMFLOAT4X4 xf;
        const XMFLOAT4X4& mat = self->transform_matrix();
        XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
        memcpy(mm.matrix, &xf, sizeof(float) * 16);

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mm);
        }

        return true;
    };

    // ���� �߻�
    auto attack = [self, state](float elapsed_time) -> bool {
        if (state->attacked && (state->acc < state->fuse)) {
            state->acc += elapsed_time;
            return false;
        }

        auto* target = GetCurrentTarget(self);
        if (!target) return false; // Ÿ���� ������ ����

        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = self->id();
        mca.loop_type = 0;
        mca.animation_track = 3; // kAttack

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        // ���� ����
		XMFLOAT3 direction = target->world_position_vector() - self->world_position_vector();
		direction = xmath_util_float3::Normalize(direction);

        BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
        auto thorn_projectile = base_scene->FindModelInfo("Thorn_Projectile")->GetInstance();
        thorn_projectile->set_is_movable(true);

        //���� �߻� ����� �ٶ󺸴� ���� ��ġ
        XMFLOAT3 look = xmath_util_float3::Normalize(thorn_projectile->look_vector());
        XMFLOAT3 rotate_axis = xmath_util_float3::CrossProduct(look, direction);
        float angle = xmath_util_float3::AngleBetween(look, direction);
        XMMATRIX rotation_matrix = XMMatrixRotationAxis(XMLoadFloat3(&rotate_axis), angle);
        XMFLOAT4X4 transform_matrix = thorn_projectile->transform_matrix();
        XMStoreFloat4x4(&transform_matrix, rotation_matrix * XMLoadFloat4x4(&transform_matrix));
        thorn_projectile->set_transform_matrix(transform_matrix);

        MovementComponent* movement = new MovementComponent(thorn_projectile);
        thorn_projectile->AddComponent(movement);
        thorn_projectile->set_position_vector(self->world_position_vector());
        movement->DisableFriction();
        movement->set_gravity_acceleration(0.f);
        movement->set_max_speed_xz(4.f);
        movement->Move(direction, 4.f);
        thorn_projectile->Scale(1.f);
        base_scene->AddObject(thorn_projectile);

        std::function<void(Object*)> on_destroy_func = [state](Object* thorn) {
            state->fired_thorn_list.remove(thorn);
            };
        thorn_projectile->OnDestroy(on_destroy_func);
        
		state->fired_thorn_list.push_back(thorn_projectile);

        state->attacked = true;
        state->acc = 0.0f;

        //TODO: thorn_projectile ����ȭ
        sc_packet_shotdragon_attack sa;
        sa.size = sizeof(sc_packet_shotdragon_attack);
        sa.type = S2C_P_SHOTDRAGON_ATTACK;
        sa.id = self->id();
        sa.thorn_id = thorn_projectile->id();
        sa.dx = direction.x;
        sa.dy = direction.y;
        sa.dz = direction.z;

        std::cout << "thorn_id: " << thorn_projectile->id() << std::endl;

        for (auto& u : users) {
            u.second->do_send(&sa);
        }

        return true;
    };

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();

    // ������ �׳� ���ݸ�(ĳ���Ϳ� ���� ȸ���ҰŶ� ���� ȸ���ϴ� ���� �ʿ�X)
    {
        auto* seq = new Sequence();
		seq->children.push_back(new ActionNode(thorn_update)); // ���� ������Ʈ
        seq->children.push_back(new ActionNode(rotate));
        seq->children.push_back(new ActionNode(attack));
        root->children.push_back(seq);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(20);
    monstercomp->set_shield(100);
    monstercomp->set_hp(100);
    return root;
}

static BTNode* Build_Hit_Dragon_Tree(Object* self)
{
    // �ٰŸ� ����
    auto melee = [self](float elapsed_time) -> bool {
        // 1. ��Ÿ� �� Ÿ�� ���� Ȯ��
        // 2. ���� �ϸ� ���� �� return false + target ��Ž��, �������� ������ Ÿ������ �̵��ϵ��� 
        std::cout << "����" << std::endl;
        return true;
    };

    auto move_to_player = [self](float elapsed_time) -> bool {
        auto* target = GetCurrentTarget(self);
        if (!target)target = Set_Target(self);

        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return true;

        bool is_range = InRangeXZ(self, target, 1.0f);
        if (is_range) return false;

        //�ƴϸ� Ÿ�ٹ������� �̵�
        return ai->Move_To_Target(elapsed_time); // ��� ���������� true
    };

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();

    // ���� ��Ÿ����� Ÿ���� �����ϸ� ���� -> �ƴϸ� �̵�
    {
        auto* chase = new Sequence();
        chase->children.push_back(new ActionNode(move_to_player));
        root->children.push_back(chase);

        auto* attack = new Sequence();
        attack->children.push_back(new ActionNode(melee));
        root->children.push_back(attack);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);

    return root;
}

static BTNode* Build_Strong_Dragon_Tree(Object* self)
{

    auto hp_ratio = [self]() -> float {
        auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
        if (!monstercomp) return 0.f;
        float maxhp = monstercomp->max_hp();
        std::cout << monstercomp->hp()/maxhp << std::endl;
        return monstercomp->hp() / monstercomp->max_hp();
    };

    auto move_to_player_dash_in_place = [self](float elapsed_time) -> bool {
        // TODO: �÷��̾ ���� �̵� ���
        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;
        bool flag = ai->Move_To_Target(elapsed_time);
        return flag; // ��� ���������� true
    };

    auto spin_attack_once = [self]() -> bool {
        // TODO: ���� 1ȸ ���� Ʈ���� (�ִϸ��̼� �̺�Ʈ�� ��Ʈ ����) ���ݹ��� ���� Ÿ�� �÷��̾� �����ϸ� 1ȸ ȸ������ 
        // ��: self->PlayAnimOnce("SpinAttackOnce");
        return true;
    };

    // 3�� ���� ���� �����ϸ� �߰�: ������ �׼��� �ʿ��ϸ�
    // ������Ʈ ������ ����/Ÿ�̸Ӹ� üũ�ؼ� �Ϸ� �� true�� ��ȯ�ϼ���.
    auto move_to_player_over_3s_spin_loop = [self, acc = 0.f](float elapsed_time) -> bool {
        // TODO:
        //  - ���� ȣ�� �� ���� ���� ���� ����
        //  - self ���� Ÿ�̸Ӱ� 3�� ����ߴ��� üũ
        //  - 3�� ������ ��� �÷��̾� �߰� ����
        //  - 3�� ��� �� true, �ƴϸ� false�� ������ ���赵 ����
        // ��, ���� BT �����ӿ�ũ�� elapsed_time�� �������� �����Ƿ�
        //  Ÿ�̸Ӵ� self �ʿ��� �����ϼ���.
        // �켱 Ʈ���������� ��� ���� ó��(���� ���� �� ���������� �ٲټ���)
        return true;
    };

    auto set_other_target = [self]() -> bool {
        // TODO: spin_loop �����Ŀ� �ٸ� Ÿ�� ����
        return true;
    };

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();

    // ���� ������: HP > 50% �� Move(dash-in-place) �� Spin once
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() > 0.5f; }));
        seq_left->children.push_back(new ActionNode(move_to_player_dash_in_place));
        seq_left->children.push_back(new ActionNode(spin_attack_once));
        root->children.push_back(seq_left);
    }

    // ���� ������: HP �� 50% �� Move for 3s(spin loop) �� Dash(spin loop)
    {
        auto* seq_right = new Sequence();
        seq_right->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() <= 0.5f; }));
        seq_right->children.push_back(new ActionNode(move_to_player_over_3s_spin_loop));
        seq_right->children.push_back(new ActionNode(set_other_target));
        root->children.push_back(seq_right);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_shield(5000);
    monstercomp->set_hp(5000);

    return root;
}
