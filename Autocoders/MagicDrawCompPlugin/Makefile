#!/bin/sh -e

# ----------------------------------------------------------------------
# Makefile
# ----------------------------------------------------------------------

all: plugin.jar

include classes.mk

update_build:
	./gen_classes_mk > classes.mk

clean:
	rm -rf classes plugin.jar

plugin.jar: classes
	jar -cf $@ \
		-C classes gov/nasa/jpl/componentaction \
		-C classes gov/nasa/jpl/componentaction/isfxmlwriter \
		lib \
		-C lib bundles \
		-C lib/bundles javax.jmi_1.0.0.201803050637 \
		-C lib/bundles org.junit_4.11.0.v201303080030

install: plugin.jar
	cp plugin.jar /Users/bocchino/Downloads/MagicDraw/plugins/gov.nasa.jpl.magicdraw.componentAutocoder/plugin.jar

.PHONY:
	update_build
