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
    float fuse = 0.533f;
};

struct ShotState {
	std::list<std::weak_ptr<Object>> fired_thorn_list;
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
	bool is_hp_low = false; // ü���� ���� ����
    bool is_attacking = false;
	bool is_move_to_target = false; // Ÿ������ �̵� ��
    float attack_time = 0.f; // ���� �ð�
    float attack_cooldown = 0.f; // ���� ��Ÿ��
	float spawn_time = 0.f; // ��ȯ �ð�
};

struct SuperState
{
	bool is_hp_low = false; // ü���� ���� ����
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
    AIComponent(const std::shared_ptr<Object>& owner);
    AIComponent(const AIComponent& other);
    ~AIComponent();

    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void SetBehaviorTree(BTNode* root);

    bool Move_To_Target(float dt);                       // ��� ��Ÿ��/����(�̵�/ȸ��/��Ŷ)

    bool Rotate_To_Target(float elapsed_time, std::shared_ptr<Object> target);

    void Send_Move_Packet(float elapsed_time, float speed);
private:
    BTNode* behavior_tree_root_ = nullptr;
    int current_node_idx_{ 0 };
    Node* current_node_{ nullptr };
    float astar_delta_cool_time_{ 0.0f };
    std::vector<Node*> path_;

    static std::unordered_map<int, int> s_node_owner_;   // node_id -> owner_id(0=����)
    static std::unordered_map<int, int> s_desire_next_;  // owner_id -> node_id

    // ���� ���� ���� ��� id(������), ��� �纸 Ÿ�̸�
    int   last_owned_node_id_{ -1 };
    float yield_timer_{ 0.f };

    bool     direct_mode_ = false;     // ���� '���� �߰�' ����ΰ�
    float    direct_mode_cooldown_ = 0.f; // ��� ��ȯ ��Ÿ��
    XMFLOAT3 last_dir_{ 0.f, 0.f, 1.f };  // ���� ��������
};

static std::shared_ptr<Object> GetCurrentTarget(std::shared_ptr<Object> self) {
    auto monster_component = Object::GetComponentInChildren<MonsterComponent>(self);
    return monster_component->target();
}

