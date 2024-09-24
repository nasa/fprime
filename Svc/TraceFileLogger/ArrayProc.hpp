// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef SVCARRAYPROC_HPP_
#define SVCARRAYPROC_HPP_

#include <Fw/Types/BasicTypes.hpp>

namespace Svc {

    //! \class ArrayProc
    //! \brief ArrayProc struct
    //!
    //! The object is used for supporting array functions. Making it a struct so all
    //! members are public, for ease of use in object composition.

    struct ArrayProc {

        //!  \brief Constructor
        //!
        ArrayProc();

        //!  \brief Destructor
        //!
        ~ArrayProc();

        // ----------------------------------------------------------------------
        // Member Functions
        // ----------------------------------------------------------------------

        //!  \brief Set pointer to the array 
        //!
        //!  \param array_ptr pointer to the array to be used 
        //!  \param array_size array size 
        //!  
        void set_array(U32 *array_ptr, U8 array_size); 

        //!  \brief Add an element to the array 
        //!
        //!  \param element value to be added to the array 
        //!
        //!  \return true if adding an element to the array was successful, false otherwise
        bool add_element(U32 element); 

        //!  \brief Delete element from the array and shift the rest of the array up 
        //!
        //!  \param element value to be deleted from the array 
        //!
        //!  \return true if deleting an element from the array was successful, false otherwise
        bool delete_element(U32 element);

        //!  \brief Search for an element in the array
        //!
        //!  \param element value to be searched in the array
        //!  \param arrayIndex returns the index where (if) the array element exists
        //!
        //!  \return true if the element is found in the array, false otherwise
        bool search_array(U32 element, U8 *index); 

        // ----------------------------------------------------------------------
        // Member Variables
        // ----------------------------------------------------------------------

        //Max size of the array
        U8 m_maxSize;

        //Current Array Size
        U8 m_currentSize;

        //Array
        U32 *m_storedArray;
    };

}
#endif /* SVCARRAYPROC_HPP_ */
