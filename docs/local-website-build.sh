#!/bin/bash

########################################################################
# README:
#
# This script is a convenience tool for building and serving the docs part
# of the F´ website locally. It will NOT look like the actual F´ website
# since themes and other content is no included in the core F´ repository
#
# DISCLAIMER: This script is experimental and has been authored by AI, 
#             then reviewed and edited by a human
#
# How-To Run:
# 0. Side effects: this script will create a Python virtual environment at `docs/docs-venv`
#       and an output directory at `../fprime-docs-site-local` relative to the fprime/ root.
# 1. Change working directory to the parent of the root of the fprime repository
#    e.g. `cd path/to/fprime && cd ..`
# 2. Run this script: `./fprime/docs/local-website-build.sh`
# 
########################################################################

set -euo pipefail

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# This script is intended to work from an fprime/ checkout alone.
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_DIR="$SCRIPT_DIR/docs-venv"
# mkdocs.yml sets docs_dir to the fprime/ root, so site_dir must be outside it.
SITE_DIR="$REPO_ROOT/../fprime-docs-site-local"
PORT=8000

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}F Prime Nested Docs Local Build${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

if ! command -v python3 >/dev/null 2>&1; then
	echo -e "${RED}Error: python3 is required but was not found.${NC}"
	exit 1
fi

if [ ! -d "$VENV_DIR" ]; then
	echo -e "${GREEN}Creating isolated virtual environment at docs/docs-venv...${NC}"
	python3 -m venv "$VENV_DIR"
else
	echo -e "${GREEN}Using existing virtual environment at docs/docs-venv...${NC}"
fi

# shellcheck disable=SC1091
source "$VENV_DIR/bin/activate"

echo -e "${GREEN}Installing/updating Python dependencies...${NC}"
python -m pip install --upgrade pip
python -m pip install -Ur "$REPO_ROOT/requirements.txt"
python -m pip install \
	mkdocs \
	mkdocs-material \
	mkdocs-awesome-nav \
	mike \
	mkdocs-open-in-new-tab \
	mkdocs-multirepo-plugin \
	pymdown-extensions \
	markdown-callouts \
    pygments==2.18.0

echo -e "${GREEN}Building nested docs website only...${NC}"
rm -rf "$SITE_DIR"

# Temporarily disable custom_dir since overrides/ doesn't exist in standalone fprime/ checkout
sed 's/custom_dir:/#custom_dir:/' "$SCRIPT_DIR/mkdocs.yml" > "$SCRIPT_DIR/mkdocs.local.yml"

mkdocs build --clean --config-file "$SCRIPT_DIR/mkdocs.local.yml" --site-dir "$SITE_DIR"

if [ $? -ne 0 ]; then
	echo -e "${RED}Error: Documentation build failed${NC}"
	exit 1
fi

if [ ! -d "$SITE_DIR" ] || [ -z "$(ls -A "$SITE_DIR")" ]; then
	echo -e "${RED}Error: build output directory is empty: $SITE_DIR${NC}"
	exit 1
fi

if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null 2>&1; then
	PORT=8001
fi

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Nested docs build successful!${NC}"
echo -e "${GREEN}========================================${NC}"
echo -e "${BLUE}Site directory:${NC} $SITE_DIR"
echo -e "${BLUE}Serving at:${NC}      http://localhost:$PORT"
echo ""
echo -e "${BLUE}Press Ctrl+C to stop the server${NC}"
echo ""

cd "$SITE_DIR"
python -m webbrowser "http://localhost:$PORT/docs"
python -m http.server "$PORT"