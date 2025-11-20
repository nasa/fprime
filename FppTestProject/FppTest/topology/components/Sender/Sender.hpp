// ======================================================================
// \title  Sender.hpp
// \author tumbar
// \brief  hpp file for Sender component implementation class
// ======================================================================

#ifndef FppTest_Sender_HPP
#define FppTest_Sender_HPP

#include "FppTest/topology/components/Sender/SenderComponentAc.hpp"
#include "Fw/Types/SerialBuffer.hpp"

namespace FppTest {

class Sender final : public SenderComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Sender object
    Sender(const char* const compName  //!< The component name
    );

    //! Destroy Sender object
    ~Sender();

    // ----------------------------------------------------------------------
    // Test cases
    // ----------------------------------------------------------------------

    void testNoArgs(const TestDeploymentPort& portId);
    void testPrimitiveArgs(const TestDeploymentPort& portId);
    void testStringArgs(const TestDeploymentPort& portId);
    void testEnumArgs(const TestDeploymentPort& portId);
    void testArrayArgs(const TestDeploymentPort& portId);
    void testStructArgs(const TestDeploymentPort& portId);
    void testNoArgsReturn(const TestDeploymentPort& portId);
    void testPrimitiveReturn(const TestDeploymentPort& portId);
    void testStringReturn(const TestDeploymentPort& portId);
    void testStringAliasReturn(const TestDeploymentPort& portId);
    void testEnumReturn(const TestDeploymentPort& portId);
    void testArrayReturn(const TestDeploymentPort& portId);
    void testArrayStringAliasReturn(const TestDeploymentPort& portId);
    void testStructReturn(const TestDeploymentPort& portId);

  private:
    void replyIn_handlerImpl(FwIndexType portNum,
                             FwIndexType handlerPortNum,
                             const FppTest::TestDeploymentPort& portId,
                             const Fw::Buffer& inputData);
    void replyIn_handler(FwIndexType portNum,
                         FwIndexType handlerPortNum,
                         const FppTest::TestDeploymentPort& portId,
                         const Fw::Buffer& inputData) override;

    void done_internalInterfaceHandler() override;
    void wait();

    template <typename ArgTy>
    ArgTy initTestCase(FwIndexType portNum, const TestDeploymentPort& portId);

    Fw::SerialBuffer m_expected;
    U8 m_expectedData[1024];

    FwIndexType m_expectedPortNum;
    TestDeploymentPort m_expectedPortId;
};

}  // namespace FppTest

#endif
