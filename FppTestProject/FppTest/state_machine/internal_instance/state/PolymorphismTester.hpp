// ======================================================================
// \title  PolymorphismTester.hpp
// \author bocchino
// \brief  hpp file for PolymorphismTester component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_Polymorphism_HPP
#define FppTest_SmInstanceState_Polymorphism_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/PolymorphismComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class PolymorphismTester : public PolymorphismComponentBase {
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The queue depth
    static constexpr FwSizeType queueDepth = 10;

    //! The instance ID
    static constexpr FwEnumStoreType instanceId = 0;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct PolymorphismTester object
    PolymorphismTester(const char* const compName  //!< The component name
    );

    //! Destroy PolymorphismTester object
    ~PolymorphismTester();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_Polymorphism
    using SmState_Polymorphism = FppTest_SmState_Polymorphism;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test polymorphic transition in S2
    void testS2_poly();

    //! Test transition from S2 to S3
    void testS2_to_S3();

    //! Test polymorphic transition in S3
    void testS3_poly();
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
