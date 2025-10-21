// ======================================================================
// \title  BComponentAc.hpp
// \author bocchino
// \brief  hpp file for B component base class
// ======================================================================

#ifndef FppTest_BComponentAc_HPP
#define FppTest_BComponentAc_HPP

#include "Fw/Comp/ActiveComponentBase.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "FppTest/topology/ports/handcode/DataPortAc.hpp"
#if !FW_DIRECT_PORT_CALLS
#include "Fw/Port/InputSerializePort.hpp"
#include "Fw/Port/OutputSerializePort.hpp"
#endif

namespace FppTest {

//! \class BComponentBase
//! \brief Auto-generated base for B component
class BComponentBase : public Fw::ActiveComponentBase {
    // ----------------------------------------------------------------------
    // Friend classes
    // ----------------------------------------------------------------------

    //! Friend class tester to support autocoded test harness
    friend class BTesterBase;
    //! Friend class tester implementation to support white-box testing
    friend class BTester;

  protected:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The number of ports
    static constexpr FwIndexType NUM_PORTS = 2;

    //! Constants for numbers of typed input ports
    static constexpr FwIndexType NUM_DATAIN_INPUT_PORTS = 1;

    //! Constants for numbers of typed output ports
    static constexpr FwIndexType NUM_DATAOUT_OUTPUT_PORTS = 1;

  public:
    // ----------------------------------------------------------------------
    // Component initialization
    // ----------------------------------------------------------------------

    //! Initialize BComponentBase object
    void init(FwSizeType queueDepth,        //!< The queue depth
              FwEnumStoreType instance = 0  //!< The instance number
    );

  protected:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BComponentBase object
    BComponentBase(const char* compName = ""  //!< The component name
    );

    //! Destroy BComponentBase object
    virtual ~BComponentBase();

#if !FW_DIRECT_PORT_CALLS

  public:
    // ----------------------------------------------------------------------
    // Getters for typed input ports
    // ----------------------------------------------------------------------

    //! Get typed input port at index
    //!
    //! \return dataIn[portNum]
    FppTest::InputDataPort* get_dataIn_InputPort(FwIndexType portNum  //!< The port number
    );

#endif

#if !FW_DIRECT_PORT_CALLS

  public:
    // ----------------------------------------------------------------------
    // Connect typed input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to dataOut[portNum]
    void set_dataOut_OutputPort(FwIndexType portNum,           //!< The port number
                                FppTest::InputDataPort* port  //!< The input port
    );

#endif

#if !FW_DIRECT_PORT_CALLS && FW_PORT_SERIALIZATION

  public:
    // ----------------------------------------------------------------------
    // Connect serial input ports to typed output ports
    // ----------------------------------------------------------------------

    //! Connect port to dataOut[portNum]
    void set_dataOut_OutputPort(FwIndexType portNum,          //!< The port number
                                Fw::InputSerializePort* port  //!< The port
    );

#endif

  protected:
    // ----------------------------------------------------------------------
    // Connection status queries for typed output ports
    // ----------------------------------------------------------------------

    //! Check whether port dataOut is connected
    //!
    //! \return Whether port dataOut is connected
    bool isConnected_dataOut_OutputPort(FwIndexType portNum  //!< The port number
    ) const;

  protected:
    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port dataIn
    virtual void dataIn_handler(FwIndexType portNum,  //!< The port number
                                U32 data              //!< The data
                                ) = 0;

#if FW_DIRECT_PORT_CALLS
  public:
#else
  protected:
#endif
    // ----------------------------------------------------------------------
    // Port handler base-class functions for typed input ports
    //
    // Call these functions directly to bypass the corresponding ports
    // ----------------------------------------------------------------------

    //! Handler base-class function for input port dataIn
    void dataIn_handlerBase(FwIndexType portNum,  //!< The port number
                            U32 data              //!< The data
    );

  protected:
    // ----------------------------------------------------------------------
    // Pre-message hooks for typed async input ports
    //
    // Each of these functions is invoked just before processing a message
    // on the corresponding port. By default, they do nothing. You can
    // override them to provide specific pre-message behavior.
    // ----------------------------------------------------------------------

    //! Pre-message hook for async input port dataIn
    virtual void dataIn_preMsgHook(FwIndexType portNum,  //!< The port number
                                   U32 data              //!< The data
    );

  protected:
    // ----------------------------------------------------------------------
    // Invocation functions for typed output ports
    // ----------------------------------------------------------------------

    //! Invoke output port dataOut
    void dataOut_out(FwIndexType portNum,  //!< The port number
                     U32 data              //!< The data
    );

  private:
    // ----------------------------------------------------------------------
    // Message dispatch functions
    // ----------------------------------------------------------------------

    //! Called in the message loop to dispatch a message from the queue
    virtual MsgDispatchStatus doDispatch();

#if !FW_DIRECT_PORT_CALLS

  private:
    // ----------------------------------------------------------------------
    // Calls for messages received on typed input ports
    // ----------------------------------------------------------------------

    //! Callback for port dataIn
    static void m_p_dataIn_in(Fw::PassiveComponentBase* callComp,  //!< The component instance
                              FwIndexType portNum,                 //!< The port number
                              U32 data                             //!< The data
    );

#endif

#if !FW_DIRECT_PORT_CALLS

  private:
    // ----------------------------------------------------------------------
    // Typed input ports
    // ----------------------------------------------------------------------

    //! Input port dataIn
    FppTest::InputDataPort m_dataIn_InputPort[NUM_DATAIN_INPUT_PORTS] = {};


#endif

#if !FW_DIRECT_PORT_CALLS

  private:
    // ----------------------------------------------------------------------
    // Typed output ports
    // ----------------------------------------------------------------------

    //! Output port dataOut
    FppTest::OutputDataPort m_dataOut_OutputPort[NUM_DATAOUT_OUTPUT_PORTS] = {};

#endif
};

}  // namespace FppTest

#endif
