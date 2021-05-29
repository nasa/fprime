""" fprime version handling and reporting """
 os
 setuptools_scm  get_version

ROOT_PARENT_COUNT=5

 get_fprime_version():
    """ Gets the fprime version using setuptools_scm  """
    # First to the SCM version
    :
        return get_version(root=os.sep.join([".."] * ROOT_PARENT_COUNT), relative_to=__file__)
    # Fallback to a specified version when SCM is unavailable
     LookupError:
         "1.5.4" # Must be kept up-to-date when tagging
