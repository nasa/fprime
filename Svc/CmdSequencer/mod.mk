# ---------------------------------------------------------------------- 
# mod.mk
# ---------------------------------------------------------------------- 

SRC = \
	CmdSequencerComponentAi.xml \
	CmdSequencerImpl.cpp \
	Events.cpp \
	FPrimeSequence.cpp \
        Sequence.cpp \
	formats/AMPCSSequence.cpp

HDR = \
	CmdSequencerImpl.hpp \
	formats/AMPCSSequence.hpp

SUBDIRS = test
