# Generates docs/user-manual/index.md at build time via mkdocs-gen-files.

from pathlib import Path
import mkdocs_gen_files
import yaml

# Source of truth for section order and external links: fprime/.nav.yml
NAV_FILE = Path(__file__).parent.parent.parent / ".nav.yml"

# Source of truth for pages within a section: filesystem walk of docs/user-manual/<section>/
USER_MANUAL_DIR = Path(__file__).parent.parent / "user-manual"

# Section blurbs shown next to each top-level entry. Keys must match the titles in .nav.yml.
SECTION_DESCRIPTIONS = {
    "Overview": "Technical overview of the F´ ecosystem.",
    "Framework": "Learn concepts and mechanisms needed to build and use an F´ application.",
    "FPP": "In-depth user guide for F Prime Prime (FPP), the F´ modeling language.",
    "GDS": "Learn how to use the F Prime Ground Data System, and how it can be used to test F´ applications.",
    "Design Patterns": "Learn about common design patterns used in F´ applications.",
    "Build System": "Learn about the F´ build system and how to customize it.",
    "Security": "Security considerations when designing and developing F´ applications.",
}


def find_section(nav, *keys):
    # Walk a nav list (list of single-key dicts) following the given keys.
    current = nav
    for key in keys:
        for entry in current:
            if isinstance(entry, dict) and key in entry:
                current = entry[key]
                break
        else:
            raise KeyError(f"Could not find '{key}' in nav at {NAV_FILE}")
    return current


with NAV_FILE.open() as nav_file:
    nav_data = yaml.safe_load(nav_file)

entries = find_section(nav_data["nav"], "Documentation", "User Manual")


def read_h1(md_file: Path) -> str | None:
    # Return the first H1 (header) line in a markdown file, skipping YAML frontmatter.
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
    # Yield (title, url) for each page in a section. Url is relative to user-manual/.
    for entry in sorted(section_dir.iterdir()):
        if entry.name.startswith((".", "_")) or entry.name == "index.md":
            continue
        if entry.is_dir():
            sub_index = entry / "index.md"
            if sub_index.exists():
                title = read_h1(sub_index)
                yield title, f"{section_dir.name}/{entry.name}/"
            # else: probably an asset dir (img/, _includes/, etc.) — skip
        elif entry.suffix == ".md":
            title = read_h1(entry)
            yield title, f"{section_dir.name}/{entry.stem}/"


with mkdocs_gen_files.open("docs/user-manual/index.md", "w") as f:
    f.write("---\nhide:\n  - toc\n---\n\n")
    
    f.write("# User Manual\n\n")
    f.write(
        "The User Manual dives into F Prime concepts and usage, "
        "providing a deep understanding of how the framework operates. "
        "The different chapters are listed below.\n\n"
    )

    f.write(
        "> [!TIP]\n"
        "> **← Navigation pane**  \n"
        "> Use the navigation pane on the left to explore the different chapters of the User Manual. "
        "If the navigation pane is not visible, click on the menu icon (three horizontal lines) at the top left corner of the page. "
        "The navigation pane is hidden on narrow screens or if zoomed in.\n\n"
    )

    f.write("## Table of Contents\n\n")

    for entry in entries:
        [(title, target)] = entry.items()
        if title == "User Manual Index":
            continue  # this is the file being generated

        description = SECTION_DESCRIPTIONS.get(title, "")
        suffix = f" — {description}" if description else ""

        if target.startswith(("http://", "https://")):
            # External section — wrap in <details> for visual consistency with local sections
            f.write('<details markdown="1">\n')
            f.write(f"<summary><strong>{title}</strong>{suffix}</summary>\n\n")
            f.write(f"- [{title} ↗]({target})\n")
            f.write("\n</details>\n\n")
            continue

        # Local section — collapsible <details> block with the page list inside
        section_name = target.removeprefix("docs/user-manual/").rstrip("/")
        section_dir = USER_MANUAL_DIR / section_name
        f.write('<details markdown="1">\n')
        f.write(f"<summary><strong>{title}</strong>{suffix}</summary>\n\n")
        for page_title, page_url in list_pages(section_dir):
            f.write(f"- [{page_title}]({page_url})\n")
        f.write("\n</details>\n\n")