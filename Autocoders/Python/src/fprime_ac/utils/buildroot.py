"""
fprime_ac.utils.buildroot:

Helper functions to deal with build root.
"""
import functools
import os
import sys

BUILD_ROOTS = set()  # One global build root set, to rule them all


def set_build_roots(build_root_arg):
    """
    Sets up the build root flag from an argument.

    :param build_root_arg: argument supplied as build root
    """
    global BUILD_ROOTS
    normalized = map(
        lambda path: os.path.normpath(os.path.realpath(path)), build_root_arg.split(":")
    )
    build_root_elements = set(normalized)
    # Update global store
    BUILD_ROOTS = build_root_elements


def get_build_roots():
    """
    Get the build root paths to use for handling multiple roots for elements.

    :return: build roots
    """
    return BUILD_ROOTS


def get_nearest_build_root(path):
    """
    Gets the nearest build root to given path

    :param path: path to find nearest build root to
    :return: nearest build root
    """
    path = os.path.abspath(path)
    parents = filter(
        lambda build: os.path.commonpath([build, path]) == build, get_build_roots()
    )

    def path_reducer(agg, item):
        """Reduces to the longest path"""
        common = os.path.commonpath([item, path])
        if agg is None or len(common) > len(agg):
            return common
        return agg

    return functools.reduce(path_reducer, parents, None)


def locate_build_root(item):
    """
    Locates a file relative to some BUILD_ROOT.

    :param item: item to locate at some build root
    :return: build root that item was found at
    """
    search_locations = get_build_roots()
    occurrences = map(lambda build: os.path.join(build, item), search_locations)
    hits = list(filter(os.path.exists, occurrences))
    if not hits:
        raise BuildRootMissingException(item)
    elif len(hits) > 1:
        raise BuildRootCollisionException(hits)
    return hits[0]


def search_for_file(file_type, file_path):
    """
    Searches for a given included port or serializable by looking in all specified build roots, then the exact path.
    @param file_type: type of file searched for
    @param file_path: path to look for based on offset
    @return: full path of file
    """
    try:
        checker = locate_build_root(file_path)
        return checker
    except BuildRootMissingException:
        print(
            "ERROR: {} xml specification file {} does not exist!".format(
                file_type, file_path
            ),
            file=sys.stderr,
        )
        sys.exit(-1)
    except BuildRootCollisionException as brc:
        print(
            "ERROR: {} xml specification file exists multiple times {}".format(
                file_type, str(brc)
            ),
            file=sys.stderr,
        )
        sys.exit(-1)


def build_root_relative_path(path):
    """
    Calculate the relative path to the nearest build root.

    :param path: path to calculate from
    :return: closes relative path
    """
    path = os.path.normpath(os.path.realpath(path.replace("\\", os.sep)))
    build_root = get_nearest_build_root(path)
    if build_root is None:
        raise BuildRootMissingException(path)
    return os.path.relpath(path, build_root)


class BuildRootMissingException(Exception):
    """
    Determined that an element exists at multiple BUILD_ROOT locations
    """

    def __init__(self, item):
        """Initialize super exception"""
        super().__init__(
            "{} not found under any location: {}".format(
                item, ",".join(get_build_roots())
            )
        )


class BuildRootCollisionException(Exception):
    """
    Determined that an element exists at multiple BUILD_ROOT locations
    """

    def __init__(self, occurrences):
        """Initialize super exception"""
        super().__init__(
            "Item found at multiple locations: {}".format(",".join(occurrences))
        )
