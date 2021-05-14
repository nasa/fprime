"""
@brief Base class for all loaders that load dictionaries of python fragments

The PythonLoader class inherits from the DictLoader base class and is intended
to be inherited by all loader classes that read dictionaries in the file system
made up of multiple python file fragments.

This Class only adds helper functions and thus does not overwrite any methods in
in the base DictLoader class. Because of this, all dictionaries returned from
this class will be empty.

@date Created July 3, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

import glob
import importlib
import os
import sys

from fprime_gds.common.data_types import exceptions

# Custom Python Modules
from . import dict_loader


class PythonLoader(dict_loader.DictLoader):
    """Class to help load python file based dictionaries"""

    def read_dict(self, path, use_superpkg=False):
        """
        Reads all python modules at the given path and constructs a dict list

        This function assumes that path is a directory containing many python
        module files. Each module file has several fields with associated
        values. This function reads each file and constructs a dictionary using
        the fields and their values. These dictionaries are then compiled into
        a list that is returned.

        Args:
            path: File Path to a folder containing python modules to load
            use_superpkg: [Default=False] When true, modules will be imported
                           using both the package and superpackage qualifiers
                           (from A.B import C instead of from B import C). This
                           allows multiple dictionaries with the same package
                           name to be imported. This is especially important
                           when trying to import dictionaries from multiple
                           deployments.

        Returns:
            A list of dictionaries. Each dictionary represents one loaded module
            and for keys has the names of the fields in the file and for values
            has the values of those fields.
        """
        modules = self.import_modules(path, use_superpkg)

        module_dicts = []
        for module in modules:
            # Create a dictionary for this module's fields
            mod_dict = dict()
            for field in dir(module):
                # Verify it is not a hidden field (doesn't start with "__")
                if field.find("__") != 0:
                    mod_dict[field] = getattr(module, field)

            module_dicts.append(mod_dict)

        return module_dicts

    @staticmethod
    def import_modules(path, use_superpkg):
        """
        Imports all modules in the given directory.

        Args:
            path: File Path to a folder containing python modules to load
            use_superpkg: When true, modules will be imported
                           using both the package and superpackage qualifiers
                           (from A.B import C instead of from B import C). This
                           allows multiple dictionaries with the same package
                           name to be imported. This is especially important
                           when trying to import dictionaries from multiple
                           deployments.

        Returns:
            A list of module objects of all the newly imported modules
        """
        # Verify path is a directory
        if not os.path.isdir(path):
            raise exceptions.GseControllerUndefinedDirectoryException(path)

        # Compute package and superpackage names
        (superpkg_path, pkg) = os.path.split(path)
        (rest_of_path, superpkg) = os.path.split(superpkg_path)

        # Make sure the directory we are importing from is in the python path
        if use_superpkg:
            sys.path.append(rest_of_path)
        else:
            sys.path.append(superpkg_path)

        # Make sure serializable directory is imported
        sys.path.append(superpkg_path + os.sep + "serializable")

        # Compute a list of all files to import
        all_files = glob.glob(path + os.sep + "*.py")

        module_files = []
        for py_file in all_files:
            (file_path, file_name) = os.path.split(py_file)
            if file_name != "__init__.py":
                module_files.append(file_name)

        # Import modules
        module_list = []
        for mf in module_files:
            # Strip off .py from name by splitting at '.' and taking the first
            # string
            mod_name = mf.split(".")[0]

            if use_superpkg:
                import_name = "{}.{}.{}".format(superpkg, pkg, mod_name)
            else:
                import_name = "{}.{}".format(pkg, mod_name)

            m = importlib.import_module(import_name)

            module_list.append(m)

        return module_list
