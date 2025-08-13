#include "stdafx.h"
#include "AIComponent.h"
#include "Object.h"

AIComponent::AIComponent(Object* owner) : Component(owner)
{
}

AIComponent::AIComponent(const AIComponent& other) : Component(other.owner_)
{

}

AIComponent::~AIComponent()
{
    delete behavior_tree_root_; // 메모리 누수 방지
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

