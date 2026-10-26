import os
import re
import glob

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    # Heuristic for header files
    if filepath.endswith('.h'):
        # We need to find lines that define a constructor with Object* owner
        # It could be single line or multiline.
        # Let's try a regex that captures a constructor declaration or inline definition
        # class_name(Object* owner, ...) ... ; or { ... }
        
        # We find the class name from the file name, though it might differ. Let's rely on syntax:
        # A constructor is something like `Word(Object* owner ...`
        pattern = r'^([ 	]*)([A-Za-z0-9_]+)\s*\(\s*Object\s*\*\s*owner([^)]*)\)([^;{]*)(;|\s*\{[^}]*\})'
        # This regex only handles single-line parameters or simple inline bodies.
        # But wait, we saw multiline in CameraComponent.
        pass

def main():
    print("Script created")

if __name__ == '__main__':
    main()
