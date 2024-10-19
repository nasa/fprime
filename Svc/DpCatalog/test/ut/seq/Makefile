
%.bin: %.seq ../../../../../Ref/build-artifacts/Linux/Ref/dict/RefTopologyDictionary.json
	fprime-seqgen --dictionary ../../../../../Ref/build-artifacts/Linux/Ref/dict/RefTopologyDictionary.json $< $@

all: $(patsubst %.seq, %.bin, $(wildcard *.seq))

install: all
	mkdir -p ../../../../../Ref/seq
	cp *.bin ../../../../../Ref/seq

clean:
	rm -f *.bin