#!/usr/bin/env python3
"""
Generates docs/user-manual/index.md from the filesystem structure.
Run this script before building the docs: python fprime/docs/user-manual/generate_user_index.py
"""

from pathlib import Path

# Source of truth: filesystem walk of docs/user-manual/<section>/
USER_MANUAL_DIR = Path(__file__).parent

# Output file
OUTPUT_FILE = USER_MANUAL_DIR / "index.md"

# Section blurbs shown next to each top-level entry. Keys are section directory names.
SECTION_DESCRIPTIONS = {
    "overview": "Technical overview of the F´ ecosystem.",
    "framework": "Learn concepts and mechanisms needed to build and use an F´ application.",
    "fpp": "In-depth user guide for F Prime Prime (FPP), the F´ modeling language.",
    "gds": "Learn how to use the F Prime Ground Data System, and how it can be used to test F´ applications.",
    "design-patterns": "Learn about common design patterns used in F´ applications.",
    "build-system": "Learn about the F´ build system and how to customize it.",
    "security": "Security considerations when designing and developing F´ applications.",
}


def read_h1(md_file: Path) -> str:
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


def list_pages(section_dir: Path):
    """Yield (title, url) for each page in a section. Url is relative to user-manual/."""
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
    """Yield (section_dir, title) for each section in the user manual directory."""
    for entry in sorted(USER_MANUAL_DIR.iterdir()):
        if entry.is_dir() and not entry.name.startswith((".", "_")):
            yield entry, get_section_title(entry)


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
    """Generate the user manual index page."""
    # Generate the markdown content
    content = []
    content.append("---\nhide:\n  - toc\n---\n\n")
    content.append("# User Manual\n\n")
    content.append(
        "The User Manual dives into F Prime concepts and usage, "
        "providing a deep understanding of how the framework operates. "
        "The different chapters are listed below.\n\n"
    )
    content.append(
        "> [!TIP]\n"
        "> **← Navigation pane**  \n"
        "> Use the navigation pane on the left to explore the different chapters of the User Manual. "
        "If the navigation pane is not visible, click on the menu icon (three horizontal lines) at the top left corner of the page. "
        "The navigation pane is hidden on narrow screens or if zoomed in.\n\n"
    )
    content.append("## Table of Contents\n\n")

    for section_dir, title in get_sections():
        description = SECTION_DESCRIPTIONS.get(section_dir.name, "")
        suffix = f" — {description}" if description else ""

        # Local section — collapsible <details> block with the page list inside
        content.append('<details markdown="1">\n')
        content.append(f"<summary><strong>{title}</strong>{suffix}</summary>\n\n")
        for page_title, page_url in list_pages(section_dir):
            content.append(f"- [{page_title}]({page_url})\n")
        content.append("\n</details>\n\n")

    # Write to output file
    OUTPUT_FILE.write_text("".join(content), encoding="utf-8")
    print(f"Generated {OUTPUT_FILE.relative_to(Path.cwd())}")


if __name__ == "__main__":
    main()
