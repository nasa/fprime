// ======================================================================
// \title  Empty.cpp
// \author tiffany
// \brief  cpp file for Empty component implementation class
// ======================================================================


#include "Empty.hpp"
#include <FpConfig.hpp>


// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

Empty ::
  Empty(
      const char *const compName
  ) : EmptyComponentBase(compName)
{

}

void Empty ::
  init(
      NATIVE_INT_TYPE instance
  )
{
  EmptyComponentBase::init(instance);
}

Empty ::
  ~Empty()
{

}

