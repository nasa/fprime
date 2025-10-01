// ----------------------------------------------------------------------
//
// ComSplitter.hpp
//
// ----------------------------------------------------------------------

#ifndef COMSPLITTER_HPP
#define COMSPLITTER_HPP

#include <Fw/Types/Assert.hpp>
#include <Svc/ComSplitter/ComSplitterComponentAc.hpp>

namespace Svc {

class ComSplitter final : public ComSplitterComponentBase {
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

    // ----------------------------------------------------------------------
    // Handler implementations
    // ----------------------------------------------------------------------

  private:
    void comIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context);
};

}  // namespace Svc

#endif
