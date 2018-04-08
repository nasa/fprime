// ====================================================================== 
// \title  SequenceFiles.hpp
// \author Rob Bocchino
// \brief  Interface for F Prime sequence files
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef Svc_SequenceFiles_SequenceFiles_HPP
#define Svc_SequenceFiles_SequenceFiles_HPP

#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadCRCFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadDescriptorFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadTimeBaseFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/BadTimeContextFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/Buffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/DataAfterRecordsFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/EmptyFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/File.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/ImmediateEOSFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/ImmediateFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MissingCRCFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MissingFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/MixedFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/RelativeFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SizeFieldTooLargeFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/SizeFieldTooSmallFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/TooLargeFile.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/USecFieldTooShortFile.hpp"

#endif
