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
        TestParamSourceImpl(const char* compName);
        virtual ~TestParamSourceImpl();
        void init();
        void setPrm(U32 val);
    protected:
    private:
        Fw::ParamValid paramGetPort_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer &val);
        void paramSetPort_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer &val);
        Fw::ParamBuffer m_prm;
};

#endif /* TESTCOMMANDSOURCEIMPL_HPP_ */
