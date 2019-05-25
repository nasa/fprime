/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTPARAMRECVIMPL_HPP_
#define TESTPARAMRECVIMPL_HPP_

#include <Autocoders/Python/test/param_tester/ParamTestComponentAc.hpp>

class TestParamSourceImpl: public Prm::ParamTesterComponentBase {
    public:
#if FW_OBJECT_NAMES == 1
        TestParamSourceImpl(const char* compName);
#else
        TestParamSourceImpl();
#endif
        virtual ~TestParamSourceImpl();
        void init(void);
        void setPrm(U32 val);
    protected:
        Fw::ParamValid paramGetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);
        void paramSetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);
        Fw::ParamBuffer m_prm;
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
