/*
 * PosixTime.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef POSIX_TIME_HPP_
#define POSIX_TIME_HPP_

#include <Svc/PosixTime/PosixTimeComponentAc.hpp>

namespace Svc {

class PosixTime final : public PosixTimeComponentBase {
  public:
    explicit PosixTime(const char* compName);
    virtual ~PosixTime();
    void setTimeContext(FwTimeContextStoreType timeContext);

  protected:
    void timeGetPort_handler(FwIndexType portNum, /*!< The port number*/
                             Fw::Time& time       /*!< The U32 cmd argument*/
    );

  private:
    FwTimeContextStoreType m_timeContext;
};

}  // namespace Svc

#endif /* POSIX_TIME_HPP_ */
