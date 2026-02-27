import os
import re

def process_file(filepath):
    if not os.path.exists(filepath):
        return
        
    encodings_to_try = ['utf-8', 'cp949', 'euc-kr', 'latin-1']
    content = None
    used_encoding = None
    for enc in encodings_to_try:
        try:
            with open(filepath, 'r', encoding=enc) as f:
                content = f.read()
            used_encoding = enc
            break
        except UnicodeDecodeError:
            continue
            
    if content is None:
        print(f"Failed to read {filepath}")
        return

    # Helper function to refactor loop
    def refactor_simple_loop(match):
        var_name = match.group(1)
        list_name = match.group(2)
        res = 'for (auto it = ' + list_name + '.begin(); it != ' + list_name + '.end(); ) {\n\t'
        res += 'auto ' + var_name + ' = it->lock();\n\t'
        res += 'if (!' + var_name + ') { it = ' + list_name + '.erase(it); continue; }\n\t'
        res += '++it;'
        return res

    # for (auto& object : ground_check_object_list_)
    # Added stage_monster_spawner_list_
    content = re.sub(r'for\s*\(auto&\s+([a-zA-Z0-9_]+)\s*:\s*(ground_check_object_list_|monster_list_|razer_list_|dropped_guns_|chests_|sounds_|spawn_boxs_|monster_hit_particles_|stage_ground_collider_list_\[[^\]]+\]|stage_wall_collider_list_\[[^\]]+\]|checking_maps_mesh_collider_list_\[[^\]]+\]|stage_monster_spawner_list_\[[^\]]+\])\)\s*\{',
                     refactor_simple_loop, content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed loops in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
