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
        content = content.replace('Object* player_object_ = nullptr;', 'std::shared_ptr<Object> player_object_;')
        content = content.replace('Object* camera_object_ = nullptr;', 'std::shared_ptr<Object> camera_object_;')
        content = content.replace('Object* get_player_object() { return player_object_; }', 'std::shared_ptr<Object> get_player_object() { return player_object_; }')
        content = content.replace('void set_player_object(Object* object) { player_object_ = object; }', 'void set_player_object(std::shared_ptr<Object> object) { player_object_ = object; }')

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

process_file("SandyHeroesServer/User.h", True)