static std::shared_ptr<Object> Set_Target(std::shared_ptr<Object> self) {
    auto monster_component = Object::GetComponentInChildren<MonsterComponent>(self);

    float min_distance_sq = FLT_MAX;
    std::shared_ptr<Object> nearest_player = nullptr;

    const auto& sessions = SessionManager::getInstance().getAllSessions();
    for (const auto& pair : sessions)
    {
        const auto& session = pair.second;
        auto player = session->get_player_object();
        if (!player)
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

static bool InRangeXZ(std::shared_ptr<Object> self, std::shared_ptr<Object> target, float r) { // ���� ���
    if (!self || !target) return false;
    auto d = target->world_position_vector() - self->world_position_vector();
    d.y = 0.f;
    return xmath_util_float3::LengthSq(d) <= r * r;
}

static bool InRange(std::shared_ptr<Object> self, std::shared_ptr<Object> target, float r) { // ���� ���
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


static BTNode* Build_Bomb_Dragon_Tree(std::shared_ptr<Object> self)
{
    auto state = std::make_shared<BombState>();
	auto weak_self = std::weak_ptr<Object>(self);

    auto move_to_player = [weak_self, state](float elapsed_time)->bool {
        if (state->prepared) return false; // prepare_to_explode ���۽� �߰� X
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = Set_Target(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;

        bool is_range = InRangeXZ(locked_self, target, 1.0f);
        if (is_range) return false; // �����ȿ� Ÿ���� ������ ������������ �Ѿ��

        //�ƴϸ� Ÿ�ٹ������� �̵�
        return ai->Move_To_Target(elapsed_time); // ���� ���������� true
    };

    auto prepare_to_explode = [weak_self, state](float elapsed_time) -> bool { // acc�� �����ð�, fuse�� �غ�ð�
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        state->acc += elapsed_time;
        if (state->acc >= state->fuse) { state->acc = 0.f; return true; }

        if (!state->prepared) {
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = locked_self->id();
            mca.loop_type = 0;
            mca.animation_track = 2; // kGoingToExplode
            locked_self->set_animation_state(2);

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }
            state->prepared = true;
        }

        return false;
    };  

    auto explode = [weak_self]() -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
		if (!monstercomp) return false; 

        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = locked_self->id();
        mca.loop_type = 1;
        mca.animation_track = 3; // kExplode
        locked_self->set_animation_state(3);

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }

        for (const auto& player : users)
        {
            const auto& player_object = player.second->get_player_object();
            if (InRangeXZ(locked_self, player_object, 2.0f)) {
                auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player_object);
                playercomp->HitDamage(monstercomp->attack_force());
            }
        }
        
        monstercomp->HitDamage(9999.f);

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

    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(50);
    monstercomp->set_shield(150);
    monstercomp->set_hp(100);
    auto movement = Object::GetComponentInChildren<MovementComponent>(self);
    movement->set_max_speed_xz(2.5f);

    return root;
}

static BTNode* Build_Shot_Dragon_Tree(std::shared_ptr<Object> self)
{
    auto state = std::make_shared<ShotState>();
	std::weak_ptr<Object> weak_self = self;

    // update thorn projectile list
    auto thorn_update = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto& fired_thorns = state->fired_thorn_list;
        const auto& users = SessionManager::getInstance().getAllSessions();

        for (auto it = fired_thorns.begin(); it != fired_thorns.end();) 
        {
			auto thorn = (*it).lock();
            if(!thorn) 
            {
                it = fired_thorns.erase(it);
                continue;
			}
            ++it;
            auto thorn_position = thorn->world_position_vector();
            if (xmath_util_float3::LengthSq(thorn_position - locked_self->world_position_vector()) > 10000.f)
            {
				GameFramework::Instance()->GetScene()->DeleteObject(thorn);
                sc_packet_object_set_dead osd;
                osd.size = sizeof(sc_packet_object_set_dead);
                osd.type = S2C_P_OBJECT_SET_DEAD;
                osd.id = thorn->id();

                for (auto& u : users) {
                    u.second->do_send(&osd);
                }
                continue;
            }
            auto box = Object::GetComponentInChildren<BoxColliderComponent>(thorn);
			bool hit_player = false;
            for (const auto& user : users)
            {
				auto player = user.second->get_player_object();
                if (!player)  continue;
                auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                if (!player_box) continue;
                hit_player = box->animated_box().Intersects(player_box->animated_box());
                if (hit_player)
                {
                    auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
                    playercomp->HitDamage(monstercomp->attack_force());

                    sc_packet_object_set_dead osd;
                    osd.size = sizeof(sc_packet_object_set_dead);
                    osd.type = S2C_P_OBJECT_SET_DEAD;
                    osd.id = thorn->id();

                    for (auto& u : users) {
                        u.second->do_send(&osd);
                    }

                    GameFramework::Instance()->GetScene()->DeleteObject(thorn);
                    state->hit_someone = true;
                    break;
                }
            }

            if (!hit_player) continue;

            {
                auto movement = Object::GetComponentInChildren<MovementComponent>(thorn);
                XMFLOAT3 velocity = movement->velocity();

                XMFLOAT3 position = thorn->world_position_vector();
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

                for (auto& mesh_collider_wk : base_scene->stage_wall_collider_list(stage_num))
                {
                    ++a;
                    float t{};
					auto mesh_collider = mesh_collider_wk.lock();
					if (!mesh_collider) continue;
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
                    for (auto& mesh_collider_wk : base_scene->stage_wall_collider_list(stage_num-1))
                    {
                        ++a;
                        float t{};
						auto mesh_collider = mesh_collider_wk.lock();
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


                if (is_collide)
                {
                    sc_packet_object_set_dead osd;
                    osd.size = sizeof(sc_packet_object_set_dead);
                    osd.type = S2C_P_OBJECT_SET_DEAD;
                    osd.id = thorn->id();

                    for (auto& u : users) {
                        u.second->do_send(&osd);
                    }

					GameFramework::Instance()->GetScene()->DeleteObject(thorn);
                    state->hit_someone = true;
                }
            }
        }
        return true;
    };

    auto rotate = [weak_self](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = Set_Target(locked_self);
        if (!target) return false;    

        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;

        return ai->Rotate_To_Target(elapsed_time, target);
    };

    auto attack = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        if (state->attacked && (state->acc < state->fuse)) {
            state->acc += elapsed_time;
            return false;
        }

        auto target = GetCurrentTarget(locked_self);
        if (!target) return false;

        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = locked_self->id();
        mca.loop_type = 0;
        mca.animation_track = 3; // kAttack
        locked_self->set_animation_state(3);

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        XMFLOAT3 thorn_position = locked_self->FindFrame("AttackL")->world_position_vector();
		XMFLOAT3 direction = target->FindFrame("Root_M")->world_position_vector() - thorn_position;
		direction = xmath_util_float3::Normalize(direction);

        BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
        auto thorn_projectile = base_scene->FindModelInfo("Thorn_Projectile")->GetInstance();
        thorn_projectile->set_is_movable(true);

        XMFLOAT3 look = xmath_util_float3::Normalize(thorn_projectile->look_vector());
        XMFLOAT3 rotate_axis = xmath_util_float3::CrossProduct(look, direction);
        float angle = xmath_util_float3::AngleBetween(look, direction);
        XMMATRIX rotation_matrix = XMMatrixRotationAxis(XMLoadFloat3(&rotate_axis), angle);
        XMFLOAT4X4 transform_matrix = thorn_projectile->transform_matrix();
        XMStoreFloat4x4(&transform_matrix, rotation_matrix * XMLoadFloat4x4(&transform_matrix));
        thorn_projectile->set_transform_matrix(transform_matrix);

        auto movement = std::make_shared<MovementComponent>(thorn_projectile.get());
        thorn_projectile->AddComponent(movement);

        thorn_projectile->set_position_vector(thorn_position);
        movement->DisableFriction();
        movement->set_gravity_acceleration(0.f);
        movement->set_max_speed_xz(4.f);
        movement->Move(direction, 4.f);
        thorn_projectile->Scale(1.f);
        base_scene->AddObject(thorn_projectile);
        
		state->fired_thorn_list.push_back(thorn_projectile);

        state->attacked = true;
        state->acc = 0.0f;

        sc_packet_shotdragon_attack sa;
        sa.size = sizeof(sc_packet_shotdragon_attack);
        sa.type = S2C_P_SHOTDRAGON_ATTACK;
        sa.id = locked_self->id();
        sa.thorn_id = thorn_projectile->id();
        sa.dx = direction.x;
        sa.dy = direction.y;
        sa.dz = direction.z;
		sa.position = thorn_position;

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

    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(20);
    monstercomp->set_shield(100);
    monstercomp->set_hp(100);
    return root;
}

static BTNode* Build_Hit_Dragon_Tree(std::shared_ptr<Object> self)
{
    constexpr float range = 0.95f; // �ٰŸ� ���� ����
	constexpr float attack_cool_time = 1.f; // ���� ��Ÿ��
	auto state = std::make_shared<HitState>();

	std::weak_ptr<Object> weak_self = self;

    // �ٰŸ� ���� ������
    auto is_attacking = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
		constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
		constexpr float start_attack_time = animation_spf * 7.f; // ���� ���� �ð�
		constexpr float end_attack_time = animation_spf * 20.f; // ���� ���� �ð�

        if(state->is_attacking) {

            state->attack_time += elapsed_time;

            //std::cout << "state->is_attacking ����" << std::endl;
            //std::cout << "state->attack_time �ð�: " << state->attack_time << std::endl;
            if (state->attack_time > end_attack_time)
            {
                //std::cout << "end_attack_time ����" << std::endl;
				state->is_attacking = false; // ������ �������� ���¸� �ʱ�ȭ
                state->attack_time = 0.f; // ���� �ð� �ʱ�ȭ
                return !state->is_attacking; //���� ���� �ƴϸ� ����
            }

            if (state->attack_time > start_attack_time)
            {
                //std::cout << "start_attack_time ����" << std::endl;
                //std::cout << "state->attack_time �ð�: " << state->attack_time << std::endl;
                auto left_arm = locked_self->FindFrame("RigLArm2");
                auto box = Object::GetComponent<BoxColliderComponent>(left_arm);
                if (!box)
                {
                    std::cout << "������ RigLArm2�� box collider�� �����ϴ�." << std::endl;
                    return false;
                }
				const auto& users = SessionManager::getInstance().getAllSessions();
                for (const auto& user : users)
                {
					auto player = user.second->get_player_object();
                    if (!player) continue; 
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                    if (!player_box) continue; 
                    if (box->animated_box().Intersects(player_box->animated_box())) 
                    {
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
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
    auto is_end_cooldown = [state](float elapsed_time) -> bool {
        state->attack_cooldown += elapsed_time;
        if (state->attack_cooldown >= attack_cool_time) { // 1�� ��Ÿ��
            state->attack_cooldown = 0.f; // ��Ÿ�� �ʱ�ȭ
            return true; // ��Ÿ���� �������� true ��ȯ
        }
        return false; // ���� ��Ÿ���� ������ ����
		};
    auto is_in_range = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        return InRangeXZ(locked_self, target, range);
		};
    auto melee = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		auto target = GetCurrentTarget(locked_self);
		if (!target) return false; // Ÿ���� ������ ����
		state->attack_cooldown = 0.f; // ���� ��Ÿ�� �ʱ�ȭ
		state->is_attacking = true; // ���� ���·� ����

        // ȸ�� �� ����
        auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);

        XMFLOAT3 look = locked_self->look_vector();
        look.y = 0.f;
        look = xmath_util_float3::Normalize(look);
        XMFLOAT3 direction = target->world_position_vector() - locked_self->world_position_vector();
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
            locked_self->Rotate(0.f, angle, 0.f);
        }

        //�ִϸ��̼� ���� ����
		sc_packet_monster_change_animation mca;
		mca.size = sizeof(sc_packet_monster_change_animation);
		mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
		mca.id = locked_self->id();
		mca.loop_type = 1; // Once
		mca.animation_track = 7; // kSlashLeftAttack
        locked_self->set_animation_state(7);
		const auto& users = SessionManager::getInstance().getAllSessions();
		for (auto& u : users) {
			u.second->do_send(&mca);
		}

        return true;
    };

	// �÷��̾ ���� �̵�
    auto move_to_player = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        if (state->is_attacking) {
            return false;
        }
        auto target = Set_Target(locked_self);
        if (!target) return false; 

        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;

        bool is_range = InRangeXZ(locked_self, target, range - 0.1f);
        if (is_range) return false; 

        //�ƴϸ� Ÿ�ٹ������� �̵�
        return ai->Move_To_Target(elapsed_time); // ���� ���������� true
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

    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(15);
	monstercomp->set_shield(80);
	monstercomp->set_hp(150);

    auto movement = Object::GetComponentInChildren<MovementComponent>(self);
    movement->set_max_speed_xz(3.f);
    return root;
}

static BTNode* Build_Strong_Dragon_Tree(std::shared_ptr<Object> self)
{
	auto state = std::make_shared<StrongState>();
	constexpr float range = 1.6f; // �ٰŸ� ���� ����
    constexpr float attack_cool_time = 1.f; // ���� ��Ÿ��
	std::weak_ptr<Object> weak_self = self;
    auto spawn_wait = [state](float elapsed_time) -> bool {
        constexpr float kSpawnWaitTime = 3.f; // ���� ��� �ð�
        if (state->spawn_time < kSpawnWaitTime) {
            state->spawn_time += elapsed_time;
            return true; // ���� ��ٸ��� ��
        }
        return false; // ��ٸ� �Ϸ�
        };

    auto shield = [weak_self]() -> float {
		auto locked_self = weak_self.lock();
		if (!locked_self) return 0.f;
        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
        if (!monstercomp) return 0.f;
        return monstercomp->shield();
    };

    //���� ���� 1ȸ ������
    auto is_attacking = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
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
                auto left_arm = locked_self->FindFrame("RigLArm1");
                auto right_arm = locked_self->FindFrame("RigRArm1");
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
					auto player = user.second->get_player_object();
                    if (!player) continue; // �÷��̾ �׾����� �ǳʶٱ�
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                    if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                    bool is_collide = false;
                    for (const auto& box : box_list)
                    {
                        if (!box) continue; // �ڽ��� ������ �ǳʶٱ�
                        if (is_collide) break;
                        if (box->animated_box().Intersects(player_box->animated_box()))
                        {
                            is_collide = true;
                            auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                            auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
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
    auto move_to_player_dash_in_place = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = Set_Target(locked_self);
        if (!target) return false; 

        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;

		auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);
		if (!movement) return false;

        bool is_range = InRangeXZ(locked_self, target, range - 0.2f);
        if (is_range)
        {
            movement->Stop();
            state->is_move_to_target = false;
            return true;
        }

        if(!state->is_move_to_target) {
            state->is_move_to_target = true;

			sc_packet_monster_change_animation mca;
			mca.size = sizeof(sc_packet_monster_change_animation);
			mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
			mca.id = locked_self->id();
			mca.loop_type = 0; // Loop
			mca.animation_track = 3; // kDashInPlace
            locked_self->set_animation_state(3);
		}

        //�ƴϸ� Ÿ�ٹ������� �̵�
        XMFLOAT3 target_position = target->world_position_vector();
        XMFLOAT3 direction = target_position - locked_self->world_position_vector();
        XMFLOAT3 direction_xz = direction;
        direction_xz.y = 0.f;
        direction_xz = xmath_util_float3::Normalize(direction_xz);
        direction = xmath_util_float3::Normalize(direction);
        movement->Move(direction_xz, 5.f);

        XMFLOAT3 look = locked_self->look_vector();
        look.y = 0.f;
        direction = target_position - locked_self->world_position_vector(); // �׻� Ÿ���� �ٶ󺸵���
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
            locked_self->Rotate(0.f, angle, 0.f);
        }

        const auto& users = SessionManager::getInstance().getAllSessions();
        sc_packet_monster_move mm;
        mm.size = sizeof(sc_packet_monster_move);
        mm.type = S2C_P_MONSTER_MOVE;
        mm.id = locked_self->id();
        mm.speed = 5.f;
		mm.animation_track = 3; // kDashInPlace
        XMFLOAT4X4 xf;
        const XMFLOAT4X4& mat = locked_self->transform_matrix();
        XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
        memcpy(mm.matrix, &xf, sizeof(float) * 16);
        for (auto& u : users) {
            u.second->do_send(&mm);
        }

        return false; 
        };
    auto is_end_cooldown = [state](float elapsed_time) -> bool {
        state->attack_cooldown += elapsed_time;
        if (state->attack_cooldown >= attack_cool_time) { // 1�� ��Ÿ��
            state->attack_cooldown = 0.f; // ��Ÿ�� �ʱ�ȭ
            return true; // ��Ÿ���� �������� true ��ȯ
        }            
        return false; // ���� ��Ÿ���� ������ ����
        };
    auto is_in_range = [weak_self](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        return InRangeXZ(locked_self, target, range);
        };
    auto spin_attack_once = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
        state->attack_cooldown = 0.f; // ���� ��Ÿ�� �ʱ�ȭ
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����

        //�ִϸ��̼� ���� ����
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = locked_self->id();
        mca.loop_type = 1; // Once
        mca.animation_track = 4; // kSpinAttackOnce
        locked_self->set_animation_state(4);
        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        return true;
        };


    auto is_loop_attacking = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        if (!state->is_hp_low)
        {
            state->is_hp_low = true;
			state->attack_cooldown = 0.f; // ���� ��Ÿ�� �ʱ�ȭ
            state->attack_time = 0.f; // ���� �ð� �ʱ�ȭ
            state->is_attacking = false; // ���� ���� �ʱ�ȭ
            return false;
        }
        if (state->is_attacking) 
        {
            state->attack_time += elapsed_time; // ���� �ð� ����
            auto left_arm = locked_self->FindFrame("RigLArm1");
            auto right_arm = locked_self->FindFrame("RigRArm1");
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
				auto player = user.second->get_player_object();
                if (!player) continue; // �÷��̾ �׾����� �ǳʶٱ�
                auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                bool is_collide = false;
                for (const auto& box : box_list)
                {
                    if (!box) continue; // �ڽ��� ������ �ǳʶٱ�
                    if (is_collide) break;
                    if (box->animated_box().Intersects(player_box->animated_box()))
                    {
                        is_collide = true;
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
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
    auto chase_target = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
		auto target = Set_Target(locked_self);
		if (!target) return false; // Ÿ���� ������ ����

        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;

        auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);
        if (!movement) return false;

        bool is_range = InRangeXZ(locked_self, target, range - 0.1f);
        if (is_range)
        {
            movement->set_velocity(XMFLOAT3{0.f,0.f,0.f});

            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = locked_self->id();
            mca.loop_type = 0; // Loop
            mca.animation_track = 5; // kSpinAttackLoop
            locked_self->set_animation_state(5);
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }

            state->is_move_to_target = false;
            state->attack_time = 0.f;
            state->is_attacking = true; // ���� ���·� ����
            return false;
        }

        if (!state->is_move_to_target) {
            state->is_move_to_target = true;
        }

        //�ƴϸ� Ÿ�ٹ������� �̵�
        XMFLOAT3 target_position = target->world_position_vector();
        XMFLOAT3 direction = target_position - locked_self->world_position_vector();
        XMFLOAT3 direction_xz = direction;
        direction_xz.y = 0.f;
        direction_xz = xmath_util_float3::Normalize(direction_xz);
        direction = xmath_util_float3::Normalize(direction);
        movement->Move(direction_xz, 5.f);

        XMFLOAT3 look = locked_self->look_vector();
        look.y = 0.f;
        direction = target_position - locked_self->world_position_vector(); // �׻� Ÿ���� �ٶ󺸵���
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
            locked_self->Rotate(0.f, angle, 0.f);
        }

        const auto& users = SessionManager::getInstance().getAllSessions();
        sc_packet_monster_move mm;
        mm.size = sizeof(sc_packet_monster_move);
        mm.type = S2C_P_MONSTER_MOVE;
        mm.id = locked_self->id();
        mm.speed = 5.f;
        mm.animation_track = 5; // kDashInPlace
        XMFLOAT4X4 xf;
        const XMFLOAT4X4& mat = locked_self->transform_matrix();
        XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
        memcpy(mm.matrix, &xf, sizeof(float) * 16);
        for (auto& u : users) {
            u.second->do_send(&mm);
        }

        return false;
        };
    auto spin_attack_loop = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
		if (state->is_attacking) return false; // �̹� ���� ���̸� ����
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����
        //�ִϸ��̼� ���� ����
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = locked_self->id();
        mca.loop_type = 0; // Loop
        mca.animation_track = 5; // kSpinAttackLoop
        locked_self->set_animation_state(5);
        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        return true;
		};

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();

	auto wait_seq = new Sequence();
	wait_seq->children.push_back(new ActionNode(spawn_wait)); // ���� ���
	root->children.push_back(wait_seq);

    // ���� ������: HP > 50% �� Move(dash-in-place) �� Spin once
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([shield]() { return shield() > 0.1f; }));
        seq_left->children.push_back(new ActionNode(is_attacking));
        seq_left->children.push_back(new ActionNode(move_to_player_dash_in_place));
		seq_left->children.push_back(new ActionNode(is_end_cooldown));
		seq_left->children.push_back(new ActionNode(is_in_range));
        seq_left->children.push_back(new ActionNode(spin_attack_once));
        root->children.push_back(seq_left);
    }

    // ���� ������: HP �� 50% �� Move for 3s(spin loop) �� Dash(spin loop)
    {
        auto* seq_right = new Sequence();
        seq_right->children.push_back(new ConditionNode([shield]() { return shield() <= 0.1f; }));
        seq_right->children.push_back(new ActionNode(is_loop_attacking));
        seq_right->children.push_back(new ActionNode(chase_target));
        //seq_right->children.push_back(new ActionNode(spin_attack_loop));
        root->children.push_back(seq_right);
    }

    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_shield(1000.f);
    monstercomp->set_hp(2000.f);
    monstercomp->set_attack_force(40);

	auto movement = Object::GetComponentInChildren<MovementComponent>(self);
    if (movement) {
        movement->set_max_speed_xz(3.f);
	}

    return root;
}

