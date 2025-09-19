import os
import shutil
import sys
import zipfile

base_dir = sys.argv[1]

copy_dir = base_dir + "_copy"

def copy_directory(src_dir, dst_dir):
    def ignore_git(dir, contents):
        return ['.git'] if '.git' in contents else []

    if os.path.exists(dst_dir):
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir, dst_dir, ignore=ignore_git)
    print(f"Copied directory to: {dst_dir}")
    

def zip_directory(base_dir, zip_path):
    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, _, files in os.walk(base_dir):
            for file in files:
                abs_file_path = os.path.join(root, file)
                rel_path = os.path.relpath(abs_file_path, base_dir)
                zipf.write(abs_file_path, arcname=rel_path)
    print(f"Zipped: {zip_path}")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python clean_copy_and_zip.py C:\\path\\to\\directory")
        sys.exit(1)

    original_dir = os.path.abspath(sys.argv[1])

    if not os.path.isdir(original_dir):
        print(f"Invalid directory: {original_dir}")
        sys.exit(1)

    # Create copy directory name
    parent_dir = os.path.dirname(original_dir)
    base_name = os.path.basename(original_dir)
    copy_dir = os.path.join(parent_dir, base_name + "_copy")

    # Create the copy
    copy_directory(original_dir, copy_dir)

    bake_path = os.path.join(copy_dir, 'bake.py')
    git_ignore_path = os.path.join(copy_dir, '.gitignore')
    notes_path = os.path.join(copy_dir, 'notes')
    build_path = os.path.join(copy_dir, 'build')
    vs_path = os.path.join(copy_dir, 'visual_studio')
    example_path = os.path.join(copy_dir, 'src/examples')

    os.remove(git_ignore_path)
    print(f"Removed: {git_ignore_path}")

    os.remove(bake_path)
    print(f"Removed: {bake_path}")

    shutil.rmtree(notes_path)
    print(f"Removed: {notes_path}")

    shutil.rmtree(build_path)
    print(f"Removed: {build_path}")

    shutil.rmtree(vs_path)
    print(f"Removed: {vs_path}")

    shutil.rmtree(example_path)
    print(f"Removed: {example_path}")


    # Zip the cleaned copy
    zip_path = original_dir + ".zip"
    zip_directory(copy_dir, zip_path)

    
