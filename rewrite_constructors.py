import os

def find_matching_paren(s, start):
    count = 0
    for i in range(start, len(s)):
        if s[i] == '(': count += 1
        elif s[i] == ')':
            count -= 1
            if count == 0:
                return i
    return -1

def find_matching_brace(s, start):
    count = 0
    for i in range(start, len(s)):
        if s[i] == '{': count += 1
        elif s[i] == '}':
            count -= 1
            if count == 0:
                return i
    return -1

def process_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        with open(filepath, 'r', encoding='cp949') as f:
            content = f.read()
    
    if 'Object* owner' not in content:
        return

    out = ""
    idx = 0
    changed = False

    while idx < len(content):
        # find 'Object* owner'
        pos = content.find('Object* owner', idx)
        if pos == -1:
            out += content[idx:]
            break
        
        # Now find the start of the line or the function name
        # backtrack to find the beginning of the declaration
        start_decl = pos
        while start_decl > idx and content[start_decl] not in (';', '}', '{', '\n'):
            start_decl -= 1
        
        # Let's find the `(` that pairs with `Object* owner`
        paren_open = content.rfind('(', idx, pos)
        if paren_open == -1:
            out += content[idx:pos+13]
            idx = pos + 13
            continue

        # Start of the line
        line_start = content.rfind('\n', idx, paren_open)
        if line_start == -1: line_start = idx
        else: line_start += 1

        # Find matching `)`
        paren_close = find_matching_paren(content, paren_open)
        if paren_close == -1:
            out += content[idx:pos+13]
            idx = pos + 13
            continue
            
        # Now find the end of the declaration/definition.
        search_idx = paren_close + 1
        while search_idx < len(content) and content[search_idx] in ' \t\n\r':
            search_idx += 1
            
        end_decl = -1
        if search_idx < len(content):
            if content[search_idx] == ';':
                end_decl = search_idx
            elif content[search_idx] == ':': # initializer list
                # find `{`
                brace_open = content.find('{', search_idx)
                if brace_open != -1:
                    end_decl = find_matching_brace(content, brace_open)
            elif content[search_idx] == '{':
                end_decl = find_matching_brace(content, search_idx)
        
        if end_decl != -1:
            block = content[line_start:end_decl+1]
            new_block = block.replace('Object* owner', 'const std::shared_ptr<Object>& owner')
            out += content[idx:line_start]
            out += block + "\n" + new_block
            idx = end_decl + 1
            changed = True
        else:
            out += content[idx:pos+13]
            idx = pos + 13

    if changed:
        try:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(out)
        except Exception:
            with open(filepath, 'w', encoding='cp949') as f:
                f.write(out)
        print(f"Updated {filepath}")

def main():
    dirs = ['SandyHeroesClient', 'SandyHeroesServer']
    for d in dirs:
        for root, _, files in os.walk(d):
            for file in files:
                if file.endswith('.h') or file.endswith('.cpp'):
                    process_file(os.path.join(root, file))

if __name__ == '__main__':
    main()
