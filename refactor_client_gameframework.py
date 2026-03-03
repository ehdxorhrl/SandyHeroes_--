import re

def refactor_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # send_mouse_click_packet, send_mouse_unclick_packet, send_mouse_move_packet
    pattern = r'Object\*\s+player\s*=\s*base_scene->player\(\);\s*// BaseScene에 player_ 멤버가 존재함\s*if \(!player\) return;\s*FPSControllerComponent\*\s+controller\s*=\s*Object::GetComponent<FPSControllerComponent>\(player\);\s*if \(!controller\) return;\s*Object\*\s+camera\s*=\s*controller->camera_object\(\);\s*if \(!camera\) return;\s*GunComponent\*\s+gun\s*=\s*Object::GetComponentInChildren<GunComponent>\(scene_->player\(\)\);'
    replacement = r'''auto player = base_scene->player();
    if (!player) return;
    auto player_ptr = player->shared_from_this();

    auto controller = Object::GetComponent<FPSControllerComponent>(player_ptr);
    if (!controller) return;

    auto camera = controller->camera_object().lock();
    if (!camera) return;

    auto gun = Object::GetComponentInChildren<GunComponent>(player_ptr);'''
    content = re.sub(pattern, replacement, content, flags=re.MULTILINE)

    # FindObject(packet->id) -> auto
    content = re.sub(r'Object\*\s+player\s*=\s*base_scene->FindObject\(packet->id\);', r'auto player = base_scene->FindObject(packet->id);', content)
    content = re.sub(r'Object\*\s+monster\s*=\s*base_scene->FindObject\(packet->id\);', r'auto monster = base_scene->FindObject(packet->id);', content)

    # GetComponents -> auto
    content = re.sub(r'PlayerComponent\*\s+player_component\s*=\s*Object::GetComponentInChildren<PlayerComponent>\(player\);', r'auto player_component = Object::GetComponentInChildren<PlayerComponent>(player);', content)
    content = re.sub(r'GunComponent\*\s+gun\s*=\s*Object::GetComponentInChildren<GunComponent>\(player\);', r'auto gun = Object::GetComponentInChildren<GunComponent>(player);', content)
    content = re.sub(r'MonsterComponent\*\s+monster_component\s*=\s*Object::GetComponentInChildren<MonsterComponent>\(monster\);', r'auto monster_component = Object::GetComponentInChildren<MonsterComponent>(monster);', content)
    
    # PLAY_RELOAD_SOUND
    pattern_reload = r'Object\*\s+player\s*=\s*base_scene->player\(\);\s*if \(!player\) break;\s*GunComponent\*\s+gun\s*=\s*Object::GetComponentInChildren<GunComponent>\(player\);'
    replace_reload = r'''auto player = base_scene->player();
        if (!player) break;

        auto gun = Object::GetComponentInChildren<GunComponent>(player->shared_from_this());'''
    content = re.sub(pattern_reload, replace_reload, content, flags=re.MULTILINE)

    # SHOTDRAGON_ATTACK
    pattern_movement = r'MovementComponent\*\s+movement\s*=\s*new\s+MovementComponent\(thorn_projectile\);\s*thorn_projectile->AddComponent\(movement\);'
    replace_movement = r'''auto movement = std::make_shared<MovementComponent>(thorn_projectile.get());
            thorn_projectile->AddComponent(movement);'''
    content = re.sub(pattern_movement, replace_movement, content, flags=re.MULTILINE)

    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)

refactor_file("SandyHeroesClient/GameFramework.cpp")