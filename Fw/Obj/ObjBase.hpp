/**
 * \file
 * \author T. Canham
 * \brief Declarations for Fw::ObjBase and Fw::ObjRegistry
 *
 * \copyright
 * Copyright 2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef FW_OBJ_BASE_HPP
#define FW_OBJ_BASE_HPP

#include <FpConfig.hpp>

namespace Fw {

#if FW_OBJECT_REGISTRATION == 1
    class ObjRegistry; //!< forward declaration for object registry
#endif

    //! \class ObjBase
    //! \brief Brief class description
    //!
    //! This class is the base class of the ISF object class hierarchy.
    //! Depending on which features of the architecture are enabled, this class:
    //! 1) Stores an object name
    //! 2) Provides for object registration

    class ObjBase {
        public:
#if FW_OBJECT_NAMES == 1

            //!  \brief Returns the object's name
            //!
            //!  This function returns a pointer to the name of the object
            //!
            //!  \return object name
            const char* getObjName(); //!< Returns object name

            //!  \brief Sets the object name
            //!
            //!  This function takes the provided string and copies it
            //!  to the private buffer containing the name of the object.
            //!
            //!  \param name the name of the object
            void setObjName(const char* name); //!< sets object name
#if FW_OBJECT_TO_STRING == 1

            //!  \brief Returns a string representation of the object
            //!
            //!  A virtual function defined for all ObjBase types. It is
            //!  meant to be overridden by subclasses to return a description
            //!  of the object. The default implementation in this class
            //!  returns the name of the object.
            //!
            //!  \param str destination buffer where string description is placed
            //!  \param size destination buffer size (including terminator). String should be terminated
            virtual void toString(char* str, NATIVE_INT_TYPE size); //!< virtual method to get description of object
#endif // FW_OBJECT_TO_STRING
#endif // FW_OBJECT_NAMES

#if FW_OBJECT_REGISTRATION == 1

            //!  \brief static function to set object registry.
            //!
            //!  This function registers an instance of an object registry class (see below).
            //!  After the registration call is made, any subsequent calls to ObjBase::init()
            //!  will call the regObject() method on the registry.
            //!  **NOTE** The call may not be reentrant or thread-safe. The provided
            //!  SimObjRegistry is not reentrant.
            //!
            //!  \param reg Instance of registry to be stored.
            static void setObjRegistry(ObjRegistry* reg); //!< sets the object registry, if desired
#endif

        protected:

#if FW_OBJECT_NAMES == 1
            char m_objName[FW_OBJ_NAME_MAX_SIZE]; //!< stores object name
#endif

            //!  \brief ObjBase constructor
            //!
            //!  The constructor for the base class. Protected so it will only be called
            //!  by derived classes. Stores the object name (calls setObjName()).
            //!
            //!  \param name Object name
            ObjBase(const char* name);

            //!  \brief Destructor
            //!
            //!  ObjBase destructor. Empty.
            //!
            virtual ~ObjBase(); //!< Destructor. Should only be called by derived classes

            //!  \brief Object initializer
            //!
            //!  Initializes the object. For the base class, it calls
            //!  the object registry if registered by setObjRegistry()
            //!
            void init(); //!<initialization function that all objects need to implement. Allows static constructors.
        private:
#if FW_OBJECT_REGISTRATION == 1
            static ObjRegistry* s_objRegistry; //!< static pointer to object registry. Optionally populated.
#endif
    }; // ObjBase

#if FW_OBJECT_REGISTRATION == 1
    //! \class ObjRegistry
    //! \brief Base class declaration for object registry.
    //!
    //! More detailed class description (Markdown supported)
    //!
    class ObjRegistry {
        public:

            //!  \brief virtual function called when an object is registered
            //!
            //!  This pure virtual is called through a static ObjRegistry
            //!  pointer set by a call to ObjBase::setObjRegistry(). It is passed
            //!  a pointer to the instance of the object. What is done with that
            //!  pointer is dependent on the derived class implementation.
            //!  See SimpleObjRegistry for a basic example of a registry.
            //!
            //!  \param obj pointer to object
            virtual void regObject(ObjBase* obj)=0;

            //!  \brief Object registry destructor
            //!
            //!  Destructor. Base class is empty.
            //!
            virtual ~ObjRegistry();
    }; // ObjRegistry
#endif // FW_OBJECT_REGISTRATION
}
#endif // FW_OBJ_BASE_HPP
