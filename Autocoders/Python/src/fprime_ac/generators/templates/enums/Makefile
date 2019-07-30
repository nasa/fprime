# ----------------------------------------------------------------------
# Makefile 
# ----------------------------------------------------------------------

TMPL := $(wildcard *.tmpl)
TEMPLATE_PY := $(TMPL:%.tmpl=%.py)

all: $(TEMPLATE_PY)

clean:
	$(RM) *.py.bak

%.py: %.tmpl
	$(CHEETAH_COMPILE) $<
