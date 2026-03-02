import sys

def modify_server_movement():
    with open('SandyHeroesServer/MovementComponent.cpp', 'r', encoding='ansi') as f:
        content = f.read()

    # 1. Update
    content = content.replace('void MovementComponent::Update(float elapsed_time)
{
    old_position_ = owner_->position_vector();', 
                              'void MovementComponent::Update(float elapsed_time)
{
    auto locked_owner = owner_.lock();
    if(!locked_owner) return;

    old_position_ = locked_owner->position_vector();')
    
    content = content.replace('if (owner_->is_ground() && velocity_.y < 0.f)', 'if (locked_owner->is_ground() && velocity_.y < 0.f)')
    content = content.replace('<= owner_->position_vector().y)', '<= locked_owner->position_vector().y)')
    content = content.replace('owner_->set_position_vector(owner_->position_vector() + (velocity_ * (elapsed_time)));', 'locked_owner->set_position_vector(locked_owner->position_vector() + (velocity_ * (elapsed_time)));')
    
    # 2. Jump
    content = content.replace('void MovementComponent::Jump(float speed, float max_height)
{
    jump_before_y_ = owner_->position_vector().y;',
                              'void MovementComponent::Jump(float speed, float max_height)
{
    auto locked_owner = owner_.lock();
    if(!locked_owner) return;
    jump_before_y_ = locked_owner->position_vector().y;')

    # 3. Stop
    content = content.replace('void MovementComponent::Stop()
{
    velocity_ = XMFLOAT3{ 0.f, 0.f, 0.f };

    sc_packet_monster_change_animation mca;
    mca.size = sizeof(sc_packet_monster_change_animation);
    mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
    mca.id = owner_->id();',
                              'void MovementComponent::Stop()
{
    velocity_ = XMFLOAT3{ 0.f, 0.f, 0.f };

    auto locked_owner = owner_.lock();
    if(!locked_owner) return;

    sc_packet_monster_change_animation mca;
    mca.size = sizeof(sc_packet_monster_change_animation);
    mca.type = S2C_P_MONSTER_CHANGE_ANIMATION;
    mca.id = locked_owner->id();')
    
    content = content.replace('switch (owner_->monster_type()) {', 'switch (locked_owner->monster_type()) {')

    # 4. BackupPosition
    content = content.replace('void MovementComponent::BackupPosition()
{
    old_position_ = owner_->position_vector();',
                              'void MovementComponent::BackupPosition()
{
    auto locked_owner = owner_.lock();
    if(!locked_owner) return;
    old_position_ = locked_owner->position_vector();')

    with open('SandyHeroesServer/MovementComponent.cpp', 'w', encoding='ansi') as f:
        f.write(content)

modify_server_movement()
