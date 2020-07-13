/*
 * Cmd.hpp
 *
 *  Created on: Sep 10, 2012
 *      Author: ppandian 
 */

/*
 * Description:
 * This object contains the ParamBuffer type, used for storing parameters
 */
#ifndef FW_PRM_BUFFER_HPP
#define FW_PRM_BUFFER_HPP

#include <FpConfig.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Cfg/SerIds.hpp>

namespace Fw {

    class ParamBuffer : public SerializeBufferBase {
        public:

            enum {
                SERIALIZED_TYPE_ID = FW_TYPEID_PRM_BUFF,
                SERIALIZED_SIZE = FW_PARAM_BUFFER_MAX_SIZE + sizeof(FwBuffSizeType)
            };

            ParamBuffer(const U8 *args, NATIVE_UINT_TYPE size);
            ParamBuffer();
            ParamBuffer(const ParamBuffer& other);
            virtual ~ParamBuffer();
            const ParamBuffer& operator=(const ParamBuffer& other);

            NATIVE_UINT_TYPE getBuffCapacity(void) const; // !< returns capacity, not current size, of buffer
            U8* getBuffAddr(void);
            const U8* getBuffAddr(void) const;

        private:
            U8 m_data[FW_PARAM_BUFFER_MAX_SIZE]; // command argument buffer
    };

}

#endif
