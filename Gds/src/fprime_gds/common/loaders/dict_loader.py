"""
@brief Base class for all Loaders. Defines the Loader interface

Loaders are responsible for reading dictionaries autocoded during the fprime
build process and converting them to python dictionaries with ids or names as
keys and data_template objects as values.

Additionally, because loading dictionaries is often time consuming, this loader
saves the python dictionaries with the path that produced them so that when
a dictionary from a previously used path is asked for, the dictionary can be
returned immediately.

The base class will return empty dictionaries for any path.

@date Created July 3, 2018
@author R. Joseph Paetz

@bug No known bugs
"""


class DictLoader:
    """
    Base class for all loader classes. Defines the loader interface

    Example:
        l = DictLoader  # Or any other derived loader class
        id_dict = l.get_id_dict(path)       # May take a second or two
        name_dict = l.get_name_dict(path)   # Returns immediately
    """

    def __init__(self):
        """
        Constructor

        The base class constructor sets up the machinery so that id and name
        dictionaries for a given path can be saved and thus not have to be
        remade if they are asked for again.

        Returns:
            An initialized loader object
        """
        self.saved_dicts = dict()

    def get_id_dict(self, path):
        """
        Returns the python dictionary keyed by ids for the given path

        This function will return the same dictionary originally computed for
        the given path or will construct new dictionaries if the path has never
        been passed to the get_id_dict or the get_name_dict functions.

        This function should not be overwritten by inheriting classes

        Args:
            path: Path to the file system dictionary to convert to a python dict

        Returns:
            The id dictionary associated with the given path
        """
        if path in self.saved_dicts:
            (id_dict, name_dict) = self.saved_dicts[path]
        else:
            (id_dict, name_dict) = self.construct_dicts(path)
            self.saved_dicts[path] = (id_dict, name_dict)

        return id_dict

    def get_name_dict(self, path):
        """
        Returns the python dictionary keyed by names for the given path

        This function will return the same dictionary originally computed for
        the given path or will construct new dictionaries if the path has never
        been passed to the get_id_dict or the get_name_dict functions.

        This function should only be overwritten by inheriting classes if
        more arguments are needed to construct the dictionaries.

        Args:
            path: Path to the file system dictionary to convert to a python dict

        Returns:
            The name dictionary associated with the given path
        """
        if path in self.saved_dicts:
            (id_dict, name_dict) = self.saved_dicts[path]
        else:
            (id_dict, name_dict) = self.construct_dicts(path)
            self.saved_dicts[path] = (id_dict, name_dict)

        return name_dict

    def construct_dicts(self, path):
        """
        Constructs and returns python dictionaries keyed on id and name.

        This function should only be overwritten by inheriting classes if
        more arguments are needed to construct the dictionaries.

        Args:
            path: Path to the file system dictionary to convert to a python dict

        Returns:
            A tuple with two dictionaries: (id_dict, name_dict). They should
            have keys of the id and name fields respectively and the values
            for both should be data_template classes. This base class only
            returns empty dictionaries.
        """
        return dict(), dict()
