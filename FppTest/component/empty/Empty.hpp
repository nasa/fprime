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
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Empty
      //!
      Empty(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object Empty
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Empty
      //!
      ~Empty();


    };


#endif
