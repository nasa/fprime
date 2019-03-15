import os
import sys
from os import path
class BuildConstants(object):

    __constants = None
    __osConstants = None

    def __init__(self):
        self.__constants = dict()
        self.__setConstants()

    def getEntry(self, d, constant):
        return self.__constants[d][constant]

    def getDict(self, d):
        return self.__constants[d].copy()

    def __setConstants(self):

        ## ISF Libraries
        fwLibs = ["Comp", "Cmd", "Obj", "Types", "Port"]
        osLibs = ["Os"]

        ## Enviornmental Constants
        self.__constants['envar'] = dict()
        self.__constants['envar']['BUILD_ROOT'] = os.environ['BUILD_ROOT']
        self.__constants['envar']['PYTHON_SRC'] = sys.executable

        ## System Independent Constants
        self.__constants['generic'] = dict()
        self.__constants['generic']['libs'] = self.__getFwLibs(fwLibs) + osLibs

        ## Linux 2 Setup
        linux2Bin = "linux-linux-x86-debug-gnu-4.7.2-bin"
        self.__constants['linux2'] = dict()
        self.__constants['linux2']['includes'] = "-I{BUILD_ROOT}/Fw/Types/Linux -I{BUILD_ROOT}".format(**self.__constants['envar'])
        self.__constants['linux2']['cCompiler'] = "/usr/bin/g++"
        self.__constants['linux2']['libPaths'] = self.__getLibPaths(fwLibs, osLibs, linux2Bin)

        ## Darwin Setup
        darwinBin = "darwin-darwin-x86-debug-llvm-5.1-bin"
        self.__constants['darwin'] = dict()
        self.__constants['darwin']['includes'] = self.__constants['linux2']['includes']
        self.__constants['darwin']['cCompiler'] = "/usr/bin/g++"
        self.__constants['darwin']['libPaths'] = self.__getLibPaths(fwLibs, osLibs, darwinBin)


    def __getLibPaths(self, fwLibs, osLibs, libBin):
        libPaths = []

        for lib in fwLibs:
            rootPath = os.environ['BUILD_ROOT']
            rootPath += path.sep + "Fw" + path.sep + lib + path.sep + libBin
            libPaths.append(rootPath)

        for lib in osLibs:
            rootPath = os.environ['BUILD_ROOT']
            rootPath += path.sep + lib + path.sep +  libBin
            libPaths.append(rootPath)

        ## Implement for Svc support
        # for lib in svcLibs:
        #     pass

        return libPaths

    ## Add Fw to component name
    def __getFwLibs(self, unprocessedLibs):
        processedLibs = []
        for L in unprocessedLibs:
            processedLibs.append("Fw"+L)
        return processedLibs

