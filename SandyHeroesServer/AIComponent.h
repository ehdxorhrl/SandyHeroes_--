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

struct HitState
{
    bool is_attacking = false;
	float attack_time = 0.f; // ���� �ð�
	float attack_cooldown = 0.f; // ���� ��Ÿ��
};

struct StrongState
{
    bool is_attacking = false;
    float attack_time = 0.f; // ���� �ð�
    float attack_cooldown = 0.f; // ���� ��Ÿ��
};

struct SuperState
{
    bool is_attacking = false;
    bool is_fly_to_sky = false;
    bool is_revolution = false;
	bool is_move_to_target = false;
    float attack_time = 0.f; // ���� �ð�
    float attack_cooldown = 0.f; // ���� ��Ÿ��
	float revolution_time = 0.f; // ȸ�� �ð�
};;

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

static bool InRange(Object* self, Object* target, float r) { // ���� ���
    if (!self || !target) return false;
    auto d = target->world_position_vector() - self->world_position_vector();
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
    case MonsterType::Super_Dragon:
        root = Build_Super_Dragon_Tree(); 
        break;
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
    constexpr float range = 0.7f; // �ٰŸ� ���� ����
	constexpr float attack_cool_time = 1.f; // ���� ��Ÿ��
	auto state = std::make_shared<HitState>();

    // �ٰŸ� ���� ������
    auto is_attacking = [self, state](float elapsed_time) -> bool {
		constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
		constexpr float start_attack_time = animation_spf * 7.f; // ���� ���� �ð�
		constexpr float end_attack_time = animation_spf * 14.f; // ���� ���� �ð�
        if(state->is_attacking) {
            if (state->attack_time > end_attack_time)
            {
				state->is_attacking = false; // ������ �������� ���¸� �ʱ�ȭ
                state->attack_time = 0.f; // ���� �ð� �ʱ�ȭ
                return !state->is_attacking; //���� ���� �ƴϸ� ����
            }

            if (state->attack_time > start_attack_time)
            {
                auto left_arm = self->FindFrame("RigLArm2");
                auto box = Object::GetComponent<BoxColliderComponent>(left_arm);
                if (!box)
                {
                    std::cout << "������ RigLArm2�� box collider�� �����ϴ�." << std::endl;
                    return false;
                }
                //�浹 �˻�
				const auto& users = SessionManager::getInstance().getAllSessions();
                for (const auto& user : users)
                {
                    if (user.second->get_player_object()->is_dead()) continue; // �÷��̾ �׾����� �ǳʶٱ�
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(user.second->get_player_object());
                    if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                    if (box->animated_box().Intersects(player_box->animated_box())) 
                    {
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(user.second->get_player_object());
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                        playercomp->HitDamage(monstercomp->attack_force());
                        sc_packet_player_damaged pd;
                        pd.size = sizeof(sc_packet_player_damaged);
                        pd.type = S2C_P_PLAYER_DAMAGED;
                        pd.id = user.second->get_id();
                        pd.hp = playercomp->hp();
                        pd.shield = playercomp->shield();
                        for (auto& u : users) 
                        {
                            u.second->do_send(&pd);
                        }
                    }
                }
            }
		}
        return !state->is_attacking; //���� ���� �ƴϸ� ����
        };
    auto is_end_cooldown = [self, state](float elapsed_time) -> bool {
        state->attack_cooldown += elapsed_time;
        if (state->attack_cooldown >= attack_cool_time) { // 1�� ��Ÿ��
            state->attack_cooldown = 0.f; // ��Ÿ�� �ʱ�ȭ
            return true; // ��Ÿ���� �������� true ��ȯ
        }
        return false; // ���� ��Ÿ���� ������ ����
		};
    auto is_in_range = [self](float elapsed_time) -> bool {
        auto target = GetCurrentTarget(self);
        return InRangeXZ(self, target, range);
		};
    auto melee = [self, state](float elapsed_time) -> bool {
		auto* target = GetCurrentTarget(self);
		if (!target) return false; // Ÿ���� ������ ����
		state->attack_cooldown = 0.f; // ���� ��Ÿ�� �ʱ�ȭ
		state->is_attacking = true; // ���� ���·� ����

        //�ִϸ��̼� ���� ����
		sc_packet_monster_change_animation mca;
		mca.size = sizeof(sc_packet_monster_change_animation);
		mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
		mca.id = self->id();
		mca.loop_type = 1; // Once
		mca.animation_track = 7; // kSlashLeftAttack
		const auto& users = SessionManager::getInstance().getAllSessions();
		for (auto& u : users) {
			u.second->do_send(&mca);
		}

        return true;
    };

	// �÷��̾ ���� �̵�
    auto move_to_player = [self](float elapsed_time) -> bool {
        auto* target = Set_Target(self);
        if (!target) return false; 

        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;

        bool is_range = InRangeXZ(self, target, range - 0.1f);
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
        attack->children.push_back(new ActionNode(is_attacking)); 
        attack->children.push_back(new ActionNode(is_end_cooldown)); 
		attack->children.push_back(new ActionNode(is_in_range)); // ���� ���� Ÿ���� �ִ��� Ȯ��
        attack->children.push_back(new ActionNode(melee));
        root->children.push_back(attack);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    auto* movement = Object::GetComponentInChildren<MovementComponent>(self);
    movement->set_max_speed_xz(4.5f);
    return root;
}

static BTNode* Build_Strong_Dragon_Tree(Object* self)
{
	auto state = std::make_shared<StrongState>();
	constexpr float range = 3.0f; // �ٰŸ� ���� ����
    constexpr float attack_cool_time = 1.f; // ���� ��Ÿ��
    auto hp_ratio = [self]() -> float {
        auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
        if (!monstercomp) return 0.f;
        return monstercomp->hp() / monstercomp->max_hp();
    };

    //���� ���� 1ȸ ������
    auto move_to_player_dash_in_place = [self](float elapsed_time) -> bool {
        auto* target = Set_Target(self);
        if (!target) return false; 

        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;

        bool is_range = InRangeXZ(self, target, range - 0.1f);
        if (is_range) return true;

        //�ƴϸ� Ÿ�ٹ������� �̵�
        return !ai->Move_To_Target(elapsed_time); 
        };
    auto is_attacking = [self, state](float elapsed_time) -> bool {
            constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
            constexpr float start_attack_time = animation_spf * 10.f; // ���� ���� �ð�
            constexpr float end_attack_time = animation_spf * 20.f; // ���� ���� �ð�
            if (state->is_attacking) {
                state->attack_time += elapsed_time; // ���� �ð� ����
                if (state->attack_time > end_attack_time)
                {
                    state->is_attacking = false; // ������ �������� ���¸� �ʱ�ȭ
                    state->attack_time = 0.f; // ���� �ð� �ʱ�ȭ
                    return !state->is_attacking; //���� ���� �ƴϸ� ����
                }
                if (state->attack_time > start_attack_time)
                {
                    auto left_arm = self->FindFrame("RigLArm1");
                    auto right_arm = self->FindFrame("RigRArm1");
                    auto box_list = Object::GetComponentsInChildren<BoxColliderComponent>(left_arm);
                    box_list.splice(box_list.end(), Object::GetComponentsInChildren<BoxColliderComponent>(right_arm));
                    if (!box_list.size())
                    {
                        std::cout << "��� Arm�� box collider�� �����ϴ�." << std::endl;
                        return false;
                    }
                    //�浹 �˻�
                    const auto& users = SessionManager::getInstance().getAllSessions();
                    for (const auto& user : users)
                    {
                        if (user.second->get_player_object()->is_dead()) continue; // �÷��̾ �׾����� �ǳʶٱ�
                        auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(user.second->get_player_object());
                        if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                        bool is_collide = false;
                        for (const auto& box : box_list)
                        {
                            if (!box) continue; // �ڽ��� ������ �ǳʶٱ�
                            if (is_collide) break;
                            if (box->animated_box().Intersects(player_box->animated_box()))
                            {
                                is_collide = true;
                                auto playercomp = Object::GetComponentInChildren<PlayerComponent>(user.second->get_player_object());
                                auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                                playercomp->HitDamage(monstercomp->attack_force());
                                sc_packet_player_damaged pd;
                                pd.size = sizeof(sc_packet_player_damaged);
                                pd.type = S2C_P_PLAYER_DAMAGED;
                                pd.id = user.second->get_id();
                                pd.hp = playercomp->hp();
                                pd.shield = playercomp->shield();
                                for (auto& u : users)
                                {
                                    u.second->do_send(&pd);
                                }
                            }
                        }
                    }
                }
            }
            return !state->is_attacking;
            };
    auto is_end_cooldown = [self, state](float elapsed_time) -> bool {
        state->attack_cooldown += elapsed_time;
        if (state->attack_cooldown >= attack_cool_time) { // 1�� ��Ÿ��
            std::cout << "is_end_cooldown: true"<< std::endl;
            state->attack_cooldown = 0.f; // ��Ÿ�� �ʱ�ȭ
            return true; // ��Ÿ���� �������� true ��ȯ
        }            
        return false; // ���� ��Ÿ���� ������ ����
        };
    auto is_in_range = [self](float elapsed_time) -> bool {
        auto target = GetCurrentTarget(self);
        return InRangeXZ(self, target, range);
        };
    auto spin_attack_once = [self, state](float elapsed_time) -> bool {
            auto* target = GetCurrentTarget(self);
            if (!target) return false; // Ÿ���� ������ ����
            state->attack_cooldown = 0.f; // ���� ��Ÿ�� �ʱ�ȭ
            state->attack_time = 0.f;
            state->is_attacking = true; // ���� ���·� ����

            //�ִϸ��̼� ���� ����
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = self->id();
            mca.loop_type = 1; // Once
            mca.animation_track = 4; // kSpinAttackOnce
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }
            return true;
            };


    auto is_loop_attacking = [self, state](float elapsed_time) -> bool {
        if (state->is_attacking) 
        {
            state->attack_time += elapsed_time; // ���� �ð� ����
            auto left_arm = self->FindFrame("RigLArm1");
            auto right_arm = self->FindFrame("RigRArm1");
            auto box_list = Object::GetComponentsInChildren<BoxColliderComponent>(left_arm);
            box_list.splice(box_list.end(), Object::GetComponentsInChildren<BoxColliderComponent>(right_arm));
            if (!box_list.size())
            {
                std::cout << "��� Arm�� box collider�� �����ϴ�." << std::endl;
                return false;
            }
            //�浹 �˻�
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (const auto& user : users)
            {
                if (user.second->get_player_object()->is_dead()) continue; // �÷��̾ �׾����� �ǳʶٱ�
                auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(user.second->get_player_object());
                if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                bool is_collide = false;
                for (const auto& box : box_list)
                {
                    if (!box) continue; // �ڽ��� ������ �ǳʶٱ�
                    if (is_collide) break;
                    if (box->animated_box().Intersects(player_box->animated_box()))
                    {
                        is_collide = true;
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(user.second->get_player_object());
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                        playercomp->HitDamage(monstercomp->attack_force());
                        sc_packet_player_damaged pd;
                        pd.size = sizeof(sc_packet_player_damaged);
                        pd.type = S2C_P_PLAYER_DAMAGED;
                        pd.id = user.second->get_id();
                        pd.hp = playercomp->hp();
                        pd.shield = playercomp->shield();
                        for (auto& u : users)
                        {
                            u.second->do_send(&pd);
                        }
                    }
                }
            }
        }
        return true;
    };
    auto chase_target = [self](float elapsed_time) -> bool {
		auto target = Set_Target(self);
		if (!target) return false; // Ÿ���� ������ ����

        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;

        if(InRangeXZ(self, target, range)) return true;

        return ai->Move_To_Target(elapsed_time);
        };
    auto spin_attack_loop = [self, state](float elapsed_time) -> bool {
		std::cout << "���� ���� ���� ����" << std::endl;
        auto* target = GetCurrentTarget(self);
        if (!target) return false; // Ÿ���� ������ ����
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����
        //�ִϸ��̼� ���� ����
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = self->id();
        mca.loop_type = 0; // Loop
        mca.animation_track = 5; // kSpinAttackLoop
        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        return true;
		};

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();

    // ���� ������: HP > 50% �� Move(dash-in-place) �� Spin once
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() > 0.5f; }));
        seq_left->children.push_back(new ActionNode(move_to_player_dash_in_place));
        seq_left->children.push_back(new ActionNode(is_attacking));
		seq_left->children.push_back(new ActionNode(is_end_cooldown));
		seq_left->children.push_back(new ActionNode(is_in_range));
        seq_left->children.push_back(new ActionNode(spin_attack_once));
        root->children.push_back(seq_left);
    }

    // ���� ������: HP �� 50% �� Move for 3s(spin loop) �� Dash(spin loop)
    {
        auto* seq_right = new Sequence();
        seq_right->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() <= 0.5f; }));
        seq_right->children.push_back(new ActionNode(is_loop_attacking));
        seq_right->children.push_back(new ActionNode(chase_target));
        seq_right->children.push_back(new ActionNode(spin_attack_loop));
        root->children.push_back(seq_right);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_shield(500.f);
    monstercomp->set_hp(90.f);
    monstercomp->set_attack_force(40);

    return root;
}

