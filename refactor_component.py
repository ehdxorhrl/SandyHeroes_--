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
        if '#include <memory>' not in content:
            content = content.replace('#pragma once', '#pragma once\n#include <memory>')
        
        content = content.replace('Object* owner_ = nullptr;', 'std::weak_ptr<Object> owner_;')
        content = content.replace('Object* hierarchy_root_ = nullptr;', 'std::weak_ptr<Object> hierarchy_root_;')

    else:
        # Implementation
        # Using regex to find the constructor and update it
        content = re.sub(r'Component::Component\(Object\*\s+owner\)\s*:\s*owner_\(owner\)', r'Component::Component(Object* owner) : owner_(owner->shared_from_this())', content)
        content = re.sub(r'void\s+Component::set_owner\(Object\*\s+owner\)\s*\{\s*owner_\s*=\s*owner;\s*\}', r'void Component::set_owner(Object* owner)\n{\n\towner_ = owner->shared_from_this();\n}', content)
        content = re.sub(r'Object\*\s+Component::owner\(\)\s*const\s*\{\s*return\s*owner_;\s*\}', r'Object* Component::owner() const\n{\n\treturn owner_.lock().get();\n}', content)
        content = re.sub(r'Object\*\s+Component::hierarchy_root\(\)\s*\{.*?\}', r'Object* Component::hierarchy_root()\n{\n\tif (hierarchy_root_.expired())\n\t{\n\t\tauto locked_owner = owner_.lock();\n\t\tif(locked_owner) hierarchy_root_ = locked_owner->GetHierarchyRoot()->shared_from_this();\n\t}\n\treturn hierarchy_root_.lock().get();\n}', content, flags=re.DOTALL)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/Component.h", True)
process_file("SandyHeroesClient/Component.cpp", False)
process_file("SandyHeroesServer/Component.h", True)
process_file("SandyHeroesServer/Component.cpp", False)
