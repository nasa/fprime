#!/bin/bash

while [[ $# -gt 0 ]]
do
        key="$1"

        case $key in
                -package|--package)
                        PACKAGE="$2"
                        shift # past argument
                        ;;
                -project|-project_name|--project|--project_name)
                        PROJECT="$2"
                        shift # past argument
                        ;;
                -h|--help)
                        HELP=YES
                        ;;
                -v|-verbose)
                        VERBOSE=YES
                        ;;
                *)
                        # unknown option
                        ;;
        esac
        shift
done

#echo PACKAGE = "$PACKAGE"
#echo PROJECT = "$PROJECT"
#echo HELP = "$HELP"
#echo VERBOSE = "$VERBOSE"

run="java -Xmx800M -XX:PermSize=8M -XX:MaxPermSize=270m -Dinstall.root=$INSTALLDIR -ea -Dfile.encoding=UTF-8 -classpath ${BUILD_ROOT}Autocoders/MagicDrawCompPlugin/bin:${INSTALLDIR}lib/*:${INSTALLDIR}lib/aspectj/*:${INSTALLDIR}lib/graphics/*:${INSTALLDIR}lib/webservice/*:${BUILD_ROOT}Autocoders/MagicDrawCompPlugin/lib/velocity-1.6.2-dep.jar:${BUILD_ROOT}Autocoders/MagicDrawCompPlugin/lib/commons-cli-1.3.1.jar:. gov.nasa.jpl.componentaction.IsfCommandLine"

if [ -n "$HELP" ]; then
        run="$run -h"
fi

if [ -n "$PROJECT" ]; then
        run="$run -project_name $PROJECT"
fi

if [ -n "$PACKAGE" ]; then
        run="$run -package $PACKAGE"
fi

if [ -n "$VERBOSE" ]; then
        run="$run -verbose"
fi

if [ -z "$INSTALLDIR" ]; then
        echo WARNING: Environmental variable INSTALLDIR is not defined
fi

if [ -z "$BUILD_ROOT" ]; then
        echo WARNING: Environmental variable BUILD_ROOT is not defined
fi

eval "$run"
