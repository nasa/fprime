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

#include <cstddef>
#include <functional>
#include <stddef.h>
#include <stdbool.h>

namespace Svc {
namespace Ccsds {

typedef enum
{
    CF_CListTraverse_Status_CONTINUE = 0,
    CF_CListTraverse_Status_EXIT     = 1
} CF_CListTraverse_Status_t;

#define CF_CLIST_CONT CF_CListTraverse_Status_CONTINUE /**< \brief Constant indicating to continue traversal */
#define CF_CLIST_EXIT CF_CListTraverse_Status_EXIT     /**< \brief Constant indicating to stop traversal */

/**
 * Checks if the list traversal should continue
 */
static inline bool CF_CListTraverse_Status_IS_CONTINUE(CF_CListTraverse_Status_t stat)
{
    return (stat == CF_CListTraverse_Status_CONTINUE);
}

/**
 * @brief Node link structure
 */
struct CF_CListNode
{
    struct CF_CListNode *next;
    struct CF_CListNode *prev;
};

/**
 * @brief Circular linked list node links
 */
typedef struct CF_CListNode CF_CListNode_t;

/**
 * @brief Obtains a pointer to the parent structure
 *
 * Given a pointer to a CF_CListNode_t object which is known to be a member of a
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
 * @brief Callback function type for use with CF_CList_Traverse()
 *
 * @param node    Current node being traversed
 * @param context Opaque pointer passed through from initial call
 *
 * @returns integer status code indicating whether to continue traversal
 * @retval  #CF_CLIST_CONT Indicates to continue traversing the list
 * @retval  #CF_CLIST_EXIT Indicates to stop traversing the list
 */
typedef CF_CListTraverse_Status_t (*CF_CListFn_t)(CF_CListNode_t *node, void *context);

/**
 * @brief Modern callback type for list traversal
 *
 * Replaces CF_CListFn_t with a more flexible std::function-based callback.
 * The callback receives the node and an opaque context pointer.
 */
using CListTraverseCallback = std::function<CF_CListTraverse_Status_t(CF_CListNode_t* node, void* context)>;

/************************************************************************/
/** @brief Initialize a clist node.
 *
 * @param node  Pointer to node structure to be initialized
 */
void CF_CList_InitNode(CF_CListNode_t *node);

/************************************************************************/
/** @brief Insert the given node into the front of a list.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Insert the given node into the back of a list.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Remove the given node from the list.
 *
 * @param head  Pointer to head of list to remove from
 * @param node  Pointer to node to remove
 */
void CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Remove the first node from a list and return it.
 *
 * @param head  Pointer to head of list to remove from
 *
 * @returns The first node (now removed) in the list
 * @retval  NULL if list was empty.
 */
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head);

/************************************************************************/
/** @brief Insert the given node into the last after the given start node.
 *
 * @param head  Pointer to head of list to remove from
 * @param start Pointer to node to insert
 * @param after Pointer to position to insert after
 */
void CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after);

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
void CF_CList_Traverse(CF_CListNode_t *start, CF_CListFn_t fn, void *context);

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
void CF_CList_Traverse(CF_CListNode_t *start, const CListTraverseCallback& callback, void *context);

/************************************************************************/
/** @brief Reverse list traversal, starting from end, calling given function on all nodes.
 *
 * @note traverse_R will work backwards from the parameter's prev, and end on param
 *
 * @param end     List to traverse (last node)
 * @param fn      Callback function to invoke for each node
 * @param context Opaque pointer to pass to callback
 */
void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context);

}  // namespace Ccsds
}  // namespace Svc

#endif /* !CFDP_CLIST_HPP */
