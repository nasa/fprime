// ======================================================================
// \title  GenericRepeaterComponentImpl.hpp
// \author joshuaa
// \brief  hpp file for GenericRepeater component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef GenericRepeater_HPP
#define GenericRepeater_HPP

#include "Svc/GenericRepeater/GenericRepeaterComponentAc.hpp"

namespace Svc {

class GenericRepeaterComponentImpl : public GenericRepeaterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object GenericRepeater
    //!
    GenericRepeaterComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Initialize object GenericRepeater
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object GenericRepeater
    //!
    ~GenericRepeaterComponentImpl();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for portIn
    //!
    void portIn_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                        Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
    );
};

}  // end namespace Svc

#endif
