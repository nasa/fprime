#!/usr/bin/env python


import argparse
import os
import sys
import subprocess
import logging


## Designate test applications here
def generate_constants():
    """
    Applications are registered here.
    Returns a dictionary of test modules
    and their test cases.
    """
    ## Every application test suite will have these
    base_tests = dict()
    base_tests['Make_Test'] = "a_make_test"
    base_tests['Make_Ut_Test'] = "b_make_ut_test"

    supported_apps = dict()

    ## Define apps here
    ## All supported applications must have the base tests
    supported_apps['active_tester'] = base_tests.copy()
#    supported_apps['app1'] = base_tests.copy()
    supported_apps['app2'] = base_tests.copy()
#    supported_apps['cnt_only'] = base_tests.copy()


    ## Define test cases and their descriptions here
    ## Define 'test_module' as the module in the application
    ## Define the test type as the method name within the test module
    supported_apps['active_tester']['test_file'] = "active_tester_nose_test.py"
    supported_apps['active_tester']['Sync_Input_Test'] = "c_Sync_InputPortF32_test"
    supported_apps['active_tester']['Async_Input_Test'] = "d_Async_InputPortU32_test"

#    supported_apps['app1']['test_file'] = "app1_nose_test.py"
#    supported_apps['app1']['Port_Send_Test'] = "c_port_send_test"

    supported_apps['app2']['test_file'] = "app2_nose_test.py"
    supported_apps['app2']['Void_Send_Test'] = "c_void_send_test"
    supported_apps['app2']['Port_Send_Test'] = "d_port_send_test"

#    supported_apps['cnt_only']['test_file'] = "cnt_only_nose_test.py"
#    supported_apps['cnt_only']['Port_Send_Test'] = "c_port_send_test"


    return supported_apps

def generate_entry(key, supported_apps):
    """
    Generates an entry for
    the help text.
    """

    entry_template = """\n\n{app_name}""".format(app_name=key)
    entry_template += """\n----------"""
    for test_t in supported_apps[key]:
        if test_t == 'test_file':
            continue
        entry_template += """\n{test_type}""".format(test_type=test_t)
    return entry_template


def get_app_list():
    """
    Return a formated list
    of test modules and their
    test cases.
    """

    app_list = """--------- Supported Applications ---------"""


    supported_apps = generate_constants()
    ## Add app_name and test methods combination to app_list
    for key in supported_apps:
        entry = generate_entry(key, supported_apps)
        app_list += entry
    return app_list

def process_args(args):
    """
    Process user arguments.
    """

    # Application Dictionary
    supported_apps  = generate_constants()

    nose_call = """nosetests -v"""

    ## Run all tests if no applications are specified
    if len(args.applications) == 0:
        for key in supported_apps:
            logging.debug("app: %s" % key)
            nose_call += " "+key+" "
    else:
        for appTest in args.applications:
            nose_args = parse_app(appTest)
            logging.debug("nose_args: %s" % nose_args)
            nose_call += nose_args

    logging.debug('nose_call: %s' % nose_call)
    return nose_call

def generate_for_method(test_method):
    """
    @param test_method: What test case to run.
    Returns argument string for nose.
    """

    supported_apps = generate_constants()

    cmd_string = ""

    for app in supported_apps:

        ## Dictionary for nose arguments
        args_dict = dict()

        try:
            # Application might not support speficied test_method
            try:
                args_dict['test_method'] = supported_apps[app][test_method]
            except KeyError:
                continue

            args_dict['app_name'] = app
            args_dict['test_file'] = supported_apps[app]['test_file']

        except KeyError:
            print("Test Method not not found")
            print("Are you sure it is specified correctly in runNose.py?")
            raise RuntimeError

        nose_args = """ {app_name}/{test_file}:{test_method} """.format(**args_dict)

        cmd_string += nose_args

    return cmd_string

def parse_app(appTest):
    """
    @param appTest: User argument defining what module.case to run.
    """
    supported_apps = generate_constants()
    args_dict = dict()
    test_method = None
    nose_args = None

    app_test = appTest.split(".")
    app_or_test = app_test[0]

    # Add test method to nose args
    if "_Test" in app_or_test:
        test_method = app_or_test
        nose_args = generate_for_method(test_method)
        return nose_args

    # Add single application to nose args
    args_dict['app_name'] = app_or_test
    try:
        args_dict['test_file'] = supported_apps[app_or_test]['test_file']
    except KeyError:
        print("Invalid application name: %s" % app_or_test)
        raise RuntimeError


    ## Add application specific test method
    args_dict['test_method'] = ""
    if len(app_test) == 2:
        test_method = app_test[1]
        try:
            args_dict['test_method'] = supported_apps[app_or_test][test_method]
        except KeyError:
            print("Invalid test method name: %s" % test_method)
            raise RuntimeError


    nose_args = """ {app_name}/{test_file}:{test_method} """.format(**args_dict)

    print(nose_args)

    return nose_args

