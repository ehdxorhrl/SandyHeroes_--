import os
import re
import glob

def safe_replace(file_path):
    with open(file_path, 'r', encoding='ansi') as f:
        content = f.read()

    original_content = content
    
    # Simple substitution strategy:
    # We replace `owner_->` with `locked_owner->` and insert `auto locked_owner = owner_.lock(); if(!locked_owner) return ...;`
    # if it's not already handled.
    
    if 'TextComponent.cpp' in file_path:
        content = content.replace('if (!owner_)', 'auto locked_owner = owner_.lock();
		if (!locked_owner)'.replace('
', '
').replace('	', '	'))
        content = content.replace('std::string name = owner_->name();', 'std::string name = locked_owner->name();')
        content = content.replace('text_ = get_text_func_(owner_);', 'if(locked_owner) text_ = get_text_func_(locked_owner.get());')

    elif 'UiMeshComponent.cpp' in file_path:
        content = content.replace('XMVECTOR world_pos = XMLoadFloat3(&owner_->world_position_vector());', 'auto locked_owner = owner_.lock();
		if(!locked_owner) return;
		XMVECTOR world_pos = XMLoadFloat3(&locked_owner->world_position_vector());'.replace('
', '
').replace('	', '	'))

    elif 'SkinnedMeshComponent.cpp' in file_path:
        content = content.replace('Object* hierarchy_root = owner_->GetHierarchyRoot();', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return;
	Object* hierarchy_root = locked_owner->GetHierarchyRoot();'.replace('
', '
').replace('	', '	'))

    elif 'TestControllerComponent.cpp' in file_path:
        content = content.replace('owner_->Rotate(', 'auto locked_owner = owner_.lock();
			if(locked_owner) locked_owner->Rotate('.replace('
', '
').replace('	', '	'))
        content = content.replace('XMFLOAT3 look = owner_->look_vector();', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return;
	XMFLOAT3 look = locked_owner->look_vector();'.replace('
', '
').replace('	', '	'))
        content = content.replace('XMFLOAT3 right = owner_->right_vector();', 'XMFLOAT3 right = locked_owner->right_vector();')

    elif 'ParticleComponent.cpp' in file_path:
        content = re.sub(r'owner_->', 'locked_owner->', content)
        content = content.replace('XMFLOAT3 pivot_position = locked_owner->world_position_vector();', 'auto locked_owner = owner_.lock();
				if(!locked_owner) return;
				XMFLOAT3 pivot_position = locked_owner->world_position_vector();'.replace('
', '
').replace('	', '	'))
        # Fix missing locks that were introduced because regex changed them:
        content = content.replace('position = locked_owner->world_position_vector();', 'auto locked_owner = owner_.lock();
				if(!locked_owner) break;
				position = locked_owner->world_position_vector();'.replace('
', '
').replace('	', '	'))

    elif 'BoxColliderComponent.cpp' in file_path:
        content = content.replace('box_.Transform(animated_box_, XMLoadFloat4x4(&owner_->world_matrix()));', 'if(auto locked_owner = owner_.lock()) box_.Transform(animated_box_, XMLoadFloat4x4(&locked_owner->world_matrix()));')

    elif 'ColliderComponent.cpp' in file_path:
        content = content.replace('bounding_box_->Transform(animated_bounding_box_, XMLoadFloat4x4(&owner_->world_matrix()));', 'if(auto locked_owner = owner_.lock()) bounding_box_->Transform(animated_bounding_box_, XMLoadFloat4x4(&locked_owner->world_matrix()));')

    elif 'MeshColliderComponent.cpp' in file_path:
        content = content.replace('XMMATRIX world = XMLoadFloat4x4(&owner_->world_matrix());', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return;
	XMMATRIX world = XMLoadFloat4x4(&locked_owner->world_matrix());'.replace('
', '
').replace('	', '	'))
        content = content.replace('XMFLOAT4X4 mat = owner_->world_matrix();', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return false;
	XMFLOAT4X4 mat = locked_owner->world_matrix();'.replace('
', '
').replace('	', '	'))
        content = content.replace('XMFLOAT4X4 world_matrix_copy = owner_->world_matrix();', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return false;
	XMFLOAT4X4 world_matrix_copy = locked_owner->world_matrix();'.replace('
', '
').replace('	', '	'))
        content = content.replace('XMFLOAT4X4 world_mat = owner_->world_matrix();', 'auto locked_owner = owner_.lock();
	if(!locked_owner) return BoundingOrientedBox{};
	XMFLOAT4X4 world_mat = locked_owner->world_matrix();'.replace('
', '
').replace('	', '	'))

    if content != original_content:
        with open(file_path, 'w', encoding='ansi') as f:
            f.write(content)
        print(f"Updated {file_path}")

client_files = glob.glob('SandyHeroesClient/*Component.cpp')
server_files = glob.glob('SandyHeroesServer/*Component.cpp')

for f in client_files + server_files:
    safe_replace(f)
