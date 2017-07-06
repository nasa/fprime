# Create sequence files using tinyseqgen.sh

# ----------------------------------------------------------------------
# Makefile 
# ----------------------------------------------------------------------

SEQ_TXT := $(wildcard *.seq)
SEQ_BIN := $(SEQ_TXT:%.seq=%.bin)

all: $(SEQ_BIN)

clean:
	$(RM) *.bin

%.bin: %.seq
	@echo "=== Compiling sequence $< ===" 	
	@../../../../Gse/bin/run_tinyseqgen.sh $<
	
print_time:
	@date -u +A"%Y-%jT%H:%M:%S%Z"
	@date +A"%Y-%jT%H:%M:%S%Z"
	