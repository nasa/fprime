// ======================================================================
// \title  TcDeframer.hpp
// \author thomas-bc
// \brief  hpp file for TcDeframer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_TcDeframer_HPP
#define Svc_Ccsds_TcDeframer_HPP

#include "Fw/DataStructures/ArrayMap.hpp"
#include "Os/Mutex.hpp"
#include "Svc/Ccsds/TcDeframer/TcDeframerComponentAc.hpp"
#include "TcDeframerConfig/FppConstantsAc.hpp"

namespace Svc {
namespace Ccsds {
class TcDeframer : public TcDeframerComponentBase {
    friend class TcDeframerTester;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct TcDeframer object
    TcDeframer(const char* const compName  //!< The component name
    );

    //! Destroy TcDeframer object
    ~TcDeframer();

    //! \brief Configure the TcDeframer to deframe only a specific VCID and spacecraft ID
    //!
    //! By default, the TcDeframer is configured with the spacecraft ID set in the config/ComCfg.fpp file,
    //! and deframes all incoming frames regardless of their VCID. Should project instantiate a TcDeframer
    //! with a different configuration, they can use this configure method to set the desired properties.
    //!
    //! \param vcId The virtual channel ID to accept (if acceptAllVcid is false)
    //! \param spacecraftId The spacecraft ID to accept
    //! \param acceptAllVcid If true, the deframer will accept all VCIDs. If false, it will only accept configured vcId
    //!
    void configure(U16 vcId, U16 spacecraftId, bool acceptAllVcid);

    //! \brief Configure reassembly of packets segmented across multiple TC frames (CCSDS 232.0-B-4 Section 4.1.3.3)
    //!
    //! Disabled by default. When enabled, each frame data field starts with a Segment Header, and allocate/deallocate
    //! must be connected before the first frame is received; see docs/sdd.md for the reassembly rules. Call once,
    //! before frames are processed (typically from the topology's configComponents).
    //!
    //! \param segmentHeaderPresent Whether TC frame data fields carry a Segment Header
    //! \param mapId The MAP ID to accept (0..63, asserted); segments carrying another MAP ID are dropped
    //! \param maxSpanningPacketSize Maximum size of a reassembled packet (> 0, asserted), also the size requested
    //!        from the allocator; an allocation smaller than this is treated as an allocation failure
    //!
    void configureSegmentation(bool segmentHeaderPresent, U8 mapId, FwSizeType maxSpanningPacketSize);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed data
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,     //!< The buffer
                              const ComCfg::FrameContext& context) override;

    //! Helper method to send an error notification if the errorNotify port is connected
    //! \param error The error to send
    void errorNotifyHelper(Svc::Ccsds::FrameError error);

    //! Process a validated TC frame data field that starts with a Segment Header
    //! \param data The frame data field (segment header | segment data); ownership closes in every branch
    //! \param frameContext The context the frame arrived with, used when returning the frame upstream
    //! \param dataContext The context to forward with deframed data
    void handleSegment(Fw::Buffer& data,
                       const ComCfg::FrameContext& frameContext,
                       const ComCfg::FrameContext& dataContext);

    //! Allocate the reassembly buffer and start a new spanning packet
    //! \return true on success, false if allocation failed (an event has been emitted)
    bool startSpanningPacket(const ComCfg::FrameContext& context);

    //! Append segment data to the spanning packet in progress
    //! \return true on success, false if the packet overflowed and was discarded (an event has been emitted)
    bool appendToSpanningPacket(const Fw::Buffer& data);

    //! Forward the completed spanning packet downstream, or drop it if the in-flight table is full
    void completeSpanningPacket();

    //! Record a reassembled packet as owned downstream
    //! \return true if a table slot was available, false if the in-flight table is full
    bool trackInFlight(const Fw::Buffer& allocated);

    //! Remove the tracked allocation backing a returned buffer, if the buffer is one of ours
    //! \param returned The buffer received on dataReturnIn
    //! \param allocated Set to the full allocation to deallocate when this returns true
    //! \return true if the returned buffer was a reassembled packet allocated by this component
    bool releaseInFlight(const Fw::Buffer& returned, Fw::Buffer& allocated);

    //! Emit an event and discard the spanning packet in progress, if any
    void abandonSpanningPacket();

    //! Deallocate the reassembly buffer and reset the spanning packet state
    void discardSpanningPacket();

    //! Reset the spanning packet state without touching the reassembly buffer's allocation
    void resetSpanningPacket();

    //! \return true if a spanning packet is in progress
    bool isSpanningPacketInProgress() const;

  private:
    U16 m_vcId = 0;               //!< The virtual channel ID this deframer is configured to handle
    U16 m_spacecraftId;           //!< The spacecraft ID this deframer is configured to handle
    bool m_acceptAllVcid = true;  //!< Flag to accept all VCIDs

    bool m_segmentHeaderPresent = false;     //!< Whether frame data fields carry a TC Segment Header
    U8 m_mapId = 0;                          //!< MAP ID accepted by this deframer
    FwSizeType m_maxSpanningPacketSize = 0;  //!< Maximum reassembled packet size
    Fw::Buffer m_spanningBuffer;             //!< Reassembly buffer, valid while a spanning packet is in progress
    FwSizeType m_spanningBytesReceived = 0;  //!< Bytes accumulated in the spanning packet in progress
    ComCfg::FrameContext m_spanningContext;  //!< Context of the first segment, forwarded with the reassembled packet
    //! Reassembled packets owned downstream, keyed by allocation address, awaiting return on dataReturnIn for
    //! deallocation. dataReturnIn is a sync port that may run on a downstream thread while dataIn holds the
    //! component guard, so the table has its own lock.
    Fw::ArrayMap<U8*, Fw::Buffer, TcDeframerCfg::MaxSpanningPacketsInFlight> m_inFlight;
    Os::Mutex m_inFlightLock;  //!< Guards m_inFlight
};
}  // namespace Ccsds
}  // namespace Svc

#endif
