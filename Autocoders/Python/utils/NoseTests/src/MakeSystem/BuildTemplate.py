import subprocess
import CmdBuilder

# Setup Path and Build
def setup_module():

    ## Application Specific Variables
    xmlFiles = ["Comp1Ai.xml", "Comp2Ai.xml"]
    implFiles = ["Comp1Impl.cpp", "Comp2Impl.cpp"]
    appPath = "Autocoders/Python/test/app1"
    target = "Top"

    ## Configure Command Builder
    Builder = CmdBuilder.CmdBuilder.getInstance()
    Builder.setAppPath(appPath)
    cppFiles = Builder.generateCppFiles(xmlFiles)
    Builder.setTarget(target)
    Builder.setImplFiles(implFiles)
    Builder.setXmlFiles(xmlFiles)
    Builder.setCppFiles(cppFiles)


    ## Set isfgen flags
    flags = "-b"


    isfgenCmds = Builder.generateIsfgenCommands(flags)
    buildCmds = Builder.generateBuildCommands()


    for cmd in isfgenCmds:
        print(cmd)
    for cmd in buildCmds:
        print(cmd)

#    for cmd in isfgenCmds:
#        subprocess.call(cmd, shell=True)




if __name__ == "__main__":
    setup_module()
