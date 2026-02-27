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

    # Update Object creation
    # Object* player = model_infos_[0]->GetInstance(); 
    # GetInstance() should return Object* which we then need to wrap.
    # But wait, if object_list_.push_back(std::shared_ptr<Object>(obj)) works.
    
    # Update Component creation passed to AddComponent
    # Pattern: AddComponent(new XComponent(...))
    content = re.sub(r'AddComponent\(\s*new\s+([a-zA-Z0-9]+Component)\s*\((.*?)\)\s*\)', r'AddComponent(std::make_shared<\1>(\2))', content, flags=re.DOTALL)

    # Update cases where component is created first then added
    # auto comp = new XComponent(obj); ... obj->AddComponent(comp);
    # This is harder. But usually it's direct.
    
    # Handle specific cases in SandyHeroesClient/BaseScene.cpp
    # auto player_component = new PlayerComponent(player);
    # player_component->set_scene(this);
    # player->AddComponent(player_component);
    # ->
    # auto player_component = std::make_shared<PlayerComponent>(player);
    # player_component->set_scene(this);
    # player->AddComponent(player_component);
    content = re.sub(r'auto\s+([a-zA-Z0-9_]+)\s*=\s*new\s+([a-zA-Z0-9]+Component)\s*\((.*?)\);', r'auto \1 = std::make_shared<\2>(\3);', content)

    # Handle mesh_collider creation
    # MeshColliderComponent* mesh_collider = new MeshColliderComponent(object);
    # ...
    # object->AddComponent(mesh_collider);
    content = re.sub(r'([a-zA-Z0-9]+Component)\*\s+([a-zA-Z0-9_]+)\s*=\s*new\s+\1\s*\((.*?)\);', r'auto \2 = std::make_shared<\1>(\3);', content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Refactored component creation in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
