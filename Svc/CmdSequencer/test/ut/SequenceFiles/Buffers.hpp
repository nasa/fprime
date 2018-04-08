// ====================================================================== 
// \title  Buffers.hpp
// \author Rob Bocchino
// \brief  Sequence file buffers
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

#ifndef Svc_SequenceFiles_Buffers_HPP
#define Svc_SequenceFiles_Buffers_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  namespace SequenceFiles {

    namespace Buffers {

      //! A file buffer
      class FileBuffer :
        public Fw::SerializeBufferBase
      {

        public:

          enum Constants {
            CAPACITY = 4096
          };

        public:

          NATIVE_UINT_TYPE getBuffCapacity(void) const;

          U8* getBuffAddr(void);

          const U8* getBuffAddr(void) const;

        private:

          U8 m_buff[CAPACITY];

      };

      //! Write a buffer to a file
      void write(
          const Fw::SerializeBufferBase& buffer, //!< The buffer
          const char* fileName //!< The file name
      );

    }

  }

}

#endif
