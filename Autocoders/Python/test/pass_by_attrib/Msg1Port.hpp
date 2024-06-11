/*
 * Msg1Port.hpp
 *
 *  Created on: Monday, 09 September 2013
 *  Author:     reder
 *
 */
#ifndef MSG1PORT_HPP_
#define MSG1PORT_HPP_

#include <Fw/Cfg/FwConfig.hpp>
#include <Fw/Port/FwInputPortBase.hpp>
#include <Fw/Port/FwOutputPortBase.hpp>
#include <Fw/Comp/FwCompBase.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/FwSerializable.hpp>

#include <Fw/Types/FwStringType.hpp>


namespace Ports {

    class InputMsg1Port : public Fw::InputPortBase  {
    public:
        enum {
            SERIALIZED_SIZE = sizeof(U32) + sizeof(U32 *) + sizeof(U32)
        };
        typedef void (*CompFuncPtr)(Fw::ComponentBase* callComp, NATIVE_INT_TYPE portNum, U32 arg1, U32 *arg2, U32 &arg3);

        InputMsg1Port();
        void init();
        void addCallComp(Fw::ComponentBase* callComp, CompFuncPtr funcPtr);
        void invoke(U32 arg1, U32 *arg2, U32 &arg3);
    protected:
    private:
        CompFuncPtr m_func;
#if FW_PORT_SERIALIZATION == 1
        void invokeSerial(Fw::SerializeBufferBase &buffer);
#endif
};

    class OutputMsg1Port : public Fw::OutputPortBase {
      public:
        OutputMsg1Port();
        void init();
        void addCallPort(InputMsg1Port* callPort);
        void invoke(U32 arg1, U32 *arg2, U32 &arg3);
      protected:
      private:
        InputMsg1Port* m_port;
    };
};
#endif /* MSG1_HPP_ */

