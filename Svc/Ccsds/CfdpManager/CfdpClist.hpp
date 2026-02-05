// ======================================================================
// \title  CfdpClist.hpp
// \brief  CFDP circular list header file
//
// This file is a port of CFDP circular list from the following files
// from the NASA Core Flight System (cFS) CFDP (CF) Application, version 3.0.0,
// adapted for use within the F-Prime (F') framework:
// - cf_clist.h (CFDP circular list data structure definitions)
//
// ======================================================================
//
// NASA Docket No. GSC-18,447-1
//
// Copyright (c) 2019 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ======================================================================

#ifndef CFDP_CLIST_HPP
#define CFDP_CLIST_HPP

#include <Fw/Types/BasicTypes.hpp>
#include <cstddef>
#include <functional>

namespace Svc {
namespace Ccsds {

/**
 * @brief Traverse status for circular list operations
 */
enum CfdpCListTraverseStatus : U8
{
    CFDP_CLIST_TRAVERSE_CONTINUE = 0, /**< \brief Continue traversing the list */
    CFDP_CLIST_TRAVERSE_EXIT     = 1  /**< \brief Stop traversing the list */
};

/** \brief Constant indicating to continue traversal */
constexpr U8 CFDP_CLIST_CONT = CFDP_CLIST_TRAVERSE_CONTINUE;

/** \brief Constant indicating to stop traversal */
constexpr U8 CFDP_CLIST_EXIT = CFDP_CLIST_TRAVERSE_EXIT;

/**
 * Checks if the list traversal should continue
 */
static inline bool CfdpCListTraverseStatusIsContinue(CfdpCListTraverseStatus stat)
{
    return (stat == CFDP_CLIST_TRAVERSE_CONTINUE);
}

/**
 * @brief Circular linked list node structure
 */
struct CfdpCListNode
{
    struct CfdpCListNode *next; /**< \brief Pointer to next node */
    struct CfdpCListNode *prev; /**< \brief Pointer to previous node */
};

/**
 * @brief Obtains a pointer to the parent structure
 *
 * Given a pointer to a CfdpCListNode object which is known to be a member of a
 * larger container, this converts the pointer to that of the parent.
 */
template <typename Container, typename Member>
constexpr Container* container_of_cpp(Member* member_ptr,
                                      Member Container::*member)
{
    return reinterpret_cast<Container*>(
        reinterpret_cast<char*>(member_ptr) - reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<Container*>(0)->*member))
    );
}


/**
 * @brief Callback function type for use with CfdpCListTraverse()
 *
 * @param node    Current node being traversed
 * @param context Opaque pointer passed through from initial call
 *
 * @returns integer status code indicating whether to continue traversal
 * @retval  #CFDP_CLIST_CONT Indicates to continue traversing the list
 * @retval  #CFDP_CLIST_EXIT Indicates to stop traversing the list
 */
using CfdpCListFunc = CfdpCListTraverseStatus(*)(CfdpCListNode*, void*);

/**
 * @brief Modern callback type for list traversal
 *
 * Replaces CfdpCListFunc with a more flexible std::function-based callback.
 * The callback receives the node and an opaque context pointer.
 */
using CfdpCListTraverseCallback = std::function<CfdpCListTraverseStatus(CfdpCListNode*, void*)>;

/************************************************************************/
/** @brief Initialize a clist node.
 *
 * @param node  Pointer to node structure to be initialized
 */
void CfdpCListInitNode(CfdpCListNode *node);

/************************************************************************/
/** @brief Insert the given node into the front of a list.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CfdpCListInsertFront(CfdpCListNode **head, CfdpCListNode *node);

/************************************************************************/
/** @brief Insert the given node into the back of a list.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CfdpCListInsertBack(CfdpCListNode **head, CfdpCListNode *node);

/************************************************************************/
/** @brief Remove the given node from the list.
 *
 * @param head  Pointer to head of list to remove from
 * @param node  Pointer to node to remove
 */
void CfdpCListRemove(CfdpCListNode **head, CfdpCListNode *node);

/************************************************************************/
/** @brief Remove the first node from a list and return it.
 *
 * @param head  Pointer to head of list to remove from
 *
 * @returns The first node (now removed) in the list
 * @retval  NULL if list was empty.
 */
CfdpCListNode *CfdpCListPop(CfdpCListNode **head);

/************************************************************************/
/** @brief Insert the given node into the last after the given start node.
 *
 * @param head  Pointer to head of list to remove from
 * @param start Pointer to node to insert
 * @param after Pointer to position to insert after
 */
void CfdpCListInsertAfter(CfdpCListNode **head, CfdpCListNode *start, CfdpCListNode *after);

/************************************************************************/
/** @brief Traverse the entire list, calling the given function on all nodes.
 *
 * @note on traversal it's ok to delete the current node, but do not delete
 * other nodes in the same list!!
 *
 * @param start   List to traverse (first node)
 * @param fn      Callback function to invoke for each node
 * @param context Opaque pointer to pass to callback
 */
void CfdpCListTraverse(CfdpCListNode *start, CfdpCListFunc fn, void *context);

/************************************************************************/
/** @brief Traverse the entire list, calling the given function on all nodes (modern C++ version).
 *
 * @note on traversal it's ok to delete the current node, but do not delete
 * other nodes in the same list!!
 *
 * @param start    List to traverse (first node)
 * @param callback Callback function to invoke for each node
 * @param context  Opaque pointer to pass to callback
 */
void CfdpCListTraverse(CfdpCListNode *start, const CfdpCListTraverseCallback& callback, void *context);

/************************************************************************/
/** @brief Reverse list traversal, starting from end, calling given function on all nodes.
 *
 * @note traverse_R will work backwards from the parameter's prev, and end on param
 *
 * @param end     List to traverse (last node)
 * @param fn      Callback function to invoke for each node
 * @param context Opaque pointer to pass to callback
 */
void CfdpCListTraverseR(CfdpCListNode *end, CfdpCListFunc fn, void *context);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_CLIST_HPP */
