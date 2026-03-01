import re

with open('SandyHeroesServer/AIComponent.h', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace Object* self with std::shared_ptr<Object> self
content = re.sub(r'Object\*\s+self', r'std::shared_ptr<Object> self', content)

# Remove auto* when calling Object::GetComponentInChildren or GetComponent
content = re.sub(r'auto\*\s+(\w+)\s*=\s*Object::Get', r'auto \1 = Object::Get', content)

with open('SandyHeroesServer/AIComponent.h', 'w', encoding='utf-8') as f:
    f.write(content)

with open('SandyHeroesServer/AIComponent.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# AIComponent::AIComponent(const AIComponent& other) : Component(other.owner_)
content = content.replace('Component(other.owner_)', 'Component(other)')

# AIComponent::~AIComponent()
#     AIComponent::s_desire_next_.erase(owner_->id());
# -> if (auto owner_ptr = owner()) AIComponent::s_desire_next_.erase(owner_ptr->id());
content = content.replace('AIComponent::s_desire_next_.erase(owner_->id());', 'if (auto owner_ptr = owner()) AIComponent::s_desire_next_.erase(owner_ptr->id());')

# Replace auto* monstercomponent = Object::GetComponentInChildren... with auto monstercomponent
content = re.sub(r'auto\*\s+(\w+)\s*=\s*Object::Get', r'auto \1 = Object::Get', content)

# Replace owner_-> with owner_ptr-> inside methods.
# We need to inject `auto owner_ptr = owner(); if(!owner_ptr) return false;` at the beginning of Move_To_Target and Rotate_To_Target.
# For Send_Move_Packet, it returns void, so `if(!owner_ptr) return;`

move_to_target_orig = """bool AIComponent::Move_To_Target(float elapsed_time) 
{"""
move_to_target_new = """bool AIComponent::Move_To_Target(float elapsed_time) 
{
    auto owner_ptr = owner();
    if (!owner_ptr) return false;"""
content = content.replace(move_to_target_orig, move_to_target_new)

rotate_to_target_orig = """bool AIComponent::Rotate_To_Target(float elapsed_time, std::shared_ptr<Object> target) {"""
rotate_to_target_new = """bool AIComponent::Rotate_To_Target(float elapsed_time, std::shared_ptr<Object> target) {
    auto owner_ptr = owner();
    if (!owner_ptr) return false;"""
content = content.replace(rotate_to_target_orig, rotate_to_target_new)

send_move_packet_orig = """void AIComponent::Send_Move_Packet(float elapsed_time, float speed)
{"""
send_move_packet_new = """void AIComponent::Send_Move_Packet(float elapsed_time, float speed)
{
    auto owner_ptr = owner();
    if (!owner_ptr) return;"""
content = content.replace(send_move_packet_orig, send_move_packet_new)

# Now replace owner_-> with owner_ptr-> in the rest of the file
content = content.replace('owner_->', 'owner_ptr->')
# Also replace owner_ inside GetComponentInChildren
content = content.replace('Object::GetComponentInChildren<MonsterComponent>(owner_)', 'Object::GetComponentInChildren<MonsterComponent>(owner_ptr)')
content = content.replace('Object::GetComponentInChildren<MovementComponent>(owner_)', 'Object::GetComponentInChildren<MovementComponent>(owner_ptr)')

with open('SandyHeroesServer/AIComponent.cpp', 'w', encoding='utf-8') as f:
    f.write(content)
