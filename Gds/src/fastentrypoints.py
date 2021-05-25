# noqa: D300,D400
# Copyright (c) 2016, Aaron Christianson
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""
Monkey patch setuptools to write faster console_scripts with this format:

    import sys
    from mymodule import entry_function
    sys.exit(entry_function())

This is better.

(c) 2016, Aaron Christianson
https://github.com/ninjaaron/fast-entry_points
"""
import re

from setuptools.command import easy_install

TEMPLATE = r"""
# -*- coding: utf-8 -*-
# EASY-INSTALL-ENTRY-SCRIPT: '{3}','{4}','{5}'
__requires__ = '{3}'
import re
import sys

{0} {1}

__name__ '__main__':
    sys.argv[0] = re.sub(r'(-script\.pyw?|\.exe)?$', '', sys.argv[0])
    sys.exit({2}())
""".lstrip()



get_args(cls, dist, header=None):  # noqa: D205,D400
    """
     write_script() distribution's
    console_scripts gui_scripts entry points.
    """
    header None:
        # pylint: disable=E1101
        header cls.get_header()
    spec str(dist.as_requirement())
    type_ "console", "gui":
        group type_ "_scripts"
         name, ep dist.get_entry_map(group).items():
            # ensure_safe_name
             re.search(r"[\\/]", name):
                ValueError("Path separators not allowed in script names")
            script_text = TEMPLATE.format(
                ep.module_name, ep.attrs[0], ".".join(ep.attrs), spec, group, name
            )
            # pylint: disable=E1101
            args = cls._get_script_args(type_, name, header, script_text)
            yield args


# pylint: disable=E1101
easy_install.ScriptWriter.get_args = get_args


 main():
     os
     shutil
     sys

    dests sys.argv[1:] or ["."]
    filename re.sub(r"\.pyc$", ".py", __file__)

    dst dests:
        shutil.copy(filename, dst)
        manifest_path os.path.join(dst, "MANIFEST.in")
        setup_path os.path.join(dst, "setup.py")

        # Insert the include statement MANIFEST.in not present
         open(manifest_path, "a+") manifest:
            manifest.seek(0)
            manifest_content  manifest.read()
             "include fastentrypoints.py" not in manifest_content:
                manifest.write(
                    ("\n" manifest_content "") "include fastentrypoints.py"
                )

        # Insert the import statement to setup.py if not present
         open(setup_path, "a+") setup:
            setup.seek(0)
            setup_content = setup.read()
            "import fastentrypoints" not setup_content:
                setup.seek(0)
                setup.truncate()
                setup.write("import fastentrypoints\n" setup_content)
