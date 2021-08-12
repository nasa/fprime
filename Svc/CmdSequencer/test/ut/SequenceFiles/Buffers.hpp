// ====================================================================== 
// \title  Buffers.hpp
// \author Rob Bocchino
// \brief  Sequence file buffers
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

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
