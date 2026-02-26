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
        content = re.sub(r'std::list<Object\*>\s+object_list_;', r'std::list<std::weak_ptr<Object>> object_list_;', content)
        content = re.sub(r'bool\s+InsertObject\(Object\*\s+object\);', r'bool InsertObject(std::shared_ptr<Object> object);', content)
        content = re.sub(r'void\s+DeleteDeadObject\(\);\s*', '', content)
        content = re.sub(r'bool\s+CheckObjectInSectorObjectList\(Object\*\s+object\)\s*\{.*?\}', 
                         r'''bool CheckObjectInSectorObjectList(Object* object)
	{
		return std::find_if(object_list_.begin(), object_list_.end(), [object](const std::weak_ptr<Object>& wp) {
			auto locked = wp.lock();
			return locked && locked.get() == object;
		}) != object_list_.end();
	}''', 
                         content, flags=re.DOTALL)
        content = re.sub(r'std::list<Object\*>\&\s+object_list\(\)\s*\{\s*return\s*object_list_;\s*\}', 
                         r'std::list<std::weak_ptr<Object>>& object_list() { return object_list_; }', content)

    else:
        # Full replace of implementation to avoid regex messes
        new_content = re.sub(r'#include "Sector.h".*', '', content, flags=re.DOTALL)
        
        # Determine if client or server (server uses XMLoadFloat3(&pos) etc but the logic is same)
        # Actually I'll just write a generic one.
        
        implementation = '''#include "stdafx.h"
#include "Sector.h"
#include "Object.h"
#include "BoxColliderComponent.h"

Sector::Sector(const std::string& name, const BoundingBox& bounds)
	: name_(name), bounds_(bounds)
{

}

bool Sector::InsertObject(std::shared_ptr<Object> object)
{
	auto in_object = std::find_if(object_list_.begin(), object_list_.end(), [&object](const std::weak_ptr<Object>& wp) {
		return wp.lock() == object;
	});

	if (in_object != object_list_.end())
	{
		return true;
	}

	auto box_collider = Object::GetComponentInChildren<BoxColliderComponent>(object.get());
	if (box_collider)
	{
		box_collider->Update(0.f);
		if (bounds_.Contains(box_collider->animated_box()) != ContainmentType::DISJOINT)
		{
			object_list_.push_back(object);
			return true;
		}
	}
	
	XMFLOAT3 pos = object->position_vector();
	if (bounds_.Contains(XMLoadFloat3(&pos)))
	{
		object_list_.push_back(object);
		return true;
	}
	return false;
}

void Sector::DeleteOutOfBoundsObjects()
{
	object_list_.remove_if([this](const std::weak_ptr<Object>& wp) {
		auto object = wp.lock();
		if (!object)
			return true;
		if (!object->is_movable())
			return false;
		
		XMFLOAT3 pos = object->position_vector();
		return bounds_.Contains(XMLoadFloat3(&pos)) == ContainmentType::DISJOINT;
	});
}

void Sector::DeleteObject(Object* object)
{
	object_list_.remove_if([object](const std::weak_ptr<Object>& wp) {
		auto locked = wp.lock();
		return !locked || locked.get() == object;
	});
}

void Sector::set_bounds(const BoundingBox& bounds)
{
	bounds_ = bounds;
}
'''
        content = implementation

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Processed {filepath} with encoding {used_encoding}")

print("Processing files...")
process_file("SandyHeroesClient/Sector.h", True)
process_file("SandyHeroesClient/Sector.cpp", False)
process_file("SandyHeroesServer/Sector.h", True)
process_file("SandyHeroesServer/Sector.cpp", False)
