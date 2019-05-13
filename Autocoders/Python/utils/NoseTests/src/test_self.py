from .CmdBuilder import CmdBuilder
from .BuildConstants import BuildConstants

class Test_CmdBuilder:

    builder = None

    @classmethod
    def setup_class(cls):
        print("Setting Test_CmdBuilder")

        cls.builder = CmdBuilder.getInstance()
        cls.constants = BuildConstants()
    def test_cppGeneration_badXml(self):
        xmlFiles = ['comp.xml']
        cppFiles = self.builder.generateCppFiles(xmlFiles)

        assert cppFiles == None
    def test_cppGeneration_valid(self):
        xmlFiles = ['compAi.xml']
        cppFiles = self.builder.generateCppFiles(xmlFiles)
        assert cppFiles[0] == 'compAc.cpp'

    def test_isfgenGeneration(self):
        xmlFiles = ['compAi.xml']
        flags = "-b -C -P"

        appName = "test"
        self.builder.setAppPath(appName)
        self.builder.setXmlFiles(xmlFiles)

        isfgenCmds = self.builder.generateIsfgenCommands(flags)

        cmd = isfgenCmds[0]

        d = self.constants.getDict('envar')
        d['file'] = xmlFiles[0]
        d['app'] = appName
        properCmd = "{PYTHON_SRC} {BUILD_ROOT}/Autocoders/Python/bin/isfgen.py -b -C -P {BUILD_ROOT}/{app}/{file}".format(**d)

        print("properCmd: {}".format(properCmd))
        print("cmd: {}".format(cmd))

        assert cmd == properCmd

class Test_BuildConstants:

    Contants = None

    @classmethod
    def setup_class(cls):
        print("Setting up Class")
        cls.Constants = BuildConstants()

    def test_getLibPaths(self):

        fwLibs = ['libA']
        osLibs = []

        properPath = "{BUILD_ROOT}/Fw/libA/libBin".format(**self.Constants.getDict('envar'))

        libPaths = self.Constants._BuildConstants__getLibPaths(fwLibs, osLibs, "libBin")

        path = libPaths[0]


        print("properPath: {}".format(properPath))
        print("path: {}".format(path))
        assert properPath == libPaths[0]
    def test_getFwLibs(self):
        libs = ['libA']
        properFile = "FwlibA"
        f = self.Constants._BuildConstants__getFwLibs(libs)
        f = f[0]
        print("properLib: {}".format(properFile))
        print("lib: {}".format(f))
        assert f == properFile
