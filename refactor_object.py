import os
import re

def process_file(filepath, is_header):
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
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
        content = content.replace('class Object\n{', 'class Object : public std::enable_shared_from_this<Object>\n{')
        content = content.replace('Object* parent_ = nullptr;', 'std::weak_ptr<Object> parent_;')
        content = content.replace('Object* child_ = nullptr;', 'std::shared_ptr<Object> child_;')
        content = content.replace('Object* sibling_ = nullptr;', 'std::shared_ptr<Object> sibling_;')
        content = content.replace('std::list<std::unique_ptr<Component>> component_list_;', 'std::list<std::shared_ptr<Component>> component_list_;')
        
        # Remove dead fields
        content = re.sub(r'\s*bool is_dead_ = false;.*?\n', '\n', content)
        content = re.sub(r'\s*UINT dead_frame_count_ = 0;.*?\n', '\n', content)
        
        # Remove methods
        content = re.sub(r'\s*bool is_dead\(\) const;.*?\n', '\n', content)
        content = re.sub(r'\s*void set_is_dead\(bool is_dead\);\n', '', content)
        content = re.sub(r'\s*UINT dead_frame_count\(\) const \{ return dead_frame_count_; \}.*?\n', '\n', content)
        content = re.sub(r'\s*void AddDeadFrameCount\(UINT frame_count\);\n', '', content)
        content = re.sub(r'\s*void KillChild\(const std::string& name\);\n', '', content)
        content = re.sub(r'\s*Object\* PopDeadChild\(\);.*?\n', '\n', content)
        
        content = content.replace('void AddChild(Object* object);', 'void AddChild(std::shared_ptr<Object> object);')
        content = content.replace('void AddSibling(Object* object);', 'void AddSibling(std::shared_ptr<Object> object);')
        content = content.replace('void ChangeChild(Object* src, const std::string& dst_name, bool is_delete = true);', 'void ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete = true);')
        content = content.replace('void ChangeChild(Object* src, const std::string& dst_name, bool is_delete);', 'void ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete);')
        content = content.replace('static Object* DeepCopy(Object* value, Object* parent = nullptr);', 'static std::shared_ptr<Object> DeepCopy(const std::shared_ptr<Object>& value, const std::shared_ptr<Object>& parent = nullptr);')
        content = content.replace('void AddComponent(Component* component);', 'void AddComponent(std::shared_ptr<Component> component);')

    else:
        # Destructor
        content = re.sub(r'Object::~Object\(\)\s*\{\s*if\s*\(sibling_\)\s*delete\s*sibling_;\s*if\s*\(child_\)\s*delete\s*child_;\s*\}', 'Object::~Object()\n{\n}', content)
        
        # Getters
        content = re.sub(r'Object\*\s+Object::child\(\)\s*const\s*\{\s*return\s*child_;\s*\}', 'Object* Object::child() const\n{\n\treturn child_.get();\n}', content)
        content = re.sub(r'Object\*\s+Object::sibling\(\)\s*const\s*\{\s*return\s*sibling_;\s*\}', 'Object* Object::sibling() const\n{\n\treturn sibling_.get();\n}', content)
        content = re.sub(r'Object\*\s+Object::parent\(\)\s*const\s*\{\s*return\s*parent_;\s*\}', 'Object* Object::parent() const\n{\n\treturn parent_.lock().get();\n}', content)
        
        # Method implementations removal
        content = re.sub(r'bool\s+Object::is_dead\(\)\s*const\s*\{\s*return\s*is_dead_;\s*\}\s*', '', content)
        content = re.sub(r'void\s+Object::set_is_dead\(bool\s+is_dead\)\s*\{\s*is_dead_\s*=\s*is_dead;\s*\}\s*', '', content)
        content = re.sub(r'void\s+Object::KillChild.*?\}\s*(?=Object\*)', '', content, flags=re.DOTALL)
        content = re.sub(r'Object\*\s+Object::PopDeadChild.*?\}\s*(?=(void|Object\*|XMFLOAT))', '', content, flags=re.DOTALL)
        content = re.sub(r'void\s+Object::AddDeadFrameCount\(UINT\s+frame_count\)\s*\{\s*dead_frame_count_\s*\+=\s*frame_count;\s*\}\s*', '', content)
        content = re.sub(r'if\s*\(\s*is_dead_\s*\)\s*return;', '', content)
        
        # AddComponent
        content = re.sub(r'void\s+Object::AddComponent\(Component\*\s+component\)\s*\{\s*component_list_\.emplace_back\(\);\s*component_list_\.back\(\)\.reset\(component\);\s*\}', 'void Object::AddComponent(std::shared_ptr<Component> component)\n{\n\tcomponent_list_.push_back(component);\n}', content)
        
        # Copy constructor component list
        content = re.sub(r'for\s*\(const\s*std::unique_ptr<Component>&\s*component\s*:\s*other\.component_list_\)\s*\{\s*component_list_\.emplace_back\(\);\s*component_list_\.back\(\)\.reset\(component->GetCopy\(\)\);\s*component_list_\.back\(\)->set_owner\(this\);\s*\}', 'for (const std::shared_ptr<Component>& component : other.component_list_)\n\t{\n\t\tcomponent_list_.push_back(std::shared_ptr<Component>(component->GetCopy()));\n\t\tcomponent_list_.back()->set_owner(this);\n\t}', content)
        
        # Update component loop
        content = content.replace('for (const std::unique_ptr<Component>& component : component_list_)', 'for (const std::shared_ptr<Component>& component : component_list_)')
        
        # AddChild
        content = re.sub(r'void\s+Object::AddChild\(Object\*\s+object\)\s*\{\s*object->parent_\s*=\s*this;\s*if\s*\(child_\)\s*child_->AddSibling\(object\);\s*else\s*child_\s*=\s*object;\s*\}', 'void Object::AddChild(std::shared_ptr<Object> object)\n{\n\tobject->parent_ = shared_from_this();\n\tif (child_)\n\t\tchild_->AddSibling(object);\n\telse\n\t\tchild_ = object;\n}', content)
        
        # AddSibling
        content = re.sub(r'void\s+Object::AddSibling\(Object\*\s+object\)\s*\{\s*object->parent_\s*=\s*parent_;\s*if\s*\(sibling_\)\s*sibling_->AddSibling\(object\);\s*else\s*sibling_\s*=\s*object;\s*\}', 'void Object::AddSibling(std::shared_ptr<Object> object)\n{\n\tobject->parent_ = parent_;\n\tif (sibling_)\n\t\tsibling_->AddSibling(object);\n\telse\n\t\tsibling_ = object;\n}', content)
        
        # GetHierarchyRoot
        content = re.sub(r'Object\*\s+Object::GetHierarchyRoot\(\)\s*\{\s*if\s*\(parent_\)\s*return\s*parent_->GetHierarchyRoot\(\);\s*return\s*this;\s*\}', 'Object* Object::GetHierarchyRoot()\n{\n\tif (!parent_.expired())\n\t\treturn parent_.lock()->GetHierarchyRoot();\n\treturn this;\n}', content)
        
        # DeepCopy
        content = re.sub(r'Object\*\s+Object::DeepCopy\(Object\*\s+value,\s*Object\*\s+parent\)\s*\{.*?\}\s*(?=$|void|Object\*)', 'std::shared_ptr<Object> Object::DeepCopy(const std::shared_ptr<Object>& value, const std::shared_ptr<Object>& parent)\n{\n\tif (!value)\n\t\treturn nullptr;\n\n\tstd::shared_ptr<Object> copy = std::make_shared<Object>(*value);\n\tcopy->parent_ = parent;\n\n\tcopy->child_ = DeepCopy(value->child_, copy);\n\tcopy->sibling_ = DeepCopy(value->sibling_, parent);\n\n\treturn copy;\n}\n', content, flags=re.DOTALL)

        # ChangeChild
        content = re.sub(r'void\s+Object::ChangeChild\(Object\*\s+src,\s*const\s+std::string&\s+dst_name,\s*bool\s+is_delete\)\s*\{.*?\}\s*(?=void|Object\*)', 'void Object::ChangeChild(std::shared_ptr<Object> src, const std::string& dst_name, bool is_delete)\n{\n\tif (!src)\n\t\treturn;\n\tif (child_ && child_->name() == dst_name)\n\t{\n\t\tsrc->parent_ = shared_from_this();\n\t\tsrc->sibling_ = child_->sibling_;\n\t\tchild_ = src;\n\t\treturn;\n\t}\n\tif (child_)\n\t{\n\t\tchild_->ChangeChild(src, dst_name, is_delete);\n\t}\n\tif (sibling_)\n\t{\n\t\tsibling_->ChangeChild(src, dst_name, is_delete);\n\t}\n}\n', content, flags=re.DOTALL)

        # DeleteChild
        content = re.sub(r'void\s+Object::DeleteChild\(const\s+std::string&\s+name\)\s*\{.*?\}\s*(?=void|Object\*)', 'void Object::DeleteChild(const std::string& name)\n{\n\tif (child_ && child_->name() == name)\n\t{\n\t\tchild_ = child_->sibling_;\n\t\treturn;\n\t}\n\tif (child_)\n\t{\n\t\tchild_->DeleteChild(name);\n\t}\n\tif (sibling_)\n\t{\n\t\tsibling_->DeleteChild(name);\n\t}\n}\n', content, flags=re.DOTALL)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/Object.h", True)
process_file("SandyHeroesClient/Object.cpp", False)
process_file("SandyHeroesServer/Object.h", True)
process_file("SandyHeroesServer/Object.cpp", False)
