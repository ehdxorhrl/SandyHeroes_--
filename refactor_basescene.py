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
        # Signatures
        content = content.replace('virtual void AddObject(Object* object) override;', 'virtual void AddObject(std::shared_ptr<Object> object) override;')
        content = content.replace('virtual void DeleteObject(Object* object) override;', 'virtual void DeleteObject(std::shared_ptr<Object> object) override;')
        content = content.replace('void DeleteObject(Object* object);', 'void DeleteObject(std::shared_ptr<Object> object);')
        content = re.sub(r'virtual\s+void\s+DeleteDeadObjects\(\)\s+override;', '', content)
        content = re.sub(r'void\s+DeleteDeadObjects\(\);', '', content)

        # WallCheckObject
        content = re.sub(r'struct WallCheckObject\s*\{.*?\};', 
                         r'''struct WallCheckObject
	{
		std::weak_ptr<Object> object;
		std::weak_ptr<MovementComponent> movement;

		WallCheckObject(std::shared_ptr<Object> obj, std::shared_ptr<MovementComponent> move)
			: object(obj), movement(move) {
		}
	};''', content, flags=re.DOTALL)

        # Lists to weak_ptr (Client/Server)
        content = re.sub(r'std::list<SpawnerComponent\*>', r'std::list<std::weak_ptr<SpawnerComponent>>', content)
        content = re.sub(r'std::list<RazerComponent\*>', r'std::list<std::weak_ptr<RazerComponent>>', content)
        content = re.sub(r'std::list<GroundColliderComponent\*>', r'std::list<std::weak_ptr<GroundColliderComponent>>', content)
        content = re.sub(r'std::list<WallColliderComponent\*>', r'std::list<std::weak_ptr<WallColliderComponent>>', content)
        content = re.sub(r'std::list<MeshComponent\*>', r'std::list<std::weak_ptr<MeshComponent>>', content)
        content = re.sub(r'std::list<MonsterComponent\*>', r'std::list<std::weak_ptr<MonsterComponent>>', content)
        content = re.sub(r'std::vector<Object\*>', r'std::vector<std::weak_ptr<Object>>', content)
        content = re.sub(r'std::vector<BoxColliderComponent\*>', r'std::vector<std::weak_ptr<BoxColliderComponent>>', content)
        content = re.sub(r'std::list<MeshColliderComponent\*>', r'std::list<std::weak_ptr<MeshColliderComponent>>', content)

        # Getter update
        content = re.sub(r'std::list<MonsterComponent\*>\s+monster_list\(\)\s+const;', r'std::list<std::weak_ptr<MonsterComponent>> monster_list() const;', content)
        content = re.sub(r'const\s+std::list<MonsterComponent\*>\&\s+monster_list\(\)\s+const;', r'const std::list<std::weak_ptr<MonsterComponent>>& monster_list() const;', content)

    else:
        # Implementation
        # AddObject
        content = re.sub(r'void\s+BaseScene::AddObject\(Object\*\s+object\)\s*\{(.*?)\}', 
                         r'''void BaseScene::AddObject(std::shared_ptr<Object> object)
{
	Scene::AddObject(object);

	CollideType collide_type = object->collide_type();

	if (collide_type.ground_check)
	{
		ground_check_object_list_.push_back(object);
	}
	if (collide_type.wall_check)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object.get());
		if (movement) {
			wall_check_object_list_.emplace_back(object, movement);
		}
	}

	auto monster_component = Object::GetComponent<MonsterComponent>(object.get());
	if (monster_component)
	{
		monster_list_.push_back(monster_component);
	}

	auto razer_component = Object::GetComponent<RazerComponent>(object.get());
	if (razer_component)
	{
		razer_list_.push_back(razer_component);
	}

	if(object->is_movable()) {
		object->OnDestroy([this](Object* o) {
            // ... (keep existing broadcast logic if possible, or skip for now)
		});
	}
}''', content, flags=re.DOTALL)

        # DeleteDeadObjects implementation removal
        content = re.sub(r'void\s+BaseScene::DeleteDeadObjects\(\)\s*\{.*?\}', '', content, flags=re.DOTALL)

        # DeleteObject
        content = re.sub(r'void\s+BaseScene::DeleteObject\(Object\*\s+object\)\s*\{.*?\}', 
                         r'''void BaseScene::DeleteObject(std::shared_ptr<Object> object)
{
	CollideType collide_type = object->collide_type();
	if (collide_type.ground_check)
	{
		ground_check_object_list_.remove_if([&object](const std::weak_ptr<Object>& wp) {
            return wp.expired() || wp.lock() == object;
        });
	}
	if (collide_type.wall_check)
	{
		wall_check_object_list_.remove_if([&object](const WallCheckObject& wall_check_object) {
			return wall_check_object.object.expired() || wall_check_object.object.lock() == object;
			});
	}

	Scene::DeleteObject(object);
}''', content, flags=re.DOTALL)

        # Update monster_list() return type
        content = re.sub(r'std::list<MonsterComponent\*>\s+BaseScene::monster_list\(\)\s+const\s*\{.*?\}', 
                         r'std::list<std::weak_ptr<MonsterComponent>> BaseScene::monster_list() const
{
	return monster_list_;
}', content, flags=re.DOTALL)
        content = re.sub(r'const\s+std::list<MonsterComponent\*>\&\s+BaseScene::monster_list\(\)\s+const\s*\{.*?\}', 
                         r'const std::list<std::weak_ptr<MonsterComponent>>& BaseScene::monster_list() const
{
	return monster_list_;
}', content, flags=re.DOTALL)

        # Update loops that use unique_ptr (wait, BaseScene doesn't use unique_ptr for objects usually, but it might for other things)
        # We need to update loops over the weak_ptr lists.
        # This is the hardest part. I'll focus on the most common ones.
        
        # Example: for (auto& object : ground_check_object_list_)
        content = re.sub(r'for\s*\(auto&\s+object\s*:\s*ground_check_object_list_\)\s*\{', 
                         r'for (auto it = ground_check_object_list_.begin(); it != ground_check_object_list_.end(); ) {
	auto object = it->lock();
	if (!object) { it = ground_check_object_list_.erase(it); continue; }
	++it;
	{', content)

        # Example: for (auto& monster : monster_list_)
        content = re.sub(r'for\s*\(auto&\s+monster\s*:\s*monster_list_\)\s*\{', 
                         r'for (auto it = monster_list_.begin(); it != monster_list_.end(); ) {
	auto monster = it->lock();
	if (!monster) { it = monster_list_.erase(it); continue; }
	++it;
	{', content)

        # Update new Object() to std::make_shared<Object>()
        content = re.sub(r'new\s+Object\(\)', r'std::make_shared<Object>()', content)
        content = re.sub(r'new\s+Object\("([^"]*)"\)', r'std::make_shared<Object>("\1")', content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/BaseScene.h", True)
process_file("SandyHeroesClient/BaseScene.cpp", False)
process_file("SandyHeroesServer/BaseScene.h", True)
process_file("SandyHeroesServer/BaseScene.cpp", False)
