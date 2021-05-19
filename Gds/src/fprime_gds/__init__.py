# Python 2/3 compatibility for namespace packages
__path__ = __import__("pkgutil").extend_path(__path__, __name__)
