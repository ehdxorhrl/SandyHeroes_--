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

    # Fix Object* var = std::make_shared<Object>() -> auto var = ...
    content = re.sub(r'Object\*\s+([a-zA-Z0-9_]+)\s*=\s*std::make_shared<Object>\(', r'auto \1 = std::make_shared<Object>(', content)
    
    # Fix Component* var = std::make_shared<Component>() -> auto var = ...
    content = re.sub(r'([a-zA-Z0-9]+Component)\*\s+([a-zA-Z0-9_]+)\s*=\s*std::make_shared<\1>\(', r'auto \2 = std::make_shared<\1>(', content)

    # Wrap GetInstance() calls
    # Example: Object* player = model_infos_[0]->GetInstance();
    content = re.sub(r'Object\*\s+([a-zA-Z0-9_]+)\s*=\s*([a-zA-Z0-9_\[\].\->]+)->GetInstance\(\);', r'auto \1 = std::shared_ptr<Object>(\2->GetInstance());', content)
    # Generic GetInstance without Object* on left
    content = re.sub(r'([a-zA-Z0-9_]+)->GetInstance\(\)', r'std::shared_ptr<Object>(\1->GetInstance())', content)
    # Fix double wrap if it happened
    content = content.replace('std::shared_ptr<Object>(std::shared_ptr<Object>(', 'std::shared_ptr<Object>(')

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Fixed variable types in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
