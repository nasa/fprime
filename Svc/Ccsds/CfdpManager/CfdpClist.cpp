// ======================================================================
// \title  CfdpClist.cpp
// \brief  CFDP circular list definition source file
//
// This file is a port of the cf_clist.c file from the 
// NASA Core Flight System (cFS) CFDP (CF) Application,
// version 3.0.0, adapted for use within the F-Prime (F') framework.
//
// This is a circular doubly-linked list implementation. It is used for
// multiple data structures in CFDP.
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

#include <Svc/Ccsds/CfdpManager/CfdpClist.hpp>

#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void CfdpCListInitNode(CListNode *node)
{
    node->next = node;
    node->prev = node;
}

void CfdpCListInsertFront(CListNode **head, CListNode *node)
{
    CListNode *last;

    FW_ASSERT(head);
    FW_ASSERT(node);
    FW_ASSERT(node->next == node);
    FW_ASSERT(node->prev == node);

    if (*head)
    {
        last = (*head)->prev;

        node->next = *head;
        node->prev = last;

        last->next    = node;
        (*head)->prev = node;
    }

    *head = node;
}

void CfdpCListInsertBack(CListNode **head, CListNode *node)
{
    CListNode *last;

    FW_ASSERT(head);
    FW_ASSERT(node);
    FW_ASSERT(node->next == node);
    FW_ASSERT(node->prev == node);

    if (!*head)
    {
        *head = node;
    }
    else
    {
        last = (*head)->prev;

        node->next    = *head;
        (*head)->prev = node;
        node->prev    = last;
        last->next    = node;
    }
}

CListNode *CfdpCListPop(CListNode **head)
{
    CListNode *ret;

    FW_ASSERT(head);

    ret = *head;
    if (ret)
    {
        CfdpCListRemove(head, ret);
    }

    return ret;
}

void CfdpCListRemove(CListNode **head, CListNode *node)
{
    FW_ASSERT(head);
    FW_ASSERT(node);
    FW_ASSERT(*head);

    if (node->next == node)
    {
        /* only node in the list, so this one is easy */
        FW_ASSERT(node == *head); /* sanity check */
        *head = NULL;
    }
    else if (*head == node)
    {
        /* removing the first node in the list, so make the second node in the list the first */
        (*head)->prev->next = node->next;
        *head               = node->next;

        (*head)->prev = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

    CfdpCListInitNode(node);
}

void CfdpCListInsertAfter(CListNode **head, CListNode *start, CListNode *after)
{
    /* calling insert_after with nothing to insert after (no head) makes no sense */
    FW_ASSERT(head);
    FW_ASSERT(*head);
    FW_ASSERT(start);
    FW_ASSERT(start != after);

    /* knowing that head is not empty, and knowing that start is non-zero, this is an easy operation */
    after->next       = start->next;
    start->next       = after;
    after->prev       = start;
    after->next->prev = after;
}

void CfdpCListTraverse(CListNode *start, CListFunc fn, void *context)
{
    CListNode *node = start;
    CListNode *node_next;
    bool           last = false;

    if (node)
    {
        do
        {
            /* set node_next in case callback removes this node from the list */
            node_next = node->next;
            if (node_next == start)
            {
                last = true;
            }
            if (!CfdpCListTraverseStatusIsContinue(fn(node, context)))
            {
                break;
            }
            /* list traversal is robust against an item deleting itself during traversal,
             * but there is a special case if that item is the starting node. Since this is
             * a circular list, start is remembered so we know when to stop. Must set start
             * to the next node in this case. */
            if ((start == node) && (node->next != node_next))
            {
                start = node_next;
            }
            node = node_next;
        }
        while (!last);
    }
}

void CfdpCListTraverse(CListNode *start, const CListTraverseCallback& callback, void *context)
{
    CListNode *node = start;
    CListNode *node_next;
    bool           last = false;

    if (node)
    {
        do
        {
            /* set node_next in case callback removes this node from the list */
            node_next = node->next;
            if (node_next == start)
            {
                last = true;
            }
            if (!CfdpCListTraverseStatusIsContinue(callback(node, context)))
            {
                break;
            }
            /* list traversal is robust against an item deleting itself during traversal,
             * but there is a special case if that item is the starting node. Since this is
             * a circular list, start is remembered so we know when to stop. Must set start
             * to the next node in this case. */
            if ((start == node) && (node->next != node_next))
            {
                start = node_next;
            }
            node = node_next;
        }
        while (!last);
    }
}

void CfdpCListTraverseR(CListNode *end, CListFunc fn, void *context)
{
    if (end)
    {
        CListNode *node = end->prev;
        CListNode *node_next;
        bool           last = false;

        if (node)
        {
            end = node;

            do
            {
                /* set node_next in case callback removes this node from the list */
                node_next = node->prev;
                if (node_next == end)
                {
                    last = true;
                }

                if (!CfdpCListTraverseStatusIsContinue(fn(node, context)))
                {
                    break;
                }

                /* list traversal is robust against an item deleting itself during traversal,
                 * but there is a special case if that item is the starting node. Since this is
                 * a circular list, "end" is remembered so we know when to stop. Must set "end"
                 * to the next node in this case. */
                if ((end == node) && (node->prev != node_next))
                {
                    end = node_next;
                }
                node = node_next;
            }
            while (!last);
        }
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc