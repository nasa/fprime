#include <Fw/Logger/Logger.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>
#include <cstring>

#if FW_OBJECT_REGISTRATION == 1

namespace Fw {

    SimpleObjRegistry::SimpleObjRegistry() {
        ObjBase::setObjRegistry(this);
        this->m_numEntries = 0;
        // Initialize pointer array
        for (NATIVE_INT_TYPE entry = 0; entry < FW_OBJ_SIMPLE_REG_ENTRIES; entry++) {
            this->m_objPtrArray[entry] = nullptr;
        }
    }

    SimpleObjRegistry::~SimpleObjRegistry() {
        ObjBase::setObjRegistry(nullptr);
    }

    void SimpleObjRegistry::dump() {
        for (NATIVE_INT_TYPE obj = 0; obj < this->m_numEntries; obj++) {
#if FW_OBJECT_NAMES == 1
#if FW_OBJECT_TO_STRING == 1
            char objDump[FW_OBJ_SIMPLE_REG_BUFF_SIZE];
            this->m_objPtrArray[obj]->toString(objDump,sizeof(objDump));
            Fw::Logger::log("Entry: %d Ptr: %p Str: %s\n", obj,
                            this->m_objPtrArray[obj], objDump);
#else
            Fw::Logger::log("Entry: %d Ptr: %p Name: %s\n", obj,
                            this->m_objPtrArray[obj],
                            this->m_objPtrArray[obj]->getObjName());
#endif // FW_OBJECT_TO_STRING
#else
            Fw::Logger::log("Entry: %d Ptr: %p\n", obj, this->m_objPtrArray[obj]);
#endif
        }
    }

#if FW_OBJECT_NAMES == 1
    void SimpleObjRegistry::dump(const char* objName) {
        for (NATIVE_INT_TYPE obj = 0; obj < this->m_numEntries; obj++) {
            char objDump[FW_OBJ_SIMPLE_REG_BUFF_SIZE];
            if (strncmp(objName,this->m_objPtrArray[obj]->getObjName(),sizeof(objDump)) == 0) {
#if FW_OBJECT_TO_STRING == 1
                this->m_objPtrArray[obj]->toString(objDump,sizeof(objDump));
                Fw::Logger::log("Entry: %d Ptr: %p Str: %s\n", obj,
                                this->m_objPtrArray[obj], objDump);
#else
                Fw::Logger::log("Entry: %d Ptr: %p Name: %s\n",obj,
                                this->m_objPtrArray[obj],
                                this->m_objPtrArray[obj]->getObjName());
#endif
            }
        }
    }
#endif
    void SimpleObjRegistry::regObject(ObjBase* obj) {
        FW_ASSERT(this->m_numEntries < FW_OBJ_SIMPLE_REG_ENTRIES);
        this->m_objPtrArray[this->m_numEntries++] = obj;

    }

    void SimpleObjRegistry::clear() {
        this->m_numEntries = 0;
    }

}

#endif

