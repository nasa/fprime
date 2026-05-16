#ifndef REF_BLOCK_DRIVER_IMPL_HPP
#define REF_BLOCK_DRIVER_IMPL_HPP

#include <Ref/BlockDriver/BlockDriverComponentAc.hpp>

namespace Ref {

class BlockDriver final : public BlockDriverComponentBase {
  public:
    // Only called by derived class
    BlockDriver(const char* compName);

    ~BlockDriver();

  private:
    // downcalls for input ports
    void BufferIn_handler(FwIndexType portNum, Drv::DataBuffer& buffer);
    void Sched_handler(FwIndexType portNum, U32 context);
    //! Handler implementation for PingIn
    //!
    void PingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
    );

    // cycle count
    U32 m_cycles;
};
}  // namespace Ref

#endif
