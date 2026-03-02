import os
import re
import glob

def refactor_owner_access(file_path):
    with open(file_path, 'r', encoding='ansi') as f:
        content = f.read()

    # Skip if no owner_->
    if 'owner_->' not in content:
        return

    print(f"Refactoring {file_path}")

    # For functions returning void:
    # Match void ClassName::FuncName(...) { ... owner_-> ... }
    # Let's just do targeted replacements for the components we know.
    
    # 1. ChestComponent (Client)
    if 'ChestComponent.cpp' in file_path and 'Client' in file_path:
        content = content.replace('scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());',
                                  'if(auto locked_owner = owner_.lock()) scroll->set_transform_matrix(locked_owner->transform_matrix() * scroll->transform_matrix());')
        content = content.replace('owner_', 'locked_owner') # wait, this will replace all owner_. Let's just replace specific parts.
    
    # Let's just replace all `owner_->` with `locked_owner->` and inject `auto locked_owner = owner_.lock(); if(!locked_owner) return ...;` where appropriate, using specific string replacements.

client_files = glob.glob('SandyHeroesClient/*Component.cpp')
server_files = glob.glob('SandyHeroesServer/*Component.cpp')

for f in client_files + server_files:
    # skip already done or complex
    pass
