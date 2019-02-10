#!/bin/sh -e

# ----------------------------------------------------------------------
# Makefile
# ----------------------------------------------------------------------

all: FpCompAuto.jar

include classes.mk

update_build:
	./gen_classes_mk > classes.mk

clean:
	rm -rf classes FpCompAuto.jar

FpCompAuto.jar: classes
	jar -cf $@ \
		-C classes gov/nasa/jpl/componentaction \
		-C classes gov/nasa/jpl/componentaction/isfxmlwriter \
		lib \
		-C lib bundles \
		-C lib/bundles javax.jmi_1.0.0.201803050637 \
		-C lib/bundles org.junit_4.11.0.v201303080030 \
		-C . TopologyAi.vm \
		-C . ComponentAi.vm \
		-C . PortAi.vm

install:
	@echo "Installing to $(MD_INSTALL_LOC)"
	mkdir -p $(MD_INSTALL_LOC)/plugins/gov.nasa.jpl.magicdraw.componentAutocoder
	cp FpCompAuto.jar $(MD_INSTALL_LOC)/plugins/gov.nasa.jpl.magicdraw.componentAutocoder
	cp plugin.xml $(MD_INSTALL_LOC)/plugins/gov.nasa.jpl.magicdraw.componentAutocoder
	cp -r lib $(MD_INSTALL_LOC)/plugins/gov.nasa.jpl.magicdraw.componentAutocoder
	
#install: FpCompAuto.jar
#	cp FpCompAuto.jar /Users/bocchino/Downloads/MagicDraw/plugins/gov.nasa.jpl.magicdraw.componentAutocoder/FpCompAuto.jar

.PHONY:
	update_build
