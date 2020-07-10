The enum_xml, implgen, dictgen, testgen, and schematron UT's all are implemented for pytest.

They are not currently enabled within cmake, so to run them you must install pytest with a package installer and then run "pytest DIR_NAME" pointing to the directory that contains the pytest or pytests that you want to run.

Environment variables needed to run pytest UT's:
- BUILD_ROOT (ex. "export BUILD_ROOT=$FPRIME" where $FPRIME is root of fprime working repo)
- FPRIME_CORE_DIR (ex. "export FPRIME_CORE_DIR=$FPRIME")
- PYTHONPATH (ex. "export PYTHONPATH=$FPRIME/Autocoders/Python/src")