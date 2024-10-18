// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Svc/TraceFileLogger/ArrayProc.hpp>
#include <Fw/Types/Assert.hpp>
#include <limits>
#include <cstring>
#include <cstdio>
#include <Fw/Types/StringUtils.hpp>


namespace Svc {

    // ----------------------------------------------------------------------
    // Initialization/Exiting
    // ----------------------------------------------------------------------

    ArrayProc::ArrayProc() :
       m_maxIndex(10),m_currentIndex(0),m_storedArray(nullptr) 
    {
    }
    ArrayProc::~ArrayProc()
    {
    }

    // ----------------------------------------------------------------------
    // Member Functions
    // ----------------------------------------------------------------------

    void ArrayProc::set_array(U32 *array_ptr, U8 array_size)
    {
        FW_ASSERT(array_ptr != nullptr);
        m_storedArray = array_ptr;
        m_maxIndex = array_size;
    }
    
    bool ArrayProc::add_element(U32 element) {
        if (m_currentIndex < m_maxIndex) {
            this->m_storedArray[m_currentIndex] = element;
            this->m_currentIndex++;
            return true;
        }
        return false;
    }

    bool ArrayProc::delete_element(U32 element) {

        U8 elementIndex = 0;
        bool elementExists = this->search_array(element,&elementIndex);
        if (elementExists == false) {
            return false;
        }
        else {
            for(U8 i = elementIndex; i < this->m_currentIndex ; i++) {
                this->m_storedArray[i] = this->m_storedArray[i+1];
            }
            this->m_currentIndex--;
            return true;
        }
    }

    bool ArrayProc::search_array(U32 element, U8 *index) {
        for(U8 i = 0 ; i < m_currentIndex ; i++) {
            if(this->m_storedArray[i] == element) {
                if (index != nullptr) {
                    *index = i;
                }
                return true;
            }
        }
        return false;
    }
} // namespace Svc
