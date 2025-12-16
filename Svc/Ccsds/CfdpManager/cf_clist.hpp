/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * The CF Application circular list header file
 */

#ifndef CF_CLIST_H
#define CF_CLIST_H

#include <stddef.h>
#include <stdbool.h>

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
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (char *)offsetof(type, member)))

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

/************************************************************************/
/** @brief Initialize a clist node.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL.
 *
 * @param node  Pointer to node structure to be initialized
 */
void CF_CList_InitNode(CF_CListNode_t *node);

/************************************************************************/
/** @brief Insert the given node into the front of a list.
 *
 * @par Assumptions, External Events, and Notes:
 *       head must not be NULL, node must not be NULL.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Insert the given node into the back of a list.
 *
 * @par Assumptions, External Events, and Notes:
 *       head must not be NULL, node must not be NULL.
 *
 * @param head  Pointer to head of list to insert into
 * @param node  Pointer to node to insert
 */
void CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Remove the given node from the list.
 *
 * @par Assumptions, External Events, and Notes:
 *       head must not be NULL, node must not be NULL.
 *
 * @param head  Pointer to head of list to remove from
 * @param node  Pointer to node to remove
 */
void CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node);

/************************************************************************/
/** @brief Remove the first node from a list and return it.
 *
 * @par Assumptions, External Events, and Notes:
 *       head must not be NULL.
 *
 * @param head  Pointer to head of list to remove from
 *
 * @returns The first node (now removed) in the list
 * @retval  NULL if list was empty.
 *
 */
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head);

/************************************************************************/
/** @brief Insert the given node into the last after the given start node.
 *
 * @par Assumptions, External Events, and Notes:
 *       head must not be NULL, node must not be NULL.
 *
 * @param head  Pointer to head of list to remove from
 * @param start Pointer to node to insert
 * @param after Pointer to position to insert after
 */
void CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after);

/************************************************************************/
/** @brief Traverse the entire list, calling the given function on all nodes.
 *
 * @par Assumptions, External Events, and Notes:
 *       start may be NULL, fn must be a valid function, context may be NULL.
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
/** @brief Reverse list traversal, starting from end, calling given function on all nodes.
 *
 * @par Assumptions, External Events, and Notes:
 *       end may be NULL. fn must be a valid function, context may be NULL.
 *
 * @note traverse_R will work backwards from the parameter's prev, and end on param
 *
 * @param end     List to traverse (last node)
 * @param fn      Callback function to invoke for each node
 * @param context Opaque pointer to pass to callback
 */
void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context);

#endif /* !CF_CLIST_H */
