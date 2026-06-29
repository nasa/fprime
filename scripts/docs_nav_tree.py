import os
from pathlib import Path
import yaml
import re # for reg expression matching of nav: block in mkdocs.yml

MKDOCS_YML = Path("mkdocs.yml")
output_file = "mkdocs_nav.yml"
root = "docs"
excluded_folders = {"Home", "docs-venv", "doxygen", "index", "INSTALL", "tutorials-hello-world", "tutorials-led-blinker", "tutorials-arduino-led-blinker", "tutorials-math-component"}

tab_order = ["getting-started", "tutorials", "user-manual", "how-to", "reference"]

def build_nav_tree(path):
    nav = []
    entries = sorted(os.listdir(path), key=lambda x: tab_order.index(x) if x in tab_order else len(tab_order))

    # Detect folder index.md first
    index_md_path = os.path.join(path, "index.md")
    folder_index_title = None
    if os.path.isfile(index_md_path):
        folder_index_title = read_h1(Path(index_md_path))

    for entry in entries:
        if entry == "fprime": # Entire fprime repo is copied, but only want to nav fprime/docs
                entry = "fprime/docs"
        full_path = os.path.join(path, entry)

        # Include only markdown files (but skip index.md here; we add it separately as the folder root link)
        if os.path.isfile(full_path) and entry.endswith(".md") and not entry.endswith("index.md"):
            # Check if the file stem (filename without .md) is excluded
            file_stem = Path(entry).stem
            if file_stem in excluded_folders:
                continue
            title = read_h1(Path(full_path))
            title = reformat_entry(title)
            if title in excluded_folders:
                continue
            nav.append({title: os.path.relpath(full_path, root)})

        # Process folders only if they contain .md files inside
        elif os.path.isdir(full_path):
            if entry in excluded_folders:
                continue
            
            if entry.lower() == "tutorials": # manually defines tutorial tree since its links are external
                nav.append(build_tutorials())
                continue

            children = build_nav_tree(full_path)

            if children:
                # If folder has index.md, use that as title
                index_file = os.path.join(full_path, "index.md")
                if os.path.isfile(index_file):
                    folder_title = read_h1(Path(index_file))
                    folder_title = reformat_entry(folder_title)
                    if folder_title in excluded_folders:
                        continue
                    nav.append({folder_title: children})
                else:
                    entry = reformat_entry(entry)
                    if entry in excluded_folders:
                        continue
                    nav.append({entry: children})

    # If this folder has index.md at the root level, prefix it to the folder’s nav list so it becomes the “folder level” link
    if folder_index_title:
        formatted_title = reformat_entry(folder_index_title)
        if formatted_title not in excluded_folders:
            nav.insert(0, {formatted_title: os.path.relpath(index_md_path, root)})

    return nav

# If entry title starts with a lowercase letter, make uppercase and replace '-' with spaces
def reformat_entry(entry):
    if entry.startswith("How-To: "):
        return entry.replace("How-To: ", "")
    elif entry.islower():
        entry = entry.replace("-", " ")
        entry = entry.title()
        entry = entry.replace("Gds", "GDS")
        return entry
    else:
        return entry

def read_h1(md_file: Path) -> str:
      """Return the first '# Heading' (or Title) line from a markdown file, or the
      filename stem if no H1 is present. This mirrors awesome-nav's
      behavior of titling pages by their first heading."""
      if not md_file.exists():
          return md_file.stem
      for line in md_file.read_text().splitlines():
          if line.startswith("# "):
              return line[2:].strip()
          elif line.startswith("Title: ") or line.startswith("title: "):
              return line[7:].strip()
      return md_file.stem
      

def write_mkdocs_nav(root_path, output_file="mkdocs_nav.yml"):
    print("Found root directory", root_path)
    nav_tree = build_nav_tree(root_path)
    yaml_output = yaml.dump(
        nav_tree,
        sort_keys=False,
        default_flow_style=False,
        allow_unicode=True
    )

    with open(output_file, "w") as f:
        f.write(yaml_output)

    print(f"Wrote YAML navigation to {output_file}")

    return yaml_output

def write_nav_into_mkdocs(nav: list) -> None:
    """Replace the body of the existing `nav:` block in mkdocs.yml.
    Everything else in the file is untouched."""
    text = MKDOCS_YML.read_text()
    lines = text.splitlines(keepends=True)

    # Find the line that starts the top-level `nav:` block.
    start = next((i for i, ln in enumerate(lines)
                if re.match(r"^nav:\s*$", ln)), None)
    if start is None:
        raise SystemExit(f"No top-level `nav:` block found in {MKDOCS_YML}.")

    # Find where it ends — the next non-blank line at column 0
    # (i.e., the next top-level key like `theme:` or `plugins:`).
    end = len(lines)
    for i in range(start + 1, len(lines)):
        if lines[i].strip() and not lines[i].startswith((" ", "\t")):
            end = i
            break

    body = Path("mkdocs_nav.yml").read_text()

    # Indent everything by two spaces
    body = "".join("  " + ln for ln in body.splitlines(keepends=True))

    # Prepend the Home link to the nav body
    nav_body = "  - Home: https://fprime.jpl.nasa.gov/\n" + body

    MKDOCS_YML.write_text("".join(lines[:start + 1]) + nav_body + "".join(lines[end:]))
    print(f"Wrote YAML navigation to mkdocs.yml nav: block")

def build_tutorials():
    return {
        "Tutorials": [
            {"Tutorials Index": "tutorials/index.md"},
            {"Hello World": "tutorials/tutorials-hello-world/docs/hello-world.md"},
            {"LED Blinker": "tutorials/tutorials-led-blinker/docs/led-blinker.md"},
            {"MathComponent": "tutorials/tutorials-math-component/docs/math-component.md"},
            {"Cross-Compilation Setup": "tutorials/cross-compilation.md"},
            {"Arduino LED Blinker": "tutorials/tutorials-arduino-led-blinker/docs/arduino-led-blinker.md"},
        ]
    }

if __name__ == "__main__":  # cleaner way to write this, just testing for now
    nav_block = write_mkdocs_nav(root, output_file)
    write_nav_into_mkdocs(nav_block)