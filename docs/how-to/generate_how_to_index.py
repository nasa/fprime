#!/usr/bin/env python3
"""
Generates docs/how-to/index.md from the filesystem structure.
Run this script before building the docs: python fprime/docs/how-to/generate_how_to_index.py
"""

from pathlib import Path

# Source of truth: filesystem walk of docs/how-to/<section>/
HOW_TO_DIR = Path(__file__).parent

# Output file
OUTPUT_FILE = HOW_TO_DIR / "index.md"

# Section blurbs shown next to each top-level entry. Keys are section directory names.
SECTION_DESCRIPTIONS = {
    "dev": "Learn how to develop components, libraries, and other F´ artifacts.",
    "integrate": "Learn how to integrate F´ with external libraries and platforms.",
    "ops": "Learn how to operate F´ applications and the Ground Data System.",
    "test": "Learn how to test F´ components and applications.",
}



def read_h1(md_file: Path) -> str | None:
    """Return the first H1 (header) line in a markdown file, skipping YAML frontmatter."""
    text = md_file.read_text(encoding="utf-8")
    if text.startswith("---\n"):
        end = text.find("\n---\n", 4)
        if end != -1:
            text = text[end + 5:]
    for line in text.splitlines():
        stripped = line.strip()
        if stripped.startswith("# "):
            return stripped[2:].strip()
    return None


def list_pages(section_dir: Path):
    """Yield (title, url) for each page in a section. Url is relative to how-to/."""
    for entry in sorted(section_dir.iterdir()):
        if entry.name.startswith((".", "_")) or entry.name == "index.md":
            continue
        if entry.is_dir():
            sub_index = entry / "index.md"
            if sub_index.exists():
                title = read_h1(sub_index)
                yield title, f"{section_dir.name}/{entry.name}/index.md"
            # else: probably an asset dir (img/, _includes/, etc.) — skip
        elif entry.suffix == ".md":
            title = read_h1(entry)
            yield title, f"{section_dir.name}/{entry.name}"


def get_section_title(section_dir: Path) -> str:
    """Get the title for a section from its index.md file, or fallback to directory name."""
    index_file = section_dir / "index.md"
    if index_file.exists():
        title = read_h1(index_file)
        title = reformat_entry(title)
        if title:
            return title
    # Fallback: capitalize directory name
    return reformat_entry(section_dir.name)


def get_sections():
    """Yield (section_path, title, is_file) for each section in the how-to directory."""
    for entry in sorted(HOW_TO_DIR.iterdir()):
        if entry.name.startswith((".", "_")) or entry.name == "index.md" or entry.suffix == ".py":
            continue
        if entry.is_dir():
            yield entry, get_section_title(entry), False
        elif entry.suffix == ".md":
            title = read_h1(entry)
            if title:
                yield entry, reformat_entry(title), True


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

def main():
    """Generate the how-to index page."""
    # Generate the markdown content
    content = []
    content.append("---\nhide:\n  - toc\n---\n\n")
    content.append("# How-To\n\n")
    content.append(
        "How-To guides offer step-by-step instructions for specific development tasks in F Prime.\n\n"
    )
    content.append(
        "> [!TIP]\n"
        "> **← Navigation pane**  \n"
        "> Use the navigation pane on the left to explore the different chapters of the How-To documentation. "
        "If the navigation pane is not visible, click on the menu icon (three horizontal lines) at the top left corner of the page. "
        "The navigation pane is hidden on narrow screens or if zoomed in.\n\n"
    )
    content.append("## Table of Contents\n\n")

    for section_path, title, is_file in get_sections():
        description = SECTION_DESCRIPTIONS.get(section_path.name, "")
        suffix = f" — {description}" if description else ""

        if is_file:
            # Standalone markdown file — simple bullet point link
            file_url = section_path.name
            content.append(f"- [**{title}**]({file_url}){suffix}\n\n")
        else:
            # Directory section — collect all pages
            pages = list(list_pages(section_path))

            if pages:
                # Directory with children — collapsible <details> box
                content.append('<details markdown="1">\n')
                content.append(f"<summary><strong>{title}</strong>{suffix}</summary>\n\n")
                for page_title, page_url in pages:
                    content.append(f"- [{page_title}]({page_url})\n")
                content.append("\n</details>\n\n")
            else:
                # Empty directory or only has index.md — simple bullet point link
                index_url = f"{section_path.name}/index.md"
                content.append(f"- [**{title}**]({index_url}){suffix}\n\n")

    # Write to output file
    OUTPUT_FILE.write_text("".join(content), encoding="utf-8")
    print(f"Generated {OUTPUT_FILE.relative_to(Path.cwd())}")


if __name__ == "__main__":
    main()
