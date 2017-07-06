TMPL := $(wildcard *.tmpl)
TEMPLATE_PY := $(TMPL:%.tmpl=%.py)

all: $(TEMPLATE_PY)

clean:
	$(RM) *.py.bak $(TEMPLATE_PY) *.pyc

%.py: %.tmpl
	$(CHEETAH_COMPILE) $<
