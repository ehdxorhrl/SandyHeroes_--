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

    # Fix assignments to player_ (undo player_.lock() = ...)
    content = content.replace('player_.lock() =', 'player_ =')
    
    # Fix GetComponent/GetComponents/FindFrame etc to use .get() if passing locked weak_ptr
    # Common pattern: GetComponent<T>(player_.lock()) -> GetComponent<T>(player_.lock().get())
    content = re.sub(r'GetComponent<([a-zA-Z0-9_]+)>\(player_\.lock\(\)\)', r'GetComponent<\1>(player_.lock().get())', content)
    content = re.sub(r'GetComponents<([a-zA-Z0-9_]+)>\(player_\.lock\(\)\)', r'GetComponents<\1>(player_.lock().get())', content)
    content = re.sub(r'GetComponentInChildren<([a-zA-Z0-9_]+)>\(player_\.lock\(\)\)', r'GetComponentInChildren<\1>(player_.lock().get())', content)
    content = re.sub(r'GetComponentsInChildren<([a-zA-Z0-9_]+)>\(player_\.lock\(\)\)', r'GetComponentsInChildren<\1>(player_.lock().get())', content)
    content = content.replace('player_.lock()->FindFrame', 'player_.lock().get()->FindFrame')

    # Generic fix for locked weak_ptr passed to functions expecting Object*
    # Actually player_.lock() is shared_ptr, which has .get().
    # But if a function expects Object*, shared_ptr might not automatically convert (except in some cases).
    # In my templates, they expect Object*.
    
    # Also fix other variables if they were replaced by .lock()
    # Actually only player_ was replaced by .lock() in my script.

    with open(filepath, 'w', encoding=used_encoding) as f:
        f.write(content)
    print(f"Fixed locked weak_ptr usage in {filepath} with encoding {used_encoding}")

process_file("SandyHeroesClient/BaseScene.cpp")
process_file("SandyHeroesServer/BaseScene.cpp")
