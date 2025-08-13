#pragma once
#include "Component.h"
#include "MonsterComponent.h"
#include "SessionManager.h"
#include "MovementComponent.h"
#include "AnimatorComponent.h"
#include "User.h"
#include <vector>
#include <functional>

struct BombState {
    float acc = 0.f;
    bool prepared = false;
    float fuse = 5.f;
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

private:
    BTNode* behavior_tree_root_ = nullptr;
};

// 현재 타겟을 엔진 방식에 맞게 가져오는 헬퍼 (예시)
static Object* GetCurrentTarget(Object* self) {
    auto* monster_component = Object::GetComponentInChildren<MonsterComponent>(self);
    return monster_component->target();
}

// 다른 타겟을 선택/설정하는 헬퍼 (예시)
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


inline bool Move_To_Target(Object* self, Object* target)
{
    if (!self || !target) return false;

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
        //회전 방향 연산
        XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
        if (cross.y < 0)
        {
            angle = -angle;
        }
        angle = XMConvertToDegrees(angle);
        self->Rotate(0.f, angle, 0.f);
    }

    movement->MoveXZ(direction.x, direction.z, 5.f);

    movement->set_max_speed_xz(3.5f);

    auto velocity_xz = movement->velocity();
    velocity_xz.y = 0.f;
    float speed = xmath_util_float3::Length(velocity_xz);

    sc_packet_monster_move mm;
    mm.size = sizeof(sc_packet_monster_move);
    mm.type = S2C_P_MONSTER_MOVE;
    mm.id = self->id();
    XMFLOAT4X4 xf;
    const XMFLOAT4X4& mat = self->transform_matrix();
    XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
    memcpy(mm.matrix, &xf, sizeof(float) * 16);

    const auto& users = SessionManager::getInstance().getAllSessions();
    for (auto& u : users) {
        u.second->do_send(&mm);
    }

    return true;
}

static bool InRangeXZ(Object* self, Object* target, float r) { // 범위 계산
    if (!self || !target) return false;
    auto d = target->world_position_vector() - self->world_position_vector();
    d.y = 0.f;
    return xmath_util_float3::LengthSq(d) <= r * r;
}

template<typename MonsterPtr>
std::unique_ptr<AIComponent> CreateMonsterAI(MonsterPtr monster) {
    std::unique_ptr<Node> root;
    std::cout << "Bomb_Dragon 진입완료 " << std::endl;
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

    auto prepare_to_explode = [self, state](float elapsed_time) -> bool { // acc는 누적시간, fuse는 준비시간
        state->acc += elapsed_time;
        if (state->acc >= state->fuse) { state->acc = 0.f; return true; }

        if (!state->prepared) {
            sc_packet_monster_change_animation mca;
            mca.size = sizeof(sc_packet_monster_change_animation);
            mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
            mca.id = self->id();
            mca.loop_type = 3;
            mca.animation_track = 1; // kGoingToExplode

            const auto& users = SessionManager::getInstance().getAllSessions();
            for (auto& u : users) {
                u.second->do_send(&mca);
            }
            state->prepared = true;
        }

        return false;
    };  

    auto explode = [self]() -> bool {
        std::cout << "설정완료" << std::endl;
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
        
        self->set_is_dead(true);
        return true;
    };

    // ───────────────── 트리 구성 ─────────────────
    auto* root = new Selector();

    // 쏴용은 그냥 공격만(캐릭터에 맞춰 회전할거라 따로 회전하는 노드는 필요X)
    {
        auto* seq = new Sequence();
        seq->children.push_back(new ActionNode(prepare_to_explode));
        seq->children.push_back(new ActionNode(explode));
        root->children.push_back(seq);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);

    return root;
}


