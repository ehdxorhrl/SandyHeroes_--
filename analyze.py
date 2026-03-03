import os
import re
import glob

def guess_role(cls_name, base_class):
    if "Scene" in cls_name:
        return "Manages a specific state of the game, including objects, UI, and rendering resources within that state."
    elif "Component" in cls_name:
        return "A reusable module attached to an Object to define specific behaviors or data (ECS architecture)."
    elif "Shader" in cls_name:
        return "Handles the GPU pipeline setup, constant buffers, and rendering logic for a specific visual effect."
    elif "State" in cls_name:
        return "Defines a specific state within a Finite State Machine (e.g., animation or AI behavior)."
    elif "Manager" in cls_name:
        return "A singleton or central system responsible for managing global resources or systems."
    elif cls_name == "Object":
        return "The base entity in the game world. Contains a transform and a list of Components."
    elif base_class == "Mesh":
        return "Contains vertex, index, and potentially bone data for rendering 3D geometry."
    else:
        return f"Core data structure or utility class for {cls_name.lower().replace('info', ' information').replace('data', ' data')}."

def analyze_file(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    filename = os.path.basename(filepath)
    
    # 1. Classes / Structs
    # class ClassName : public BaseClass
    class_pattern = re.compile(r'(?:class|struct)\s+([A-Za-z0-9_]+)(?:\s*:\s*(?:public|protected|private)?\s*(?:virtual)?\s*([A-Za-z0-9_]+))?')
    classes = class_pattern.findall(content)
    if not classes:
        return ""
        
    output = f"## File: {filename}\n\n"
        
    for cls in classes:
        cls_name = cls[0]
        base_class = cls[1]
        if cls_name in ["EXP_OVER", "CBPass", "CBObject", "CBMaterial", "CBUi", "InstanceData", "LightInfo", "CBBoneTransform"]:
            continue # Skip common structs to save space
            
        ctype = "Struct" if "struct" in content.split(cls_name)[0][-15:] else "Class"
        
        output += f"### {ctype}: {cls_name}\n"
        if base_class:
            output += f"- **Inherits from:** {base_class}\n"
            
        role = guess_role(cls_name, base_class)
        output += f"- **Architectural Role:** {role}\n"
        
        # Isolate class content heuristically
        cls_content = ""
        try:
            cls_content = content.split(f"{ctype.lower()} {cls_name}")[1]
            # simplistic bracket matching to find the end of class
            bracket_count = 0
            end_idx = -1
            started = False
            for i, char in enumerate(cls_content):
                if char == '{':
                    bracket_count += 1
                    started = True
                elif char == '}':
                    bracket_count -= 1
                if started and bracket_count == 0:
                    end_idx = i
                    break
            if end_idx != -1:
                cls_content = cls_content[:end_idx+2]
        except:
            cls_content = content # fallback
            
        # 3. Memory Management Evaluation
        c_shared = re.findall(r'std::shared_ptr<([A-Za-z0-9_:]+)>', cls_content)
        c_weak = re.findall(r'std::weak_ptr<([A-Za-z0-9_:]+)>', cls_content)
        c_unique = re.findall(r'std::unique_ptr<([A-Za-z0-9_:]+)>', cls_content)
        raw_ptrs = re.findall(r'([A-Za-z0-9_:]+)\s*\*\s+[a-z0-9_]+_', cls_content)
        
        output += "- **Memory Management Analysis:**\n"
        if c_shared:
            c_shared_unique = list(set(c_shared))
            output += f"  - shared_ptr: Uses shared ownership for {', '.join(c_shared_unique)}.\n"
        if c_weak:
            c_weak_unique = list(set(c_weak))
            output += f"  - weak_ptr: Safely references {', '.join(c_weak_unique)} without affecting lifecycle.\n"
        if c_unique:
            c_unique_unique = list(set(c_unique))
            output += f"  - unique_ptr: Exclusively owns {', '.join(c_unique_unique)}.\n"
        
        # Contextual Evaluation based on refactoring rules
        eval_notes = []
        if "Scene" in cls_name and base_class == "Scene":
            if "Object" in c_shared and "Object" not in c_weak:
                pass
            eval_notes.append("  - *Refactoring Check:* Scene lists (except object_list_) should be weak_ptr to avoid dangling pointers during object deletion.")
        elif "Component" in cls_name:
            if "Object" in c_weak:
                eval_notes.append("  - *Refactoring Check:* owner_ or related Object references are correctly managed with weak_ptr.")
            elif "Object" in c_shared:
                eval_notes.append("  - *Refactoring Warning:* Contains shared_ptr<Object>. Ensure this does not create a circular reference with the owner.")
        elif cls_name == "Object":
            eval_notes.append("  - *Refactoring Check:* is_dead_ member should be removed. Deletion is handled by Scene::DeleteObject and weak_ptr invalidation.")
            
        if not c_shared and not c_weak and not c_unique:
            eval_notes.append("  - Primarily uses raw pointers, value types, or external handles (e.g., ComPtr for D3D12).")
            
        for note in eval_notes:
            output += f"{note}\n"
            
        output += "\n"
        
    return output

def main():
    for target, name in [('SandyHeroesClient', 'ResearchClient.md'), ('SandyHeroesServer', 'ResearchServer.md')]:
        print(f"Analyzing {target}...")
        files = glob.glob(f'D:/SandyHeroesClient/SandyHeroes_--/{target}/*.h')
        files.sort()
        
        with open(f'D:/SandyHeroesClient/SandyHeroes_--/{name}', 'w', encoding='utf-8') as f:
            f.write(f"# {target} - Deep Architectural & Memory Analysis\n\n")
            f.write("> **Overview:** This document provides a deeper analysis of class architectures and evaluates memory management patterns against the project's refactoring objectives (shared_ptr, weak_ptr enforcement and circular reference prevention).\n\n")
            
            for filepath in files:
                out = analyze_file(filepath)
                if out:
                    f.write(out)
                    f.write("---\n\n")

if __name__ == '__main__':
    main()
