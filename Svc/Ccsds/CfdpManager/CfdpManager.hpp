// ======================================================================
// \title  CfdpManager.hpp
// \author campuzan
// \brief  hpp file for CfdpManager component implementation class
// ======================================================================

#ifndef CCSDS_CFDPMANAGER_HPP
#define CCSDS_CFDPMANAGER_HPP

#include "Svc/Ccsds/CfdpManager/CfdpManagerComponentAc.hpp"
#include "Svc/Ccsds/CfdpManager/CfdpStatusEnumAc.hpp"
#include "Svc/Ccsds/CfdpManager/cf_logical_pdu.hpp"

namespace Svc {
namespace Ccsds {

class CfdpManager final : public CfdpManagerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------
    typedef struct CfdpPduBuffer
    {
      //!< This is the logical structure that is used to build a PDU
      CF_Logical_PduBuffer_t pdu;
      //!< This is where the PDU is encoded
      U8 data[CF_MAX_PDU_SIZE];
      //!< Flag if the buffer has already been sent
      bool inUse;
    } CfdpPduBuffer;
    #define CFDP_MANAGER_NUM_BUFFERS (80)

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CfdpManager object
    CfdpManager(const char* const compName  //!< The component name
    );

    //! Destroy CfdpManager object
    ~CfdpManager();

    //! Configure CFDP engine
    void configure(void);

  public:
  // ----------------------------------------------------------------------
  // Port calls that are invoked by the CFDP engine
  // These functions are analogous to the functions in cf_cfdp_sbintf.*
  // However these functions are not direct ports due to the architectural
  // differences between F' and cFE
  // ----------------------------------------------------------------------

  // Equivelent of CF_CFDP_MsgOutGet
  CfdpStatus::T getPduBuffer(CF_Logical_PduBuffer_t* pduPtr, U8* msgPtr, U8 channelNum, FwSizeType size);
  // Not sure there is an equivelent
  void returnPduBuffer(U8 channelNum, CF_Logical_PduBuffer_t *);
  // Equivelent of CF_CFDP_Send
  void sendPduBuffer(U8 channelNum, CF_Logical_PduBuffer_t * pdu, const U8* msgPtr);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for run1Hz
    //!
    //! Run port which must be invoked at 1 Hz in order to satify CFDP timer logic
    void run1Hz_handler(FwIndexType portNum,  //!< The port number
                        U32 context           //!< The call order
    ) override;

    void dataReturnIn_handler(
            FwIndexType portNum, //!< The port number
            Fw::Buffer& data,
            const ComCfg::FrameContext& context
    ) override;
                        
  private:
    // ----------------------------------------------------------------------
    // Handler implementations for commands
    // ----------------------------------------------------------------------

    //! Handler implementation for command TODO
    //!
    //! TODO
    void TODO_cmdHandler(FwOpcodeType opCode,  //!< The opcode
                         U32 cmdSeq            //!< The command sequence number
    ) override;

  private:
    // ----------------------------------------------------------------------
    // Buffer management helpers
    // These will probably be removed
    // ----------------------------------------------------------------------
    void returnBufferHelper(CF_Logical_PduBuffer_t * pdu);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------
    CfdpPduBuffer pduBuffers[CFDP_MANAGER_NUM_BUFFERS];

};

}  // namespace Ccsds
}  // namespace Svc

#endif // CCSDS_CFDPMANAGER_HPP
