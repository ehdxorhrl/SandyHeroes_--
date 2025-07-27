#pragma once
#include "Component.h"
#include <vector>
#include <functional>

class BTNode {
public:
    virtual ~BTNode() {}
    virtual bool Run() = 0;
};

class Selector : public BTNode {
public:
    std::vector<BTNode*> children;
    bool Run() override {
        for (auto child : children) {
            if (child->Run()) {
                return true;
            }
        }
        return false;
    }
};

class Sequence : public BTNode {
public:
    std::vector<BTNode*> children;
    bool Run() override {
        for (auto child : children) {
            if (!child->Run()) {
                return false;
            }
        }
        return true;
    }
};

class ConditionNode : public BTNode {
public:
    std::function<bool()> condition;
    ConditionNode(std::function<bool()> cond) : condition(cond) {}
    bool Run() override {
        return condition();
    }
};

class ActionNode : public BTNode {
public:
    std::function<bool()> action;
    ActionNode(std::function<bool()> act) : action(act) {}
    bool Run() override {
        return action();
    }
};

class AIComponent :
    public Component
{
public:
    AIComponent(Object* owner, Object* player);
    AIComponent(const AIComponent& other);
    ~AIComponent();

    virtual Component* GetCopy() override;
    virtual void Update(float elapsed_time) override;

    void SetBehaviorTree(BTNode* root);

private:
    BTNode* behavior_tree_root_ = nullptr;
    Object* player_ = nullptr;
    float detection_range_ = 10.0f; // ���� ����
    float speed_ = 5.0f;            // ���� �ӵ�

    bool CanSeePlayer(); // �÷��̾� ���� ����
    bool ChasePlayer();  // �÷��̾� ���� �ൿ
    bool Idle();         // �⺻ �ൿ (���)
};

