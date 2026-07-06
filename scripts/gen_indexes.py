#!/usr/bin/env python3
"""
Generates docs/<section>/index.md from the filesystem structure.
"""
# Run before docs_nav_tree.py so index.md are properly included in the nav tree

from pathlib import Path

# Get the directory where this script is located
SCRIPT_DIR = Path(__file__).parent.parent

# Source of truth: filesystem walk of docs/<section>/
SOURCE_DIRS = [SCRIPT_DIR /"docs"/"user-manual", SCRIPT_DIR /"docs"/"how-to", SCRIPT_DIR /"docs"/"reference"]

# Section blurbs shown next to each top-level entry. Keys are section directory names.
SECTION_DESCRIPTIONS = { # note: these are not automated, new sections will be populated without descriptions unless defined below
    "user-manual": {
        "overview": "Technical overview of the F´ ecosystem.",
        "framework": "Learn concepts and mechanisms needed to build and use an F´ application.",
        "fpp": "In-depth user guide for F Prime Prime (FPP), the F´ modeling language.",
        "gds": "Learn how to use the F Prime Ground Data System, and how it can be used to test F´ applications.",
        "design-patterns": "Learn about common design patterns used in F´ applications.",
        "build-system": "Learn about the F´ build system and how to customize it.",
        "security": "Security considerations when designing and developing F´ applications.",
    },
    "how-to": {
        "dev": "Learn how to develop components, libraries, and other F´ artifacts.",
        "integrate": "Learn how to integrate F´ with external libraries and platforms.",
        "ops": "Learn how to operate F´ applications and the Ground Data System.",
        "test": "Learn how to test F´ components and applications.",
    },
    "reference": {
        # fill in later
    },
}

# Section metadata: title and description for each main section
SECTION_METADATA = {
    "user-manual": {
        "title": "User Manual",
        "description": "The User Manual provides comprehensive documentation for understanding and using F Prime.",
    },
    "how-to": {
        "title": "How-To",
        "description": "How-To guides offer step-by-step instructions for specific development tasks in F Prime.",
    },
    "reference": {
        "title": "Reference",
        "description": "Reference documentation for F Prime APIs and components.",
    },
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
    """Yield (title, url) for each page in a section. Url is relative to the parent directory."""
    for entry in sorted(section_dir.iterdir()):
        if entry.name.startswith((".", "_")) or entry.name == "index.md":
            continue
        if entry.is_dir():
            sub_index = entry / "index.md"
            if sub_index.exists():
                title = read_h1(sub_index)
                title = reformat_entry(title)
                yield title, f"{section_dir.name}/{entry.name}/index.md"
            # else: probably an asset dir (img/, _includes/, etc.) — skip
        elif entry.suffix == ".md":
            title = read_h1(entry)
            title = reformat_entry(title)
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


def get_sections(source_dir: Path):
    """Yield (section_path, title, is_file) for each section in the given directory."""
    section_name = source_dir.name
    desired_order = list(SECTION_DESCRIPTIONS.get(section_name, {}).keys())

    def sort_key(entry):
        if entry.name in desired_order:
            return (0, desired_order.index(entry.name))
        else:
            return (1, entry.name)  # unlisted items go last, alphabetically

    for entry in sorted(source_dir.iterdir(), key=sort_key):
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
        entry = entry.replace("dev", "develop")
        entry = entry.replace("ops", "operate")
        entry = entry.title()
        entry = entry.replace("Gds", "GDS")
        return entry
    else:
        return entry

def generate_index_for_section(source_dir: Path):
    """Generate the index page for a given documentation section."""
    section_name = source_dir.name
    metadata = SECTION_METADATA.get(section_name, {})
    section_title = metadata.get("title", section_name.replace("-", " ").title())
    section_description = metadata.get("description", "")

    # Get section-specific descriptions
    subsection_descriptions = SECTION_DESCRIPTIONS.get(section_name, {})

    # Generate the markdown content
    content = []
    content.append("---\nhide:\n  - toc\n---\n\n")
    content.append(f"# {section_title}\n\n")

    if section_description:
        content.append(f"{section_description}\n\n")

    content.append(
        "> [!TIP]\n"
        "> **← Navigation pane**  \n"
        "> Use the navigation pane on the left to explore the different chapters of the "
        f"{section_title} documentation. "
        "If the navigation pane is not visible, click on the menu icon (three horizontal lines) at the top left corner of the page. "
        "The navigation pane is hidden on narrow screens or if zoomed in.\n\n"
    )
    content.append("## Table of Contents\n\n")

    for section_path, title, is_file in get_sections(source_dir):
        description = subsection_descriptions.get(section_path.name, "")
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
    output_file = source_dir / "index.md"
    output_file.write_text("".join(content), encoding="utf-8")
    print(f"Generated {output_file.relative_to(SCRIPT_DIR)}")


def main():
    """Generate index pages for all documentation sections."""
    for source_dir in SOURCE_DIRS:
        if source_dir.exists():
            generate_index_for_section(source_dir)
        else:
            print(f"Warning: {source_dir} does not exist, skipping...")


if __name__ == "__main__":
    main()
