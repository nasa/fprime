# Python 2/3 compatibility for namespace packages
__path__ = __import__("pkgutil").extend_path(__path__, __name__)

# Currently the GDS is backwards compatible with all dictionaries. This MUST be bumped with each framework release to
# ensure compatibility
MINIMUM_SUPPORTED_FRAMEWORK_VERSION = "0.0.0"
MAXIMUM_SUPPORTED_FRAMEWORK_VERSION = "1.5.4"
