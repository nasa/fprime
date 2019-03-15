import sys
from . import BuildConstants

class Config(object):

    __instance = None
    system = None
    cCompiler = None
    includes = None
    libs = None

    def __init__(self):

        self.system = sys.platform
        self.cCompiler = None
        self.includes = None
        self.libs = None
        self.BuildConstants = BuildConstants.getInstance()

    def getIncludes(self):
        return self.includes

    def getLibs(self):
        return self.libs

    def getCompiler(self):
        return self.cCompiler

    def setIncludes(self):
        self.includes = BuildConstants.get(self.system, "includes")

    def setLibs(self):
        self.libs = BuildConstants.get(self.system, "libs")

    def setCompiler(self):
        if self.system == "darwin" or self.system == "linux":
            self.cCompiler = BuildConstants.get(self.system, "compiler")



    def getInstance(self):
        if Config.__instance is None:
            Config.__instance = Config()
        return Config.__instance