def runTestGen(testGenPath, verbose):
    ## So we can get back
    binPath = os.getcwd()

    supported_apps = generate_constants()
    # Change open all supported applications and run testGen
    for app in supported_apps:
        appPath = "{BUILD_ROOT}/Autocoders/Python/test/{APP_DIR}".format(BUILD_ROOT=os.environ.get('BUILD_ROOT'), APP_DIR=app)
        logging.debug("Entering: %s" % appPath)

        os.chdir(appPath)

        logging.debug("Calling TestGen")

        v = ''
        if verbose:
            v = '-v'
            logging.info("Test Modules will be generated with verbosity turned on")

        test_gen_cmd = 'python {TEST_GEN} {verbose} pexpect_config.xml'.format(TEST_GEN=testGenPath, verbose=v)
        logging.debug("Calling: %s" % test_gen_cmd)

        subprocess.call(test_gen_cmd, shell=True)

    os.chdir(binPath)

def cleanTestGen():
    binPath = os.getcwd()

    supported_apps = generate_constants()
    # Change open all supported applications and run testGen
    for app in supported_apps:
        appPath = "{BUILD_ROOT}/Autocoders/Python/test/{APP_DIR}".format(BUILD_ROOT=os.environ.get('BUILD_ROOT'), APP_DIR=app)
        logging.debug("Entering: %s" % appPath)

        os.chdir(appPath)

        test_file = supported_apps[app]['test_file']
        logging.debug("Removing TestModule: %s" % test_file)
        subprocess.call('rm {TEST_FILE}'.format(TEST_FILE=test_file), shell=True)

    os.chdir(binPath)


def run_nose(nose_call):
    """
    @param nose_call:
    Execute nose
    """
    # Change directory to Autocoders/Python/test
    os.chdir("{BUILD_ROOT}/Autocoders/Python/test".format(BUILD_ROOT=os.environ.get('BUILD_ROOT')))

    print("\n\n--------------- Calling NoseTest ---------------\n %s\n" % nose_call)
    print("---------------    Test List     ---------------")
    subprocess.call(nose_call, shell=True)

    # Reset directory
    os.chdir("{BUILD_ROOT}/Autocoders/Python/".format(BUILD_ROOT=os.environ.get('BUILD_ROOT')))


def init_parser():
    """
    Initalize parser and description.
    """
    app_list = get_app_list()
    des = """\
             Test Suite Utility
             ''''''''''''''''''

             README located in Autocoders/Python/utils/Nosetests

             Supported applications and their
             tests are specified below. To test
             all, run with no arguments. To run
             a specific application and/or test
             run with this format:

             Generic Ex:
             runNose.py app_name.test_method

             Specific Ex:
             runNose.py app1.Make_Test
             ----------------------------------

             Apps and their tests are accumulated.
             So the following arguments are valid:

             runNose.py app1 app2 active_tester.Sync_Input_Test active_tester.Async_Input_Test

             This will test everything in app1 and app2,
             but only Sync_Input_Test and Async_Input_Test in active_tester."""



    parser = argparse.ArgumentParser(description = des,
                                     formatter_class=argparse.RawDescriptionHelpFormatter,
                                     epilog=app_list)
    parser.add_argument('-v', help="Increase verbosity", action="store_true")
    parser.add_argument('applications', nargs='*', help="A list of applications to test.")


    return parser


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    ## Setup Paths
    BUILD_ROOT = os.environ.get('BUILD_ROOT')
    testGenPath = os.path.join(BUILD_ROOT, 'Autocoders/Python/utils/NoseTests/TestGenerator.py')

    Parser = init_parser()
    args = Parser.parse_args()

    ## Get Verbosity
    verbose = args.v

    logging.debug("Test Verbose: %s" % verbose)
    ## Generate TestModules
    runTestGen(testGenPath, verbose)

    try:
        nose_call = process_args(args)
    except RuntimeError:
        print("Aborting.")
        sys.exit()

    run_nose(nose_call)

    ## Remove generated test files
    cleanTestGen()


