/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef TESTPARAMRECVIMPL_HPP_
#define TESTPARAMRECVIMPL_HPP_

#include <Autocoders/Python/test/param_tester/ParamTestComponentAc.hpp>
#include <Fw/Prm/PrmString.hpp>

class TestParamSourceImpl: public Prm::ParamTesterComponentBase {
    public:
        TestParamSourceImpl(const char* compName);
        virtual ~TestParamSourceImpl();
        void init();
        void setPrm(Fw::ParamString& val);
    protected:
        Fw::ParamValid paramGetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);
        void paramSetPort_handler(NATIVE_INT_TYPE portNum, FwPrmIdType id, Fw::ParamBuffer &val);
    private:
        Fw::ParamBuffer m_prm;
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
