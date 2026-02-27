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

    # Fix triple parenthesis
    content = content.replace('GetInstance()));)', 'GetInstance()))')
    content = content.replace('GetInstance())));', 'GetInstance()));')
    
    # Fix duplicated AddObject/DeleteObject if any
    # (Checking SandyHeroesClient/BaseScene.cpp output from previous read_file)
    # I saw duplicated blocks in the output. I'll use a regex to find and fix them.
    # Actually, I'll just look for specific patterns.
    
    # Remove the broken duplicate block in SandyHeroesClient/BaseScene.cpp
    duplicate_block = '''}

	if (collide_type.wall_check)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		if(movement)
			wall_check_object_list_.emplace_back(object, movement);
	}

	auto monster_component = Object::GetComponent<MonsterComponent>(object);
	if (monster_component)
	{
		monster_list_.push_back(monster_component);
	}

	auto razer_component = Object::GetComponent<RazerComponent>(object);
	if (razer_component)
	{
		razer_list_.push_back(razer_component);
	}

}'''
    content = content.replace(duplicate_block, '}')

    # Fix duplicated DeleteObject block
    duplicate_delete = '''}
	if (collide_type.wall_check)
	{
		wall_check_object_list_.remove_if([object](const WallCheckObject& wall_check_object) {
			return wall_check_object.object == object;
			});
	}

	Scene::DeleteObject(object);

}'''
    content = content.replace(duplicate_delete, '}')

    # Fix the weird DeleteDeadObjects remnant
    remnant = ''');
	wall_check_object_list_.remove_if([](const WallCheckObject& wall_check_object) {
		return wall_check_object.false;
		});
	monster_list_.remove_if([](const MonsterComponent* monster_component) {
		return monster_component->owner()->is_dead();
		});
	razer_list_.remove_if([](const RazerComponent* razer_component) {
		return razer_component->owner()->is_dead();
		});
	Scene::DeleteDeadObjects();
}'''
    content = content.replace(remnant, '')

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Cleaned up {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
