# This Makefile goes in each module, and allows building of an individual module library.
# It is expected that each developer will add targets of their own for building and running
# tests, for example.

# derive module name from directory

MODULE_DIR = docs/Architecture
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

IMG_PLANTUML := $(CURDIR)/ComponentTree.png

all: $(IMG_PLANTUML)

clean:
	rm $(IMG_PLANTUML)

%.png: %.puml
	java -jar $(shell cygpath -w -m $(BUILD_ROOT)/mk/tools/plantuml/plantuml.jar)  $(shell cygpath -w -m $<)

