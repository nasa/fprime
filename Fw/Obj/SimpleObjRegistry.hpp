/**
 * \file
 * \author T. Canham
 * \brief Class declaration for a simple object registry
 *
 * The simple object registry is meant to give a default implementation
 * and an example of an object registry. When the registry is instantiated,
 * it registers itself with the object base class static function
 * setObjRegistry(). Objects then register with the instance as they are
 * instantiated. The object registry can then list the objects in its
 * registry.
 *
 * \copyright
 * Copyright 2013-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */
#ifndef FW_OBJ_SIMPLE_OBJ_REGISTRY_HPP
#define FW_OBJ_SIMPLE_OBJ_REGISTRY_HPP

#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>

#if FW_OBJECT_REGISTRATION == 1

namespace Fw {

    class SimpleObjRegistry : public ObjRegistry {
        public:
            SimpleObjRegistry(); //!< constructor for registry
            ~SimpleObjRegistry(); //!< destructor for registry
            void dump(); //!< dump contents of registry
            void clear(); //!< clear registry entries
#if FW_OBJECT_NAMES == 1
            void dump(const char* objName); //!< dump a particular object
#endif
        private:
            void regObject(ObjBase* obj); //!< register an object with the registry
            ObjBase* m_objPtrArray[FW_OBJ_SIMPLE_REG_ENTRIES]; //!< array of objects
            NATIVE_INT_TYPE m_numEntries; //!< number of entries in the registry
    };

}

#endif // FW_OBJECT_REGISTRATION

#endif // FW_OBJ_SIMPLE_OBJ_REGISTRY_HPP
