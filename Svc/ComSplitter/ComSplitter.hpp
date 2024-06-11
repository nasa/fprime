// ----------------------------------------------------------------------
//
// ComSplitter.hpp
//
// ----------------------------------------------------------------------

#ifndef COMSPLITTER_HPP
#define COMSPLITTER_HPP

#include <Svc/ComSplitter/ComSplitterComponentAc.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {

  class ComSplitter :
    public ComSplitterComponentBase
  {

      // ----------------------------------------------------------------------
      // Friend class for whitebox testing
      // ----------------------------------------------------------------------

      friend class ComSplitterComponentBaseFriend;

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

    public:

      ComSplitter(const char* compName);

      ~ComSplitter();

      void init(NATIVE_INT_TYPE instance);

      // ----------------------------------------------------------------------
      // Handler implementations
      // ----------------------------------------------------------------------

    private:

      void comIn_handler(
          NATIVE_INT_TYPE portNum,
          Fw::ComBuffer &data,
          U32 context
      );

    };

}

#endif
