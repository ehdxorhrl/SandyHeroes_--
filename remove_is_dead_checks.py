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

    # Remove is_dead() checks in if conditions
    content = content.replace('|| object->is_dead()', '')
    content = content.replace('|| other->is_dead()', '')
    content = content.replace('&& !object->is_dead()', '')
    content = content.replace('&& !other->is_dead()', '')
    content = content.replace('!key_objects[idx]->is_dead()', 'key_objects[idx]')
    content = content.replace('key_objects[i]->is_dead()', 'false') # Or skip
    content = content.replace('object->is_dead()', 'false')
    
    # Clean up resulting if(!object || ) etc
    content = content.replace('if (!object || )', 'if (!object)')
    content = content.replace('if (!other || )', 'if (!other)')

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Removed is_dead checks in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
