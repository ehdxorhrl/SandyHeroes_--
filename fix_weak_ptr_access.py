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

    # Fix monster_hit_particles_.front() access
    content = content.replace('monster_hit_particles_.front()', 'monster_hit_particles_.front().lock().get()')
    
    # Fix sounds_.push_back(sound_object) where sound_object is shared_ptr
    # content = content.replace('sounds_.push_back(sound_object)', 'sounds_.push_back(sound_object)') # Already fine
    
    # Fix chests_[i] access
    # content = content.replace('chests_[i]', 'chests_[i].lock().get()') # Dangerous, need regex
    content = re.sub(r'chests_\[([^\]]+)\](?!\.lock)', r'chests_[\1].lock().get()', content)
    content = re.sub(r'dropped_guns_\[([^\]]+)\](?!\.lock)', r'dropped_guns_[\1].lock().get()', content)
    content = re.sub(r'spawn_boxs_\[([^\]]+)\](?!\.lock)', r'spawn_boxs_[\1].lock().get()', content)
    content = re.sub(r'sounds_\[([^\]]+)\](?!\.lock)', r'sounds_[\1].lock().get()', content)

    # Fix player_ in BaseScene (it might be weak_ptr now if I moved it from Scene to BaseScene? No, Scene has it)
    # Actually Scene::player_ is weak_ptr.
    # In BaseScene.cpp, player_ is often used.
    # But wait, BaseScene doesn't have its own player_, it uses Scene::player_?
    # No, BaseScene had 'Object* player_ = nullptr;' in its protected section in the initial snippet.
    # If I changed it to weak_ptr, I need to lock it.
    
    # Actually I should check if player_ is used directly.
    content = re.sub(r'(?<!it->)(?<![a-zA-Z0-9_])player_(?!\.lock)(?!\.expired)(?![a-zA-Z0-9_])', 'player_.lock()', content)
    # But player_.lock() returns a shared_ptr, so player_->... works.
    # However, if it was Object*, player_->... still works.
    # But if we need the raw pointer for some functions: player_.lock().get()
    
    # Fix double lock if it happened
    content = content.replace('player_.lock().lock()', 'player_.lock()')

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Fixed weak_ptr access in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
