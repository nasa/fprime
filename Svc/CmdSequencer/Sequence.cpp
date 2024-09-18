// ======================================================================
// \title  Sequence.cpp
// \author Bocchino/Canham
// \brief  Implementation file for CmdSequencer::Sequence
//
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <Fw/Types/Assert.hpp>
#include <Svc/CmdSequencer/CmdSequencerImpl.hpp>

namespace Svc {

    CmdSequencerComponentImpl::Sequence ::
      Sequence(CmdSequencerComponentImpl& component) :
        m_component(component),
        m_events(*this),
        m_allocatorId(0)
    {

    }

    CmdSequencerComponentImpl::Sequence ::
      ~Sequence()
    {

    }

    CmdSequencerComponentImpl::Sequence::Header ::
      Header() :
        m_fileSize(0),
        m_numRecords(0),
        m_timeBase(TB_DONT_CARE),
        m_timeContext(FW_CONTEXT_DONT_CARE)
    {

    }

    bool CmdSequencerComponentImpl::Sequence::Header ::
      validateTime(CmdSequencerComponentImpl& component)
    {
        Fw::Time validTime = component.getTime();
        Events& events = component.m_sequence->m_events;
        // Time base
        const TimeBase validTimeBase = validTime.getTimeBase();
        if (
            (this->m_timeBase != validTimeBase) and
            (this->m_timeBase != TB_DONT_CARE)
        ) {
            events.timeBaseMismatch(
                validTimeBase,
                this->m_timeBase
            );
            return false;
        }
        // Time context
        const FwTimeContextStoreType validContext = validTime.getContext();
        if (
            (this->m_timeContext != validContext) and
            (this->m_timeContext != FW_CONTEXT_DONT_CARE)
        ) {
            events.timeContextMismatch(
                validContext,
                this->m_timeContext
            );
            return false;
        }
        // Canonicalize time
        this->m_timeBase = validTimeBase;
        this->m_timeContext = validContext;
        return true;
    }

    void CmdSequencerComponentImpl::Sequence ::
      allocateBuffer(
          NATIVE_INT_TYPE identifier,
          Fw::MemAllocator& allocator,
          NATIVE_UINT_TYPE bytes
      )
    {
        // has to be at least as big as a header
        FW_ASSERT(bytes >= Sequence::Header::SERIALIZED_SIZE);
        bool recoverable;
        this->m_allocatorId = identifier;
        this->m_buffer.setExtBuffer(
            static_cast<U8*>(allocator.allocate(static_cast<NATIVE_UINT_TYPE>(identifier),bytes,recoverable)),
            bytes
        );
    }

    void CmdSequencerComponentImpl::Sequence ::
      deallocateBuffer(Fw::MemAllocator& allocator)
    {
        allocator.deallocate(
            static_cast<NATIVE_UINT_TYPE>(this->m_allocatorId),
            this->m_buffer.getBuffAddr()
        );
        this->m_buffer.clear();
    }

    const CmdSequencerComponentImpl::Sequence::Header&
      CmdSequencerComponentImpl::Sequence ::
        getHeader() const
    {
      return this->m_header;
    }

    void CmdSequencerComponentImpl::Sequence ::
      setFileName(const Fw::StringBase& fileName)
    {
        this->m_fileName = fileName;
        this->m_logFileName = fileName;
        this->m_stringFileName = fileName;
    }

    Fw::CmdStringArg& CmdSequencerComponentImpl::Sequence ::
      getFileName()
    {
        return this->m_fileName;
    }

    Fw::LogStringArg& CmdSequencerComponentImpl::Sequence ::
      getLogFileName()
    {
        return this->m_logFileName;
    }

    Fw::String& CmdSequencerComponentImpl::Sequence ::
      getStringFileName()
    {
        return this->m_stringFileName;
    }

}

