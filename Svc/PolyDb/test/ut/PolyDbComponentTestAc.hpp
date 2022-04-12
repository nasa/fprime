/*
 * PolyDbComponentTestAc.hpp
 *
 *  Created on: Monday, 14 September 2015
 *  Author:     tcanham
 *
 */
#ifndef POLYDBCOMP_TESTER_HPP_
#define POLYDBCOMP_TESTER_HPP_
#include <FpConfig.hpp>
#include <Fw/Comp/PassiveComponentBase.hpp>


// type includes
#include <Fw/Types/PolyType.hpp>
#include <Fw/Time/Time.hpp>

// port includes
#include <Svc/PolyIf/PolyPortAc.hpp>

// serializable includes

namespace Svc {

    class PolyDbTesterComponentBase : public Fw::PassiveComponentBase  {

    public:
        
        void set_getValue_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputPolyPort *port);
        void set_setValue_OutputPort(NATIVE_INT_TYPE portNum, Svc::InputPolyPort *port);
    protected:
        // Only called by derived class
        PolyDbTesterComponentBase(const char* compName);

        virtual ~PolyDbTesterComponentBase(void);
        virtual void init(NATIVE_INT_TYPE instance = 0);
        // downcalls for input ports
        // upcalls for output ports
        void getValue_out(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val);
        void setValue_out(NATIVE_INT_TYPE portNum, U32 entry, MeasurementStatus &status, Fw::Time &time, Fw::PolyType &val);
        NATIVE_INT_TYPE getNum_getValue_OutputPorts(void);
        NATIVE_INT_TYPE getNum_setValue_OutputPorts(void);

        // check to see if output port is connected

        bool isConnected_getValue_OutputPort(NATIVE_INT_TYPE portNum);

        bool isConnected_setValue_OutputPort(NATIVE_INT_TYPE portNum);

             
    private:
        // output ports
        Svc::OutputPolyPort m_getValue_OutputPort[1];
        Svc::OutputPolyPort m_setValue_OutputPort[1];

        // input ports

        // calls for incoming ports


    };
};

#endif /* POLYDBCOMP_TEST_HPP_ */