static BTNode* Build_Shot_Dragon_Tree(Object* self)
{
    // 공격
    auto attack = [self](float elapsed_time) -> bool {
        auto* target = Set_Target(self);
        if (!target) return false; // 타켓이 없으면 종료

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
            //회전 방향 연산
            XMFLOAT3 cross = xmath_util_float3::CrossProduct(look, direction);
            if (cross.y < 0)
            {
                angle = -angle;
            }
            angle = XMConvertToDegrees(angle);
            self->Rotate(0.f, angle, 0.f);
        }
    };

    // ───────────────── 트리 구성 ─────────────────
    auto* root = new Selector();

    // 쏴용은 그냥 공격만(캐릭터에 맞춰 회전할거라 따로 회전하는 노드는 필요X)
    {
        auto* seq = new Sequence();
        seq->children.push_back(new ActionNode(attack));
        root->children.push_back(seq);
    }

    auto* monstercomp = Object::GetComponentInChildren<MonsterComponent>(self);

    return root;
}

static BTNode* Build_Hit_Dragon_Tree(Object* self)
{
    // 근거리 공격
    auto attack = [self](float elapsed_time) -> bool {
        // 1. 사거리 내 타겟 존재 확인
        // 2. 존재 하면 공격 후 return false + target 재탐색, 존재하지 않으면 타겟한테 이동하도록 
        return true;
    };

    auto move_to_player = [self](float elapsed_time) -> bool {
        // TODO: 플레이어를 향해 이동 명령
        auto* target = GetCurrentTarget(self);
        return Move_To_Target(self, target); // 명령 성공했으면 true
    };

    // ───────────────── 트리 구성 ─────────────────
    auto* root = new Selector();

    // 몬스터 사거리내에 타겟이 존재하면 공격 -> 아니면 이동
    {
        auto* seq = new Sequence();
        seq->children.push_back(new ActionNode(attack));
        seq->children.push_back(new ActionNode(move_to_player));
        root->children.push_back(seq);
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

    auto move_to_player_dash_in_place = [self]() -> bool {
        // TODO: 플레이어를 향해 이동 명령
        auto* target = GetCurrentTarget(self);

        if (!target) return false;
        return Move_To_Target(self, target);
    };

    auto spin_attack_once = [self]() -> bool {
        // TODO: 스핀 1회 공격 트리거 (애니메이션 이벤트로 히트 판정) 공격범위 내에 타겟 플레이어 존재하면 1회 회전공격 
        // 예: self->PlayAnimOnce("SpinAttackOnce");
        return true;
    };

    // 3초 동안 스핀 루프하며 추격: 상태형 액션이 필요하면
    // 오브젝트 내부의 상태/타이머를 체크해서 완료 시 true를 반환하세요.
    auto move_to_player_over_3s_spin_loop = [self, acc = 0.f](float elapsed_time) -> bool {
        // TODO:
        //  - 최초 호출 시 스핀 루프 상태 진입
        //  - self 내부 타이머가 3초 경과했는지 체크
        //  - 3초 동안은 계속 플레이어 추격 유지
        //  - 3초 경과 시 true, 아니면 false로 유지형 설계도 가능
        // 단, 현재 BT 프레임워크는 elapsed_time을 전달하지 않으므로
        //  타이머는 self 쪽에서 관리하세요.
        // 우선 트리거형으로 즉시 성공 처리(엔진 연결 후 상태형으로 바꾸세요)
        return true;
    };

    auto set_other_target = [self]() -> bool {
        // TODO: spin_loop 공격후에 다른 타겟 설정
        return true;
    };

    // ───────────────── 트리 구성 ─────────────────
    auto* root = new Selector();

    // 좌측 시퀀스: HP > 50% → Move(dash-in-place) → Spin once
    {
        auto* seq_left = new Sequence();
        seq_left->children.push_back(new ConditionNode([hp_ratio]() { return hp_ratio() > 0.5f; }));
        seq_left->children.push_back(new ActionNode(move_to_player_dash_in_place));
        seq_left->children.push_back(new ActionNode(spin_attack_once));
        root->children.push_back(seq_left);
    }

    // 우측 시퀀스: HP ≤ 50% → Move for 3s(spin loop) → Dash(spin loop)
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
