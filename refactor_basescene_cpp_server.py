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

    # 1. AddObject signature and body
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
			sc_packet_object_set_dead osd{};
		osd.size = sizeof(sc_packet_object_set_dead);
		osd.type = S2C_P_OBJECT_SET_DEAD;   // ㅼ ъ⑹ 以묒 濡 援댁
		osd.id = o->id();
		osd.monster_type = o->monster_type();

		const auto& users = SessionManager::getInstance().getAllSessions();
		for (auto& u : users) u.second->do_send(&osd);   // 釉뚮濡ㅽ
			});
	}
}''', content, flags=re.DOTALL)

    # 2. DeleteObject signature
    content = re.sub(r'void\s+BaseScene::DeleteObject\(Object\*\s+object\)\s*\{(.*?)\}', 
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

    # 3. Remove DeleteDeadObjects
    content = re.sub(r'void\s+BaseScene::DeleteDeadObjects\(\)\s*\{.*?\}', '', content, flags=re.DOTALL)

    # 4. monster_list() getter
    content = re.sub(r'const\s+std::list<MonsterComponent\*>\&\s+BaseScene::monster_list\(\)\s+const\s*\{.*?\}', 
                     'std::list<std::weak_ptr<MonsterComponent>> BaseScene::monster_list() const\n{\n\treturn monster_list_;\n}', content, flags=re.DOTALL)

    # 5. new Object -> std::make_shared<Object>
    content = re.sub(r'new\s+Object\(\)', r'std::make_shared<Object>()', content)
    content = re.sub(r'new\s+Object\("([^"]*)"\)', r'std::make_shared<Object>("\1")', content)

    # 6. AddComponent calls
    content = re.sub(r'AddComponent\(new\s+([a-zA-Z0-9]+Component)\(([^)]*)\)\)', r'AddComponent(std::make_shared<\1>(\2))', content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

process_file("SandyHeroesServer/BaseScene.cpp")
