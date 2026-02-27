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

    # 1. Signatures
    content = content.replace('void AddObject(Object* object) override;', 'void AddObject(std::shared_ptr<Object> object) override;')
    content = content.replace('void DeleteObject(Object* object);', 'void DeleteObject(std::shared_ptr<Object> object);')
    content = re.sub(r'void\s+DeleteDeadObjects\(\);', '', content)

    # 2. WallCheckObject struct
    content = re.sub(r'struct WallCheckObject\s*\{.*?\};', 
                     r'''struct WallCheckObject
	{
		std::weak_ptr<Object> object;
		std::weak_ptr<MovementComponent> movement;

		WallCheckObject(std::shared_ptr<Object> obj, std::shared_ptr<MovementComponent> move)
			: object(obj), movement(move) {
		}
	};''', content, flags=re.DOTALL)

    # 3. Lists to weak_ptr
    content = re.sub(r'std::list<SpawnerComponent\*>', r'std::list<std::weak_ptr<SpawnerComponent>>', content)
    content = re.sub(r'std::list<MonsterComponent\*>', r'std::list<std::weak_ptr<MonsterComponent>>', content)
    content = re.sub(r'std::list<RazerComponent\*>', r'std::list<std::weak_ptr<RazerComponent>>', content)
    content = re.sub(r'std::list<GroundColliderComponent\*>', r'std::list<std::weak_ptr<GroundColliderComponent>>', content)
    content = re.sub(r'std::list<WallColliderComponent\*>', r'std::list<std::weak_ptr<WallColliderComponent>>', content)
    
    # Object pointers in vectors
    content = re.sub(r'std::vector<Object\*>\s+dropped_guns_;', r'std::vector<std::weak_ptr<Object>> dropped_guns_;', content)
    content = re.sub(r'std::vector<Object\*>\s+chests_;', r'std::vector<std::weak_ptr<Object>> chests_;', content)
    
    content = re.sub(r'std::vector<BoxColliderComponent\*>\s+spawn_boxs_', r'std::vector<std::weak_ptr<BoxColliderComponent>> spawn_boxs_', content)

    # 4. Getter
    content = re.sub(r'const\s+std::list<MonsterComponent\*>\&\s+monster_list\(\)\s+const;', r'std::list<std::weak_ptr<MonsterComponent>> monster_list() const;', content)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

process_file("SandyHeroesServer/BaseScene.h")