static BTNode* Build_Super_Dragon_Tree(Object* self)
{
    auto state = std::make_shared<SuperState>();
    constexpr float kRange = 6.f; // �ٰŸ� ���� ����
	constexpr float kSpeed = 8.f; // �̵� �ӵ�
    auto hp_ratio = [self]() -> float {
        auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
        if (!monstercomp) return 0.f;
        return monstercomp->hp() / monstercomp->max_hp();
        };

    auto is_bite_attacking = [self, state](float elapsed_time) -> bool {
        constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
        constexpr float start_attack_time = animation_spf * 10.f; // ���� ���� �ð�
        constexpr float end_attack_time = animation_spf * 27.f; // ���� ���� �ð�
        if (state->is_attacking) {
            state->attack_time += elapsed_time; // ���� �ð� ����
            if (state->attack_time > end_attack_time)
            {
                state->is_attacking = false; // ������ �������� ���¸� �ʱ�ȭ
                state->attack_time = 0.f; // ���� �ð� �ʱ�ȭ
                return !state->is_attacking; //���� ���� �ƴϸ� ����
            }
            if (state->attack_time > start_attack_time)
            {
                auto head = self->FindFrame("RigHead");
                auto box = Object::GetComponent<BoxColliderComponent>(head);
                if (!box)
                {
                    std::cout << "¯��� RigHead�� box collider�� �����ϴ�." << std::endl;
                    return false;
                }
                //�浹 �˻�
                const auto& users = SessionManager::getInstance().getAllSessions();
                for (const auto& user : users)
                {
                    if (user.second->get_player_object()->is_dead()) continue; // �÷��̾ �׾����� �ǳʶٱ�
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(user.second->get_player_object());
                    if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                    if (box->animated_box().Intersects(player_box->animated_box()))
                    {
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(user.second->get_player_object());
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
                        playercomp->HitDamage(monstercomp->attack_force());
                        sc_packet_player_damaged pd;
                        pd.size = sizeof(sc_packet_player_damaged);
                        pd.type = S2C_P_PLAYER_DAMAGED;
                        pd.id = user.second->get_id();
                        pd.hp = playercomp->hp();
                        pd.shield = playercomp->shield();
                        for (auto& u : users)
                        {
                            u.second->do_send(&pd);
                        }
                    }
                }
            }
        }
		return !state->is_attacking;
        };
    auto fly_to_sky = [self, state](float elapsed_time) -> bool {
        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;
		auto movement = Object::GetComponentInChildren<MovementComponent>(self);
		if (!movement) return false; 

        if (state->is_revolution) return true;
        if (state->is_move_to_target) return true;

        // �ϴ÷� ���ƿ�����
		constexpr XMFLOAT3 target_position{ 205.3f, 23.f, -91.f }; // �ϴ÷� ���ƿ��� ��ǥ ��ġ
        XMFLOAT3 direction = target_position - self->world_position_vector();
        if (xmath_util_float3::Length(direction) < 0.2f) 
        {
            state->is_fly_to_sky = false;
            movement->Stop();
            return true; // ��ǥ ��ġ�� �����ϸ� true ��ȯ
		}
        else
        {
            if(!state->is_fly_to_sky) 
            {
                state->is_fly_to_sky = true;
                direction = xmath_util_float3::Normalize(direction);
                movement->Move(direction, kSpeed);

                XMFLOAT3 look = self->look_vector();
                look = xmath_util_float3::Normalize(look);
                float angle = xmath_util_float3::AngleBetween(look, direction);
                if (angle > XM_PI / 180.f * 5.f)
                {
                    XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
                    if (cross.y < 0)
                    {
                        angle = -angle;
                    }
                    angle = XMConvertToDegrees(angle);
                    self->Rotate(0.f, angle, 0.f);
                }

                // �ִϸ��̼� ���� ����
                sc_packet_monster_change_animation mca;
                mca.size = sizeof(sc_packet_monster_change_animation);
                mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
                mca.id = self->id();
                mca.loop_type = 0; // Loop
                mca.animation_track = 4; // kFlyUpFast
                const auto& users = SessionManager::getInstance().getAllSessions();
                for (auto& u : users) {
                    u.second->do_send(&mca);
                }
			}
            sc_packet_monster_move mm;
            mm.size = sizeof(sc_packet_monster_move);
            mm.type = S2C_P_MONSTER_MOVE;
            mm.id = self->id();
            mm.speed = kSpeed;
            XMFLOAT4X4 xf;
            const XMFLOAT4X4& mat = self->transform_matrix();
            XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
            memcpy(mm.matrix, &xf, sizeof(float) * 16);

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mm);
            }

            return false;
        }
		};
    auto revolution = [self, state](float elapsed_time) -> bool {
        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;

        if (state->is_move_to_target) return true;

		constexpr float max_revolution_time = 6.f; // ȸ�� �ð�
        if (state->revolution_time > max_revolution_time)
        {
			state->is_revolution = false; // ȸ�� ����
			state->revolution_time = 0.f; // ȸ�� �ð� �ʱ�ȭ
            return true;
        }
        if (!state->is_revolution)
        {
            state->is_revolution = true; // ȸ�� ����
            state->revolution_time = 0.f; // ȸ�� �ð� �ʱ�ȭ
            // �ִϸ��̼� ���� ����
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = self->id();
            mca.loop_type = 0; // Loop
            mca.animation_track = 5; // kFlyRightFast
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }
        }

        state->revolution_time += elapsed_time;

		constexpr float anglular_velocity = 60.f; // �ʴ� ȸ�� ����
		float angle = XMConvertToRadians(anglular_velocity * -state->revolution_time); // ���� �����ӿ��� ȸ���� ����

		constexpr XMFLOAT3 start_position{ 205.3f, 23.f, -91.f }; // ȸ�� ���� ��ġ
		constexpr XMFLOAT3 revolution_center{ 205.3f, 23.f, -67.f }; // ȸ�� �߽�
		constexpr float dx = start_position.x - revolution_center.x; // ȸ�� ���� ��ġ�� �߽��� X�� ����
		constexpr float dz = start_position.z - revolution_center.z; // ȸ�� ���� ��ġ�� �߽��� Z�� ����

		const float start_angle = atan2(dz, dx);
		angle += start_angle; // ���� ��ġ������ ������ ���� ������ ����

        // ���� wrap ó�� (0 ~ 2��)
        if (angle > XM_2PI) angle -= XM_2PI;
        if (angle < 0)      angle += XM_2PI;

		const float revolution_radius = sqrtf(dx * dx + dz * dz); // ȸ�� ������
        const XMFLOAT3 next_position{ 
            revolution_center.x + revolution_radius * cosf(angle), 
            23.f, 
            revolution_center.z + revolution_radius * sinf(angle) };

		self->set_position_vector(next_position); // ������Ʈ ��ġ ������Ʈ

        // ���� ����(����ȭ)
        XMFLOAT3 tangent{
            -sinf(angle),  // X
             0.0f,         // Y
             cosf(angle)   // Z
        };
        tangent = xmath_util_float3::Normalize(tangent);
        tangent = tangent * -1.f;
		XMFLOAT3 look = self->look_vector();
		look = xmath_util_float3::Normalize(look);
        float rotate_angle = xmath_util_float3::AngleBetween(look, tangent);
        if (rotate_angle > XM_PI / 180.f * 5.f)
        {
            XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, tangent);
            if (cross.y < 0)
            {
                rotate_angle = -rotate_angle;
            }
            rotate_angle = XMConvertToDegrees(rotate_angle);
            self->Rotate(0.f, rotate_angle, 0.f);
        }

        sc_packet_monster_move mm;
        mm.size = sizeof(sc_packet_monster_move);
        mm.type = S2C_P_MONSTER_MOVE;
        mm.id = self->id();
        mm.speed = 6.f;
        XMFLOAT4X4 xf;
        const XMFLOAT4X4& mat = self->transform_matrix();
        XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
        memcpy(mm.matrix, &xf, sizeof(float) * 16);

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mm);
        }

		return false;
		};
    auto move_to_target = [self, state](float elapsed_time) -> bool {
        auto* target = Set_Target(self);
        if (!target) return false; // Ÿ���� ������ ����
        auto* ai = Object::GetComponentInChildren<AIComponent>(self);
        if (!ai) return false;
		auto movement = Object::GetComponentInChildren<MovementComponent>(self);
		if (!movement) return false;

		constexpr float kGroundY = 3.4f; // ���� ����
		constexpr float kFlyHeight = 5.f; // ���� ����

        if (!state->is_move_to_target)
        {
            state->is_move_to_target = true;

			// �ִϸ��̼� ���� ����
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = self->id();
            mca.loop_type = 0; // Loop
            mca.animation_track = 6; // kFlyDownFast
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
			}
        }

        if (InRangeXZ(self, target, kRange + 0.5f) && self->position_vector().y < kGroundY - kFlyHeight)
        {
            movement->Stop();
            state->is_move_to_target = false;
            return true; // Ÿ�ٰ��� �Ÿ��� ����� ������ true ��ȯ
        }

        movement->Stop();

        //�ƴϸ� Ÿ�ٹ������� �̵�
		XMFLOAT3 target_position = target->world_position_vector();
		target_position.y -= kFlyHeight; // fly �ִϸ��̼��� ����Ͽ� Ÿ�� ��ġ�� �ణ ����
        XMFLOAT3 direction = target_position - self->world_position_vector();
		XMFLOAT3 direction_xz = direction;
		direction_xz.y = 0.f; 
		direction_xz = xmath_util_float3::Normalize(direction_xz);
		direction -= direction_xz * kRange; // Ÿ�� ��ġ���� �ణ �ڷ� �̵�
        direction = xmath_util_float3::Normalize(direction);
        movement->Move(direction, kSpeed);

        XMFLOAT3 look = self->look_vector();
		look.y = 0.f; 
        direction.y = 0.f;
        direction = xmath_util_float3::Normalize(direction);
        look = xmath_util_float3::Normalize(look);
        float angle = xmath_util_float3::AngleBetween(look, direction);
        if (angle > XM_PI / 180.f * 5.f)
        {
            XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
            if (cross.y < 0)
            {
                angle = -angle;
            }
            angle = XMConvertToDegrees(angle);
            self->Rotate(0.f, angle, 0.f);
        }

		const auto& users = SessionManager::getInstance().getAllSessions();
		sc_packet_monster_move mm;
		mm.size = sizeof(sc_packet_monster_move);
		mm.type = S2C_P_MONSTER_MOVE;
		mm.id = self->id();
		mm.speed = kSpeed;
		XMFLOAT4X4 xf;
		const XMFLOAT4X4& mat = self->transform_matrix();
		XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
		memcpy(mm.matrix, &xf, sizeof(float) * 16);
		for (auto& u : users) {
			u.second->do_send(&mm);
		}

        return false;
        };
    auto bite_attack = [self, state](float elapsed_time) -> bool {
        auto* target = GetCurrentTarget(self);
        if (!target) return false; // Ÿ���� ������ ����
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����

        //�ִϸ��̼� ���� ����
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = self->id();
        mca.loop_type = 1; // Once
        mca.animation_track = 7; // kFlyBiteAttackLow
        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        return true;
        };

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();
    // ���� ������: HP > 50% 
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() > 0.5f; }));
		seq_left->children.push_back(new ActionNode(is_bite_attacking)); // �ϴ÷� ���ƿ�����
		seq_left->children.push_back(new ActionNode(fly_to_sky)); // �ϴ÷� ���ƿ�����
		seq_left->children.push_back(new ActionNode(revolution)); // ȸ��
        seq_left->children.push_back(new ActionNode(move_to_target)); 
        seq_left->children.push_back(new ActionNode(bite_attack));

        root->children.push_back(seq_left);
    }

    // ���� ������: HP �� 50%
    {
        auto* seq_right = new Sequence();
        seq_right->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() <= 0.5f; }));
        root->children.push_back(seq_right);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(60);

	auto movement = Object::GetComponentInChildren<MovementComponent>(self);
	movement->set_gravity_acceleration(0.f); // �߷� ����
	movement->DisableFriction(); // ���� ����

    return root;
}