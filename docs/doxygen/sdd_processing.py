from pathlib import Path

SDD_INDEX_FILE = "./docs/reference/sdd.md"

# Append the new file path to index.md
with open(Path(SDD_INDEX_FILE), 'a') as index_file:

    # Find all sdd.md files and process them
    # Sort them for convenience, but also can't use the rglob generator since
    # it would start matching generated files, so using sorted to create a list
    for file in sorted(Path('.').rglob('*/docs/sdd.md')):
        # Get module name and parent directory name
        second_parent = file.parents[1].name
        third_parent = file.parents[2].name

        if third_parent in ["", ".", "fprime", "Ref"] or third_parent.startswith("cookiecutter-"):
            continue

        index_file.write(f"- [{third_parent}::{second_parent}](../../{third_parent}/{second_parent}/docs/sdd.md)\n")
