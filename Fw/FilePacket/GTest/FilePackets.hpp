// ====================================================================== 
// \title  Fw/FilePacket/GTest/FilePackets.hpp
// \author bocchino
// \brief  hpp file for File Packet testing utilities
//
// \copyright
// Copyright (C) 2016 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef Fw_GTest_FilePackets_HPP
#define Fw_GTest_FilePackets_HPP

#include <gtest/gtest.h>

#include <Fw/FilePacket/FilePacket.hpp>

namespace Fw {

  namespace GTest {

    //! Utilities for testing File Packet operations
    //!
    namespace FilePackets {

      namespace PathName {

        //! Compare two path names
        void compare(
            const FilePacket::PathName& expected,
            const FilePacket::PathName& actual
        );

      }

      namespace Header {

        //! Compare two file packet headers
        void compare(
            const FilePacket::Header& expected,
            const FilePacket::Header& actual
        );

      }

      namespace StartPacket {

        //! Compare two start packets
        void compare(
            const FilePacket::StartPacket& expected,
            const FilePacket::StartPacket& actual
        );

      }

      namespace DataPacket {

        //! Compare two data packets
        void compare(
            const FilePacket::DataPacket& expected,
            const FilePacket::DataPacket& actual
        );

      }

      namespace EndPacket {

        //! Compare two end packets
        void compare(
            const FilePacket::EndPacket& expected,
            const FilePacket::EndPacket& actual
        );

      }

      namespace CancelPacket {

        //! Compare two cancel packets
        void compare(
            const FilePacket::CancelPacket& expected,
            const FilePacket::CancelPacket& actual
        );

      }

    }

  }

}

#endif
