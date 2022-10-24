/**
 * \file
 * \author T. Canham
 * \brief Declares ISF string base class
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#ifndef FW_STRING_TYPE_HPP
#define FW_STRING_TYPE_HPP

#include <FpConfig.hpp>
#include <Fw/Types/Serializable.hpp>
#ifdef BUILD_UT
#include <iostream>
#endif

namespace Fw {
    class StringBase : public Serializable {
        public:
            virtual const CHAR* toChar() const = 0; //<! Convert to a C-style char*
            virtual NATIVE_UINT_TYPE getCapacity() const = 0; //!< return size of buffer
            NATIVE_UINT_TYPE length() const;  //!< Get length of string

            const CHAR* operator+=(const CHAR* src); //!< Concatenate a CHAR*
            const StringBase& operator+=(const StringBase& src); //!< Concatenate a StringBase
            bool operator==(const StringBase& other) const; //!< Check for equality with StringBase
            bool operator==(const CHAR* other) const; //!< Check for equality with CHAR*
            bool operator!=(const StringBase& other) const; //!< Inequality with StringBase
            bool operator!=(const CHAR* other) const; //!< Inequality with CHAR*
            StringBase& operator=(const CHAR* src); //!< Assign CHAR*
            StringBase& operator=(const StringBase& src); //!< Assign another StringBase

            void format(const CHAR* formatString, ...); //!< write formatted string to buffer

            virtual SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialization function
            virtual SerializeStatus serialize(SerializeBufferBase& buffer, NATIVE_UINT_TYPE maxLen) const; //!< serialization function
            virtual SerializeStatus deserialize(SerializeBufferBase& buffer); //!< deserialization function

#ifdef BUILD_UT
            // to support GoogleTest framework in unit tests
            friend std::ostream& operator<<(std::ostream& os, const StringBase& str);
#endif
#if FW_SERIALIZABLE_TO_STRING || BUILD_UT
            void toString(StringBase& text) const; //!< write string with contents
#endif

        protected:
            StringBase();
            virtual ~StringBase();

            void appendBuff(const CHAR* buff, NATIVE_UINT_TYPE size);

        private:
            // A no-implementation copy constructor here will prevent the default copy constructor from being called
            // accidentally, and without an implementation it will create an error for the developer instead.
            StringBase(const StringBase &src); //!< constructor with buffer as source
    };

}

#endif