static BTNode* Build_Super_Dragon_Tree(std::shared_ptr<Object> self)
{
    auto state = std::make_shared<SuperState>();
    constexpr float kRange = 7.f; // �ٰŸ� ���� ����
	constexpr float kSpeed = 8.f; // �̵� �ӵ�

	std::weak_ptr<Object> weak_self = self;

    auto shield = [weak_self]() -> float {
		auto locked_self = weak_self.lock();
		if (!locked_self) return 0.f;
        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
        if (!monstercomp) return 0.f;
        return monstercomp->shield();
        };

    auto is_bite_attacking = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
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
                auto head = locked_self->FindFrame("RigHead");
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
					auto player = user.second->get_player_object();
                    if (!player) continue; // �÷��̾ �׾����� �ǳʶٱ�
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                    if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                    if (box->animated_box().Intersects(player_box->animated_box()))
                    {
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
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

    auto fly_to_sky = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;

        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;
		auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);
		if (!movement) return false; 

        if (state->is_revolution) return true;
        if (state->is_move_to_target) return true;

		constexpr XMFLOAT3 target_position{ 205.3f, 23.f, -91.f }; // �ϴ÷� ���ƿ��� ��ǥ ��ġ
        XMFLOAT3 direction = target_position - locked_self->world_position_vector();
        if (xmath_util_float3::Length(direction) < 0.5f) 
        {
            state->is_fly_to_sky = false;
            movement->set_velocity(XMFLOAT3{ 0.f,0.f,0.f });
            return true; 
		}
        else
        {
            if(!state->is_fly_to_sky) 
            {
                if (state->is_hp_low)
                {
					std::cout << "¯��� fly_to_sky ����" << std::endl;
                }
                state->is_fly_to_sky = true;
                direction = xmath_util_float3::Normalize(direction);
                movement->Move(direction, kSpeed);

                XMFLOAT3 look = locked_self->look_vector();
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
                    locked_self->Rotate(0.f, angle, 0.f);
                }

                // �ִϸ��̼� ���� ����
                sc_packet_monster_change_animation mca;
                mca.size = sizeof(sc_packet_monster_change_animation);
                mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
                mca.id = locked_self->id();
                mca.loop_type = 0; // Loop
                mca.animation_track = 4; // kFlyUpFast
                locked_self->set_animation_state(4);
                const auto& users = SessionManager::getInstance().getAllSessions();
                for (auto& u : users) {
                    u.second->do_send(&mca);
                }
			}
            sc_packet_monster_move mm;
            mm.size = sizeof(sc_packet_monster_move);
            mm.type = S2C_P_MONSTER_MOVE;
            mm.id = locked_self->id();
            mm.speed = kSpeed;
			mm.animation_track = 4; // kFlyUpFast
            XMFLOAT4X4 xf;
            const XMFLOAT4X4& mat = locked_self->transform_matrix();
            XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
            memcpy(mm.matrix, &xf, sizeof(float) * 16);

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mm);
            }

            return false;
        }
		};

    auto revolution = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
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
            mca.id = locked_self->id();
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

		locked_self->set_position_vector(next_position); // ������Ʈ ��ġ ������Ʈ

        // ���� ����(����ȭ)
        XMFLOAT3 tangent{
            -sinf(angle),  // X
             0.0f,         // Y
             cosf(angle)   // Z
        };
        tangent = xmath_util_float3::Normalize(tangent);
        tangent = tangent * -1.f;
		XMFLOAT3 look = locked_self->look_vector();
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
            locked_self->Rotate(0.f, rotate_angle, 0.f);
        }

        sc_packet_monster_move mm;
        mm.size = sizeof(sc_packet_monster_move);
        mm.type = S2C_P_MONSTER_MOVE;
        mm.id = locked_self->id();
        mm.speed = 6.f;
		mm.animation_track = 5; // kFlyRightFast
        XMFLOAT4X4 xf;
        const XMFLOAT4X4& mat = locked_self->transform_matrix();
        XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
        memcpy(mm.matrix, &xf, sizeof(float) * 16);

        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mm);
        }

		return false;
		};

    auto move_to_target = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = Set_Target(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
        auto ai = Object::GetComponentInChildren<AIComponent>(locked_self);
        if (!ai) return false;
		auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);
		if (!movement) return false;

		constexpr float kGroundY = 3.6f; // ���� ����
		constexpr float kFlyHeight = 2.1f; // ���� ����

        if (!state->is_move_to_target)
        {
            state->is_move_to_target = true;

			// �ִϸ��̼� ���� ����
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = locked_self->id();
            mca.loop_type = 0; // Loop
            mca.animation_track = 6; // kFlyDownFast
            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
			}
            locked_self->set_animation_state(6);
        }

        if (InRangeXZ(locked_self, target, kRange + 0.5f) && locked_self->position_vector().y < kGroundY - kFlyHeight)
        {
            movement->set_velocity(XMFLOAT3{ 0.f,0.f,0.f });
            state->is_move_to_target = false;
            return true; // Ÿ�ٰ��� �Ÿ��� ����� ������ true ��ȯ
        }

        movement->set_velocity(XMFLOAT3{ 0.f,0.f,0.f });

        //�ƴϸ� Ÿ�ٹ������� �̵�
		XMFLOAT3 target_position = target->world_position_vector();
		target_position.y -= kFlyHeight; // fly �ִϸ��̼��� �����Ͽ� Ÿ�� ��ġ�� �ణ ����
        XMFLOAT3 direction = target_position - locked_self->world_position_vector();
		XMFLOAT3 direction_xz = direction;
		direction_xz.y = 0.f; 
		direction_xz = xmath_util_float3::Normalize(direction_xz);
		direction -= direction_xz * kRange; // Ÿ�� ��ġ���� �ణ �ڷ� �̵�
        direction = xmath_util_float3::Normalize(direction);
        movement->Move(direction, kSpeed);

        XMFLOAT3 look = locked_self->look_vector();
		look.y = 0.f; 
        direction = target_position - locked_self->world_position_vector(); // �׻� Ÿ���� �ٶ󺸵���
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
            locked_self->Rotate(0.f, angle, 0.f);
        }

		const auto& users = SessionManager::getInstance().getAllSessions();
		sc_packet_monster_move mm;
		mm.size = sizeof(sc_packet_monster_move);
		mm.type = S2C_P_MONSTER_MOVE;
		mm.id = locked_self->id();
		mm.speed = kSpeed;
		mm.animation_track = 6; // kFlyDownFast
		XMFLOAT4X4 xf;
		const XMFLOAT4X4& mat = locked_self->transform_matrix();
		XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
		memcpy(mm.matrix, &xf, sizeof(float) * 16);
		for (auto& u : users) {
			u.second->do_send(&mm);
		}

        return false;
        };

    auto bite_attack = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����

        //�ִϸ��̼� ���� ����
        sc_packet_monster_change_animation mca;
        mca.size = sizeof(sc_packet_monster_change_animation);
        mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
        mca.id = locked_self->id();
        mca.loop_type = 1; // Once
        mca.animation_track = 7; // kFlyBiteAttackLow
        locked_self->set_animation_state(7);
        const auto& users = SessionManager::getInstance().getAllSessions();
        for (auto& u : users) {
            u.second->do_send(&mca);
        }
        return true;
        };

    auto is_breath_attacking = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto movement = Object::GetComponentInChildren<MovementComponent>(locked_self);
        if (!movement) return false;

        if (!state->is_hp_low)
        {
            movement->set_velocity(XMFLOAT3{ 0.f,0.f,0.f });
            state->is_hp_low = true;
            state->is_attacking = false;
            state->is_fly_to_sky = false;
            state->is_revolution = false;
            state->is_move_to_target = false;
            state->attack_time = 0.f; // ���� �ð�
            state->attack_cooldown = 0.f; // ���� ��Ÿ��
            state->revolution_time = 0.f; // ȸ�� �ð�
            return false;
        }

        constexpr float animation_spf = 0.03f; // ���� �ִϸ��̼� �����Ӵ� �ð�
        constexpr float start_attack_time = animation_spf * 13.f; // ���� ���� �ð�
        constexpr float end_attack_time = animation_spf * 40.f; // ���� ���� �ð�
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
                auto head = locked_self->FindFrame("Breath");
                auto box = Object::GetComponent<BoxColliderComponent>(head);
                if (!box)
                {
                    std::cout << "¯��� Breath�� box collider�� �����ϴ�." << std::endl;
                    return false;
                }
                //�浹 �˻�
                const auto& users = SessionManager::getInstance().getAllSessions();
                for (const auto& user : users)
                {
					auto player = user.second->get_player_object();
                    if (!player) continue; // �÷��̾ �׾����� �ǳʶٱ�
                    auto player_box = Object::GetComponentInChildren<BoxColliderComponent>(player);
                    if (!player_box) continue; // �÷��̾� �ڽ��� ������ �ǳʶٱ�
                    if (box->animated_box().Intersects(player_box->animated_box()))
                    {
                        auto playercomp = Object::GetComponentInChildren<PlayerComponent>(player);
                        auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(locked_self);
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

    auto breath_attack = [weak_self, state](float elapsed_time) -> bool {
		auto locked_self = weak_self.lock();
		if (!locked_self) return false;
        auto target = GetCurrentTarget(locked_self);
        if (!target) return false; // Ÿ���� ������ ����
        state->attack_time = 0.f;
        state->is_attacking = true; // ���� ���·� ����

        if (locked_self->animation_state() != 8) {
            //�ִϸ��̼� ���� ����
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = locked_self->id();
            mca.loop_type = 1; // Once
            mca.animation_track = 8; // kFlyFireBreathAttackLow
            locked_self->set_animation_state(8);

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users)
            {
                u.second->do_send(&mca);
            }
        }
   
        return true;
		};

    // ���������������������������������� Ʈ�� ���� ����������������������������������
    auto* root = new Selector();
    // ���� ������: HP > 50% 
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([shield]() { return shield() > 0.1f; }));
		seq_left->children.push_back(new ActionNode(is_bite_attacking)); 
		seq_left->children.push_back(new ActionNode(fly_to_sky)); // �ϴ÷� ���ƿ�����
		seq_left->children.push_back(new ActionNode(revolution)); // ȸ��
        seq_left->children.push_back(new ActionNode(move_to_target)); 
        seq_left->children.push_back(new ActionNode(bite_attack));

        root->children.push_back(seq_left);
    }

    // ���� ������: HP �� 50%
    {
        auto* seq_right = new Sequence();
        seq_right->children.push_back(new ConditionNode([shield]() { return shield() <= 0.1f; }));
        seq_right->children.push_back(new ActionNode(is_breath_attacking));
        seq_right->children.push_back(new ActionNode(fly_to_sky)); // �ϴ÷� ���ƿ�����
        seq_right->children.push_back(new ActionNode(revolution)); // ȸ��
        seq_right->children.push_back(new ActionNode(move_to_target));
        seq_right->children.push_back(new ActionNode(breath_attack));

        root->children.push_back(seq_right);
    }

    auto monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);
    monstercomp->set_attack_force(60);
	monstercomp->set_hp(4000.f);
	monstercomp->set_shield(2000.f);

	auto movement = Object::GetComponentInChildren<MovementComponent>(self);
	movement->set_gravity_acceleration(0.f); // �߷� ����
	movement->DisableFriction(); // ���� ����

    return root;
}
