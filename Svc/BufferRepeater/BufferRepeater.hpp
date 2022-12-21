// ======================================================================
// \title  BufferRepeater.hpp
// \author lestarch
// \brief  hpp file for GenericRepeater component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef BufferRepeater_HPP
#define BufferRepeater_HPP

#include "Svc/BufferRepeater/BufferRepeaterComponentAc.hpp"

namespace Svc {

class BufferRepeater : public BufferRepeaterComponentBase {
  public:
    /**
     * Set of responses to failures to allocate a buffer when requested
     */
    enum BufferRepeaterFailureOption {
        NO_RESPONSE_ON_OUT_OF_MEMORY,       /*!< The component will continue regardless of allocation failures */
        WARNING_ON_OUT_OF_MEMORY,           /*!< The component will produce a warning on allocation failures */
        FATAL_ON_OUT_OF_MEMORY,             /*!< The component will produce a FATAL on allocation failures */
        NUM_BUFFER_REPEATER_FAILURE_OPTIONS /*!< Maximum value of this setting. Used to mark as uninitialized. */
    };
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object BufferRepeater
    //!
    BufferRepeater(const char* const compName /*!< The component name*/
    );

    //! Initialize object BufferRepeater
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object BufferRepeater
    //!
    ~BufferRepeater();

    /**
     * Set the response used when an allocation request fails to produce a buffer. By default this will assert.
     * @param allocation_failure_response: set response
     */
    void configure(BufferRepeaterFailureOption allocation_failure_response);

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    /**
     * Checks the allocation for viability and reports if it fails.
     * @param index: index of the port that needs this allocation
     * @param new_allocation: new allocation for a copy of the incoming buffer.
     * @param incoming_buffer: buffer that is to be cloned
     * @return true when the allocation is valid, false otherwise
     */
    bool check_allocation(FwIndexType index, const Fw::Buffer& new_allocation, const Fw::Buffer& incoming_buffer);

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for portIn
    //!
    void portIn_handler(NATIVE_INT_TYPE portNum, /*!< The port number*/
                        Fw::Buffer& Buffer       /*!< The serialization buffer*/
    );

    BufferRepeaterFailureOption m_allocation_failure_response;  //!< Local storage for configured response
};

}  // end namespace Svc

#endif
