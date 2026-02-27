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

    # Update GetComponent
    content = re.sub(r'static\s+T\*\s+GetComponent\(Object\*\s+object\)\s*\{\s*for\s*\(auto&\s+component\s*:\s*object->component_list_\)\s*\{\s*if\s*\(dynamic_cast<T\*>\(component\.get\(\)\)\)\s*return\s*static_cast<T\*>\(component\.get\(\)\);\s*\}\s*return\s*nullptr;\s*\}', 
                     r'''static std::shared_ptr<T> GetComponent(Object* object)
	{
		for (auto& component : object->component_list_)
		{
			if (std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(component))
				return res;
		}
		return nullptr;
	}''', content, flags=re.DOTALL)

    # Update GetComponents
    content = re.sub(r'static\s+std::list<T\*>\s+GetComponents\(Object\*\s+object\)\s*\{.*?\}', 
                     r'''static std::list<std::shared_ptr<T>> GetComponents(Object* object)
	{
		std::list<std::shared_ptr<T>> r_value;
		for (auto& component : object->component_list_)
		{
			if (std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(component))
			{
				r_value.push_back(res);
			}
		}
		return r_value;
	}''', content, flags=re.DOTALL)

    # Update GetComponentInChildren
    content = re.sub(r'static\s+T\*\s+GetComponentInChildren\(Object\*\s+object\)\s*\{.*?\}', 
                     r'''static std::shared_ptr<T> GetComponentInChildren(Object* object)
	{
		std::shared_ptr<T> component = GetComponent<T>(object);
		if (component)
			return component;

		if (object->sibling_)
		{
			component = GetComponentInChildren<T>(object->sibling_.get());
			if (component)
				return component;
		}
		if (object->child_)
			return GetComponentInChildren<T>(object->child_.get());

		return nullptr;
	}''', content, flags=re.DOTALL)

    # Update GetComponentsInChildren
    content = re.sub(r'static\s+std::list<T\*>\s+GetComponentsInChildren\(Object\*\s+object\)\s*\{.*?\}', 
                     r'''static std::list<std::shared_ptr<T>> GetComponentsInChildren(Object* object)
	{
		std::list<std::shared_ptr<T>> component_list = GetComponents<T>(object);
		std::list<std::shared_ptr<T>> r_value;

		for (auto& component : component_list)
		{
			r_value.push_back(component);
		}

		if (object->sibling_)
		{
			auto sibling_components = GetComponentsInChildren<T>(object->sibling_.get());
			for (auto& component : sibling_components)
			{
				r_value.push_back(component);
			}
		}
		if (object->child_)
		{
			auto child_components = GetComponentsInChildren<T>(object->child_.get());
			for (auto& component : child_components)
			{
				r_value.push_back(component);
			}
		}

		return r_value;
	}''', content, flags=re.DOTALL)

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/Object.h")
process_file("SandyHeroesServer/Object.h")
