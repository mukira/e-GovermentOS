
import os
import shutil

patches_dir = "packages/e-governmentos/chromium_patches"

def rename_browseros(path):
    new_name = path.replace("browseros", "e_governmentos")
    return new_name

def process_renames():
    # Collect all paths first to avoid issues while renaming (bottom-up)
    paths_to_rename = []
    
    for root, dirs, files in os.walk(patches_dir, topdown=False):
        for name in files:
            if "browseros" in name:
                paths_to_rename.append(os.path.join(root, name))
        for name in dirs:
            if "browseros" in name:
                paths_to_rename.append(os.path.join(root, name))
                

def process_renames():
    # Collect all paths first to avoid issues while renaming (bottom-up)
    paths_to_rename = []
    
    for root, dirs, files in os.walk(patches_dir, topdown=False):
        for name in files:
            if "browseros" in name:
                paths_to_rename.append(os.path.join(root, name))
        for name in dirs:
            if "browseros" in name:
                paths_to_rename.append(os.path.join(root, name))
                
    for path in paths_to_rename:
        dirname, basename = os.path.split(path)
        new_basename = basename.replace("browseros", "e_governmentos")
        new_path = os.path.join(dirname, new_basename)
        if path != new_path:
            print(f"Renaming {path} -> {new_path}")
            os.rename(path, new_path)

if __name__ == "__main__":
    process_renames()
