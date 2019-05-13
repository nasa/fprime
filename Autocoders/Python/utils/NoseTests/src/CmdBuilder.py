import sys
import os
from .BuildConstants import BuildConstants

class CmdBuilder(object):

    ## These stay constant for the duration of the test
    #  They are set during initalization
    __instance = None
    __cCompiler = None
    __includes = None
    __libPaths = None
    __libs = None
    __BUILD_ROOT = None
    __BuildConstants = None

    ## These vary per application test.
    #  They are set in the test module
    __appPath = None
    __xmlFiles = None
    __cppFiles = None
    __target = None


    def __init__(self, system):

        print("Building for %s" % system)

        self.__BuildConstants = BuildConstants()

        self.__BUILD_ROOT = self.__BuildConstants.getEntry('envar', 'BUILD_ROOT')
        self.__cCompiler = self.__getcCompiler(system)
        self.__includes = self.__getIncludes(system)
        self.__libPaths = self.__getLibPaths(system)
        self.__libs = self.__getLibs(system)

    def generateBuildCommands(self):
        if len(self.__cppFiles) == 0:
            print("Error: No cpp files are set.")
            exit()
        if self.__target == None:
            print("Error: Target not set")
            exit()

        cmdDict = {'oFile': None, 'cCompiler': self.__cCompiler, 'includes': self.__includes, 'appPath':"-I"+self.__appPath, 'cFile': None, 'target': self.__target}

        oFiles = []
        buildList = []
        ## Generate .o files and create compiler commands
        for f in self.__cppFiles:
            s = f.split('.')

            oFile = s[0] + ".o"
            oFiles.append(oFile)
            cmdDict['oFile'] = oFile
            cmdDict['cFile'] = f


            buildCmd = """{cCompiler} -Wall -m64 -o {oFile} -c {includes} {appPath} {cFile}""".format(**cmdDict)
            buildList.append(buildCmd)

        ## Link everything together
        buildCmd = """{cCompiler} -o {target} -Wall -m64 -v""".format(**cmdDict)
        for f in oFiles:
            buildCmd += " {} ".format(f)
        for L in self.__libPaths:
            buildCmd += " -L{} ".format(L)
        for l in self.__libs:
            buildCmd += " -l{} ".format(l)

        buildList.append(buildCmd)
        return buildList


    def generateIsfgenCommands(self, flags):
        if len(self.__xmlFiles) == 0:
            print("Error: No XML Files are set.")
            exit()

        isfgenCmds = []

        cmdDict  = self.__BuildConstants.getDict('envar')
        cmdDict['flags'] = flags
        for f in self.__xmlFiles:
            cmdDict['xmlFile'] = f
            cmd = """{PYTHON_SRC} {BUILD_ROOT}/Autocoders/Python/bin/isfgen.py {flags} {xmlFile}""".format(**cmdDict)
            isfgenCmds.append(cmd)
        return isfgenCmds


    def setAppPath(self, appPath):
        self.__appPath = os.path.join(self.__BUILD_ROOT, appPath)
    def setXmlFiles(self, xmlFiles):
       fileList = []
       for x in xmlFiles:
           fileList.append(os.path.join(self.__appPath, x))
       self.__xmlFiles = fileList
    def setCppFiles(self, cppFiles):
        for f in cppFiles:
            self.__cppFiles.append(os.path.join(self.__appPath, f))
    def setImplFiles(self, implFiles):
        for f in implFiles:
            self.__cppFiles.append(os.path.join(self.__appPath, f))
    def setTarget(self, target):
        fileList = []
        fileList.append(target + ".cpp")
        self.__cppFiles = fileList
        self.__target = target
    def __getcCompiler(self, system):
        return self.__BuildConstants.getEntry(system, 'cCompiler')
    def __getIncludes(self, system):
        return self.__BuildConstants.getEntry(system, 'includes')
    def __getLibPaths(self, system):
        return self.__BuildConstants.getEntry(system, 'libPaths')
    def __getLibs(self, system):
        return self.__BuildConstants.getEntry("generic", 'libs')


    def generateCppFiles(self, xmlFiles):
        cppFiles = []

        for f in xmlFiles:
            s = f.split('.')
            if s[0][-2:] != 'Ai': #Check if Xmlfiles are named properly
                print("FileName Error. Xmlfile must be of format __Ai.xml")
                return None
            d = s[0].split('Ai')
            cppFiles.append(d[0] + "Ac.cpp")
        return cppFiles

    def getInstance():
        if(CmdBuilder.__instance is None):
            system = sys.platform

            CmdBuilder.__instance = CmdBuilder(system)
        return CmdBuilder.__instance
    getInstance = staticmethod(getInstance)

