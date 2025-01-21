import os
import shutil
from pathlib import Path

SDD_DIR = "./docs/documentation/reference/sdd"
os.makedirs(SDD_DIR, exist_ok=True)

# Append the new file path to index.md
with open(Path(SDD_DIR) / 'index.md', 'a') as index_file:

    # Find all sdd.md files and process them
    for file in list(Path('.').rglob('*/docs/sdd.md')):
        # Get module name and parent directory name
        second_parent = file.parents[1].name
        third_parent = file.parents[2].name

        if third_parent in ["", ".", "fprime", "Ref"] or third_parent.startswith("cookiecutter-"):
            continue

        source_dir = file.parents[1] / 'docs'
        dest_dir = Path(SDD_DIR) / third_parent / second_parent / 'docs'

        os.makedirs(dest_dir, exist_ok=True)
        shutil.copytree(source_dir, dest_dir, dirs_exist_ok=True)

        index_file.write(f"- [{third_parent}::{second_parent}](./{third_parent}/{second_parent}/docs/sdd.md)\n")
