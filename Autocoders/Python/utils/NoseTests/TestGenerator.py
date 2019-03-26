##Test Generator

import sys
import os
import re
import argparse
import logging
from fprime_ac.generators.templates.component import includes1ComponentTestCpp
from NoseTests.Template import test_template
from lxml import etree

def parseConfigFile(args):
    ## Get args
    verbose = args.v
    configFile = args.configFile
    if not os.path.exists(configFile):
        logging.info("No config file found")
        sys.exit(-1)


    ##Open file and read data
    configFilePath = os.path.join(os.getcwd(), configFile)
    logging.debug("configFilePath: %s" % configFilePath)
    f = open(configFilePath, 'r')

    TestModuleValues = doParse(f)

    ## Add Verbosity
    TestModuleValues['verbose'] = 'INFO'
    if verbose:
        TestModuleValues['verbose'] = 'DEBUG'

    logging.info("Verbose: %s" % TestModuleValues['verbose'])

    return TestModuleValues

def doParse(f):

    TestModuleValues = dict()
    #test_cases is a list of Test_Case objects
    TestModuleValues['test_cases'] = []
    ## Structure:
    #TestModuleValues['module_path']
    #TestModuleValues['test_cases'] = [ Test_Case, Test_Case,...  ]

    xml_parser = etree.XMLParser()
    element_tree = etree.parse(f, parser=xml_parser)

    test_module = element_tree.getroot()

    if 'path' in test_module.attrib:
        BUILD_ROOT = os.environ.get('BUILD_ROOT')
        module_path = test_module.attrib['path']

        path = BUILD_ROOT + module_path
        TestModuleValues['module_path'] = path
        logging.debug("module_path: %s" % path)
    else:
        logging.info("test_module needs to have path")
        sys.exit(-1)

    ## Traverse Tree
    for test_case in test_module:
        ptuples = []

        name = None
        if 'name' in test_case.attrib:
            name = test_case.attrib['name']
            logging.debug("test_case: %s" % name)
        else:
            logging.info("test_case must have name")
            sys.exit(-1)

        ## Get pexpect groups
        for p_group in test_case:
            ptuple = []
            for p in p_group:
                ptuple.append(p)

            p_expect = ptuple[0].text
            p_send = ptuple[1].text

            logging.debug("p_expect: %s" % p_expect)
            logging.debug("p_send: %s" % p_send)

            ptuples.append((p_expect, p_send))

        case = Test_Case(name, ptuples)
        ## Add to TestModule
        TestModuleValues['test_cases'].append(case)

    return TestModuleValues


## Simple attribute wrapper
class Test_Case(object):
    def __init__(self, name, ptuples):
        self.name = name
        self.ptuples = ptuples

def writeToTemplate(TestModuleValues):
    filename = TestModuleValues['module_path'].split('/')[-1] + '_nose_test.py'
    filepath = os.path.join(os.getcwd(), filename)
    logging.info("Generating TestModule: %s" % filename)

    fp = open(filepath, 'w')

    template = test_template.test_template()

    template.TestModuleValues = TestModuleValues
    template.log_level = TestModuleValues['verbose']

    fp.writelines(template.__str__())

def initParser():
    parser = argparse.ArgumentParser()
    parser.add_argument('configFile')
    parser.add_argument('-v', help="Turn on Verbosity", action='store_true')

    return parser


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    Parser = initParser()
    args = Parser.parse_args()

    #logging.debug("configFile: %s" % configFile)
    logging.debug("Parsing ConfigFile")
    TestModuleValues = parseConfigFile(args)

    logging.debug("Writing to template")
    writeToTemplate(TestModuleValues)




