import os
import re

def process_file(filepath, is_header):
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

    if is_header:
        content = re.sub(r'std::list<std::unique_ptr<Object>>\s+object_list_;', r'std::list<std::shared_ptr<Object>> object_list_;', content)
        content = re.sub(r'std::list<std::unique_ptr<Object>>\s+dead_object_list_;\s*', '', content)
        
        content = re.sub(r'virtual\s+void\s+AddObject\(Object\*\s+object\);', r'virtual void AddObject(std::shared_ptr<Object> object);', content)
        content = re.sub(r'virtual\s+void\s+DeleteObject\(Object\*\s+object\);', r'virtual void DeleteObject(std::shared_ptr<Object> object);', content)
        content = re.sub(r'void\s+DeleteObject\(Object\*\s+object\);', r'void DeleteObject(std::shared_ptr<Object> object);', content)
        content = re.sub(r'virtual\s+void\s+DeleteDeadObjects\(\);\s*', '', content)
        content = re.sub(r'void\s+DeleteDeadObjects\(\);\s*', '', content)
        
        content = re.sub(r'std::array<std::list<WallColliderComponent\*>\s*,\s*8>\s*stage_wall_collider_list_;', r'std::array<std::list<std::weak_ptr<WallColliderComponent>>, 8> stage_wall_collider_list_;', content)
        content = re.sub(r'std::list<Object\*>\s+ground_check_object_list_;', r'std::list<std::weak_ptr<Object>> ground_check_object_list_;', content)
        content = re.sub(r'Object\*\s+player_\s*=\s*nullptr;', r'std::weak_ptr<Object> player_;', content)
        content = re.sub(r'std::list<WallColliderComponent\*>\s+stage_wall_collider_list\(int\s+index\)\s+const\s*\{\s*return\s+stage_wall_collider_list_\[index\];\s*\}', r'std::list<std::weak_ptr<WallColliderComponent>> stage_wall_collider_list(int index) const { return stage_wall_collider_list_[index]; }', content)
        
        content = re.sub(r'std::array<std::list<MeshColliderComponent\*>\s*,\s*8>\s*checking_maps_mesh_collider_list_;', r'std::array<std::list<std::weak_ptr<MeshColliderComponent>>, 8> checking_maps_mesh_collider_list_;', content)
        content = re.sub(r'virtual\s+Object\*\s+CreatePlayerObject\(long\s+long\s+session_id\);', r'virtual std::shared_ptr<Object> CreatePlayerObject(long long session_id);', content)

    else:
        # Update AddObject
        content = re.sub(r'void\s+Scene::AddObject\(Object\*\s+object\)\s*\{\s*object_list_\.emplace_back\(\);\s*object_list_\.back\(\)\.reset\(object\);\s*\}', r'void Scene::AddObject(std::shared_ptr<Object> object)\n{\n\tobject_list_.push_back(object);\n}', content)
        content = re.sub(r'void\s+Scene::AddObject\(Object\*\s+object\)\s*\{\s*object_list_\.emplace_back\(object\);\s*\}', r'void Scene::AddObject(std::shared_ptr<Object> object)\n{\n\tobject_list_.push_back(object);\n}', content)
        
        # Update DeleteDeadObjects
        content = re.sub(r'void\s+Scene::DeleteDeadObjects\(\)\s*\{.*?\}(?=\n\n(?:void|Object\*|ModelInfo\*|XMVECTOR|Mesh\*|Texture\*|Material\*|const|using|std::shared_ptr<Object>|std::string))', '', content, flags=re.DOTALL)
        
        # Update object loop
        content = re.sub(r'const\s+std::unique_ptr<Object>&\s*object\s*:\s*object_list_', r'const std::shared_ptr<Object>& object : object_list_', content)
        
        # Update DeleteObject
        content = re.sub(r'void\s+Scene::DeleteObject\(Object\*\s+object\)\s*\{.*?\}(?=\n\n(?:void|Object\*|ModelInfo\*|XMVECTOR|Mesh\*|Texture\*|Material\*|const|using))', r'void Scene::DeleteObject(std::shared_ptr<Object> object)\n{\n\tfor (auto& sector : sectors_)\n\t{\n\t\tsector.DeleteObject(object.get());\n\t}\n\n\tobject_list_.remove(object);\n}', content, flags=re.DOTALL)

        # Update player() const
        content = re.sub(r'Object\*\s+Scene::player\(\)\s*const\s*\{\s*return\s*player_;\s*\}', r'Object* Scene::player() const\n{\n\treturn player_.lock().get();\n}', content)

        # Update BuildScene
        content = re.sub(r'object_list_\.emplace_back\(\);\s*object_list_\.back\(\)\.reset\(FindModelInfo\(load_token\)->GetInstance\(\)\);', r'object_list_.push_back(std::shared_ptr<Object>(FindModelInfo(load_token)->GetInstance()));', content)
        content = re.sub(r'object_list_\.emplace_back\(\);\s*object_list_\.back\(\)\.reset\(model_infos_\.back\(\)->GetInstance\(\)\);', r'object_list_.push_back(std::shared_ptr<Object>(model_infos_.back()->GetInstance()));', content)

        # Update Server CreatePlayerObject
        content = re.sub(r'Object\*\s+Scene::CreatePlayerObject\(long\s+long\s+session_id\)\s*\{\s*auto\s*model_info\s*=\s*FindModelInfo\("Dog00"\);\s*if\s*\(!model_info\)\s*return\s*nullptr;\s*auto\s*obj\s*=\s*model_info->GetInstance\(\);\s*obj->set_name\("Player_"\s*\+\s*std::to_string\(session_id\)\);\s*obj->set_position_vector\(0,\s*0,\s*0\);\s*object_list_\.emplace_back\(obj\);\s*return\s*obj;\s*\}', r'std::shared_ptr<Object> Scene::CreatePlayerObject(long long session_id) {\n\tauto model_info = FindModelInfo("Dog00");\n\tif (!model_info) return nullptr;\n\n\tauto obj = std::shared_ptr<Object>(model_info->GetInstance());\n\tobj->set_name("Player_" + std::to_string(session_id));\n\tobj->set_position_vector(0, 0, 0);\n\n\tobject_list_.push_back(obj);\n\treturn obj;\n}', content)

        # Update FindObject lambda
        content = re.sub(r'\[&object_name\]\(const\s+std::unique_ptr<Object>&\s*object\)', r'[&object_name](const std::shared_ptr<Object>& object)', content)
        content = re.sub(r'\[&id\]\(const\s+std::unique_ptr<Object>&\s*object\)', r'[&id](const std::shared_ptr<Object>& object)', content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/Scene.h", True)
process_file("SandyHeroesClient/Scene.cpp", False)
process_file("SandyHeroesServer/Scene.h", True)
process_file("SandyHeroesServer/Scene.cpp", False)
