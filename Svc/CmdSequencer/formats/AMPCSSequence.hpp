// ======================================================================
// \title  AMPCSSequence.hpp
// \author Bocchino
// \brief  AMPCSSequence interface
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_AMPCSSequence_HPP
#define Svc_AMPCSSequence_HPP

#include "Svc/CmdSequencer/CmdSequencerImpl.hpp"

namespace Svc {

  //! \class AMPCSSequence
  //! \brief A sequence in AMPCS format
  class AMPCSSequence :
    public CmdSequencerComponentImpl::Sequence
  {

    public:

      //! AMPCS sequence header
      struct SequenceHeader {

        struct Constants {

          typedef enum {
            //! The sequence header size in bytes
            SIZE = 4
          } t;

        };

        typedef U8 t[Constants::SIZE];

      };

      //! AMPCS sequence record
      struct Record {

        //! Time flag
        struct TimeFlag {

          //! The time flag type
          typedef enum {
            ABSOLUTE = 0x00,
            RELATIVE = 0xFF
          } t;

          //! The serial representation of a time flag
          struct Serial {

            typedef U8 t;

          };

        };

        //! Time
        struct Time {

          //! The type of the time field
          typedef U32 t;

        };

        //! Command length
        struct CmdLength {

          //! The type of the command length field
          typedef U16 t;

        };

        //! Opcode
        struct Opcode {

          //! The type of an AMPCS command opcode
          typedef U16 t;

        };

      };

    public:

      //! Construct an AMPCSSequence
      AMPCSSequence(
          CmdSequencerComponentImpl& component //!< The enclosing component
      );

    public:

      //! Load a sequence file
      //! \return Success or failure
      bool loadFile(
          const Fw::StringBase& fileName //!< The file name
      );

      //! Query whether the sequence has any more records
      //! \return Yes or no
      bool hasMoreRecords() const;

      //! Get the next record in the sequence.
      //! Asserts on failure
      void nextRecord(
          Sequence::Record& record //!< The returned record
      );

      //! Reset the sequence to the beginning.
      //! After calling this, hasMoreRecords should return true, unless
      //! the sequence has no records.
      void reset();

      //! Clear the sequence records.
      //! After calling this, hasMoreRecords should return false.
      void clear();

    PRIVATE:

      //! Read a CRC file
      //! \return Success or failure
      bool readCRCFile(
          Fw::CmdStringArg& crcFileName //!< The CRC file name
      );

      //! Read the CRC out of an open CRC file
      //! \return Success or failure
      bool readCRC();

      //! Deserialize the CRC
      //! \return Success or failure
      bool deserializeCRC();

      //! Get the aggregate size of the command records
      //! \return Success or failure
      bool getFileSize(
          const Fw::StringBase& seqFileName //!< The sequence file name
      );

      //! Read a sequence file
      //! \return Success or failure
      bool readSequenceFile(
          const Fw::StringBase& seqFileName //!< The sequence file name
      );

      //! Read an open sequence file
      //! \return Success or failure
      bool readOpenSequenceFile();

      //! Read the sequence header from the sequence file
      //! into the buffer
      //! \return Success or failure
      bool readSequenceHeader();

      //! Read records into buffer
      //! \return Success or failure
      bool readRecords();

      //! Validate the CRC
      //! \return Success or failure
      bool validateCRC();

      //! Deserialize a record from a buffer
      //! \return Serialize status
      Fw::SerializeStatus deserializeRecord(
          Sequence::Record& record //!< The record
      );

      //! Deserialize the time flag field and convert it into a record descriptor
      //! \return Serialize status
      Fw::SerializeStatus deserializeTimeFlag(
          Sequence::Record::Descriptor& descriptor //!< The descriptor
      );

      //! Deserialize the time field and convert it into a time tag
      //! \return Serialize status
      Fw::SerializeStatus deserializeTime(
          Fw::Time& timeTag //!< The time tag
      );

      //! Deserialize the command length field
      //! \return Serialize status
      Fw::SerializeStatus deserializeCmdLength(
          Record::CmdLength::t& cmdLength //!< The command length
      );

      //! Translate a serialized AMPCS command to a serialized F Prime command
      //! \return Serialize status
      Fw::SerializeStatus translateCommand(
          Fw::ComBuffer& comBuffer, //!< The com buffer
          const Record::CmdLength::t cmdLength //!< The command length
      );

      //! Validate the sequence records in the buffer
      //! \return Success or failure
      bool validateRecords();

    PRIVATE:

      //! The sequence header
      SequenceHeader::t m_sequenceHeader;

      //! The CRC values
      CmdSequencerComponentImpl::FPrimeSequence::CRC m_crc;

      //! The CRC file
      Os::File m_crcFile;

      //! The sequence file
      Os::File m_sequenceFile;

  };

}

#endif
