// ======================================================================
// \title  Empty.hpp
// \author tiffany
// \brief  hpp file for Empty component implementation class
// ======================================================================

#ifndef Empty_HPP
#define Empty_HPP

#include "FppTest/component/empty/EmptyComponentAc.hpp"

class Empty :
  public EmptyComponentBase
{

  public:

    // ----------------------------------------------------------------------
    // Component construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct Empty object
    Empty(
        const char* const compName //!< The component name
    );

    //! Initialize Empty object
    void init(
        NATIVE_INT_TYPE instance = 0 //!< The instance number
    );

    //! Destroy Empty object
    ~Empty();

};

#endif
