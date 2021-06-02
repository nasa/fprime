#!/bin/sh -e

# Set up virtual environment
rm -rf fprime-venv
python3 -m venv ./fprime-venv
. fprime-venv/bin/activate

# Install F Prime Python packages
pip install --upgrade pip
pip install --upgrade --use-deprecated=legacy-resolver fprime-tools fprime-gds
