import os
import re

def parse_header(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    classes = []
    current_class = None
    access_specifier = 'private'
    
    for line in content.split('\\n' if '\\n' in content else '\n'):
        line = line.strip()
        
        class_match = re.match(r'class\s+([A-Za-z0-9_]+)(?:\s*:\s*(public|protected|private)\s+([A-Za-z0-9_<>:,\s]+))?', line)
        if class_match and not line.endswith(';'):
            if current_class:
                classes.append(current_class)
            current_class = {
                'name': class_match.group(1),
                'bases': class_match.group(3) if class_match.group(3) else 'None',
                'methods': [],
                'members': [],
            }
            access_specifier = 'private'
            continue
            
        if current_class:
            if line.startswith('public:'): access_specifier = 'public'
            elif line.startswith('protected:'): access_specifier = 'protected'
            elif line.startswith('private:'): access_specifier = 'private'
            
            func_match = re.match(r'^(?:virtual\s+|static\s+|inline\s+)*([A-Za-z0-9_<>:\*&\s]+)\s+([A-Za-z0-9_]+)\s*\((.*?)\)(?:\s*const)?(?:\s*override)?(?:\s*=\s*0|;|\s*\{)', line)
            var_match = re.match(r'^(?:mutable\s+|static\s+|const\s+)*([A-Za-z0-9_<>:\*&\s]+)\s+([A-Za-z0-9_]+)\s*(?:=.*?)?;', line)
            
            if func_match:
                ret_type, name, args = func_match.groups()
                if name not in ['return', 'delete', 'new', 'if', 'while', 'for']:
                    if name == current_class['name'] or name == '~' + current_class['name']:
                        current_class['methods'].append(f"{name}({args})")
                    else:
                        current_class['methods'].append(f"{ret_type.strip()} {name}({args})")
            elif var_match:
                v_type, name = var_match.groups()
                if not 'return' in v_type:
                    current_class['members'].append(f"{v_type.strip()} {name}")

    if current_class:
        classes.append(current_class)

    return classes

def generate_markdown(directory, title):
    md = f"# {title} - Class Analysis\n\n"
    
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.h'):
                file_path = os.path.join(root, file)
                classes = parse_header(file_path)
                
                if classes:
                    md += f"## File: `{file}`\n\n"
                    for cls in classes:
                        md += f"### Class: `{cls['name']}`\n"
                        if cls['bases'] != 'None':
                            md += f"**Inherits from:** `{cls['bases']}`\n\n"
                        
                        md += "**Methods:**\n"
                        if cls['methods']:
                            for m in cls['methods']:
                                md += f"- `{m}`\n"
                        else:
                            md += "- (No parsed methods)\n"
                            
                        md += "\n**Members:**\n"
                        if cls['members']:
                            for m in cls['members']:
                                md += f"- `{m}`\n"
                        else:
                            md += "- (No parsed members)\n"
                        md += "\n"
    return md

if __name__ == '__main__':
    client_dir = 'D:/SandyHeroesClient/SandyHeroes_--/SandyHeroesClient'
    server_dir = 'D:/SandyHeroesClient/SandyHeroes_--/SandyHeroesServer'
    
    client_md = generate_markdown(client_dir, "Sandy Heroes Client")
    server_md = generate_markdown(server_dir, "Sandy Heroes Server")
    
    with open('ResearchClient.md', 'w', encoding='utf-8') as f:
        f.write(client_md)
        
    with open('ResearchServer.md', 'w', encoding='utf-8') as f:
        f.write(server_md)
    
    print("Markdown generation complete.")