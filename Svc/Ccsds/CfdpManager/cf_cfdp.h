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
 *  The CF Application CFDP engine and packet parsing header file
 */

#ifndef CF_CFDP_H
#define CF_CFDP_H

#include "cf_cfdp_types.h"

/**
 * @brief Structure for use with the CF_CFDP_CycleTx() function
 */
typedef struct CF_CFDP_CycleTx_args
{
    CF_Channel_t *chan;    /**< \brief channel structure */
    int           ran_one; /**< \brief should be set to 1 if a transaction was cycled */
} CF_CFDP_CycleTx_args_t;

/**
 * @brief Structure for use with the CF_CFDP_DoTick() function
 */
typedef struct CF_CFDP_Tick_args
{
    CF_Channel_t *chan;                    /**< \brief channel structure */
    void (*fn)(CF_Transaction_t *, int *); /**< \brief function pointer */
    bool early_exit;                       /**< \brief early exit result */
    int  cont;                             /**< \brief if 1, then re-traverse the list */
} CF_CFDP_Tick_args_t;

/********************************************************************************/
/**
 * @brief Initiate the process of encoding a new PDU to send
 *
 * This resets the encoder and PDU buffer to initial values, and prepares for encoding a new PDU
 * for sending to a remote entity.
 *
 * @param penc           Encoder state structure, will be reset/initialized by this call to point to msgbuf.
 * @param msgbuf         Pointer to encapsulation message, in this case a CFE software bus message
 * @param ph             Pointer to logical PDU buffer content, will be cleared to all zero by this call
 * @param encap_hdr_size Offset of first CFDP PDU octet within buffer
 * @param total_size     Allocated size of msgbuf encapsulation structure (encoding cannot exceed this)
 */
void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);

/********************************************************************************/
/**
 * @brief Initiate the process of decoding a received PDU
 *
 * This resets the decoder and PDU buffer to initial values, and prepares for decoding a new PDU
 * that was received from a remote entity.
 *
 * @param pdec           Decoder state structure, will be reset/initialized by this call to point to msgbuf.
 * @param msgbuf         Pointer to encapsulation message, in this case a CFE software bus message
 * @param ph             Pointer to logical PDU buffer content, will be cleared to all zero by this call
 * @param encap_hdr_size Offset of first CFDP PDU octet within buffer
 * @param total_size     Total size of msgbuf encapsulation structure (decoding cannot exceed this)
 */
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);

/* engine execution functions */

/************************************************************************/
/** @brief Finish a transaction
 *
 * This marks the transaction as completed and puts it into a holdover state.
 * After the inactivity timer expires, the resources will be recycled and
 * become available for re-use.
 *
 * Holdover is necessary because even though locally we consider the transaction
 * to be complete, there may be undelivered PDUs still in network queues that
 * get delivered to us late.  By holding this transaction for a bit longer,
 * we can still associate those PDUs with this transaction/seq_num and
 * appropriately handle them as dupes/spurious deliveries.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param keep_history Whether the transaction info should be preserved in history
 */
void CF_CFDP_FinishTransaction(CF_Transaction_t *txn, bool keep_history);

/************************************************************************/
/** @brief Recover resources associated with a transaction
 *
 * Wipes all data in the transaction struct and returns everything to its
 * relevant FREE list so it can be used again.
 *
 * Notably, should any PDUs arrive after this that is related to this
 * transaction, these PDUs will not be identifiable, and no longer associable
 * to this transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *     It is imperative that nothing uses the txn struct after this call,
 *     as it will now be invalid.  This is effectively like free().
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_RecycleTransaction(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Helper function to store transaction status code only
 *
 * This records the status in the history block but does not set FIN flag
 * or take any other protocol/state machine actions.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param txn_stat Status Code value to set within transaction
 */
void CF_CFDP_SetTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat);

/************************************************************************/
/** @brief Send an end of transaction packet.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_SendEotPkt(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Initialization function for the CFDP engine
 *
 * @par Description
 *       Performs all initialization of the CFDP engine
 *
 * @par Assumptions, External Events, and Notes:
 *       Only called once.
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @returns anything else on error.
 *
 */
CFE_Status_t CF_CFDP_InitEngine(void);

/************************************************************************/
/** @brief Cycle the engine. Called once per wakeup.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
void CF_CFDP_CycleEngine(void);

/************************************************************************/
/** @brief Disables the CFDP engine and resets all state in it.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
void CF_CFDP_DisableEngine(void);

/************************************************************************/
/** @brief Begin transmit of a file.
 *
 * @par Description
 *       This function sets up a transaction for and starts transmit of
 *       the given filename.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_filename must not be NULL. dst_filename must not be NULL.
 *
 * @param src_filename  Local filename
 * @param dst_filename  Remote filename
 * @param cfdp_class    Whether to perform a class 1 or class 2 transfer
 * @param keep          Whether to keep or delete the local file after completion
 * @param chan          CF channel number to use
 * @param priority      CF priority level
 * @param dest_id       Entity ID of remote receiver
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @returns CFE_SUCCESS on success. CF_ERROR on error.
 */
CFE_Status_t CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                            uint8 chan, uint8 priority, CF_EntityId_t dest_id);

/************************************************************************/
/** @brief Begin transmit of a directory.
 *
 * @par Description
 *       This function sets up CF_Playback_t structure with state so it can
 *       become part of the directory polling done at each engine cycle.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_filename must not be NULL. dst_filename must not be NULL.
 *
 * @param src_filename  Local filename
 * @param dst_filename  Remote filename
 * @param cfdp_class    Whether to perform a class 1 or class 2 transfer
 * @param keep          Whether to keep or delete the local file after completion
 * @param chan          CF channel number to use
 * @param priority      CF priority level
 * @param dest_id       Entity ID of remote receiver
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @returns CFE_SUCCESS on success. CF_ERROR on error.
 */
CFE_Status_t CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class,
                                 uint8 keep, uint8 chan, uint8 priority, uint16 dest_id);

/************************************************************************/
/** @brief Build the PDU header in the output buffer to prepare to send a packet.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn              Pointer to the transaction object
 * @param directive_code Code to use for file directive headers (set to 0 for data)
 * @param src_eid        Value to set in source entity ID field
 * @param dst_eid        Value to set in destination entity ID field
 * @param towards_sender Whether this is transmitting toward the sender entity
 * @param tsn            Transaction sequence number to put into PDU
 * @param silent         If true, suppress error event if no message buffer available
 *
 * @returns Pointer to PDU buffer which may be filled with additional data
 * @retval  NULL if no message buffer available
 */
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *txn, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent);

/************************************************************************/
/** @brief Build a metadata PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn              Pointer to the transaction object
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 */
CFE_Status_t CF_CFDP_SendMd(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Send a previously-assembled filedata PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn   Pointer to the transaction object
 * @param ph  Pointer to logical PDU buffer content
 *
 * @note Unlike other "send" routines, the file data PDU must be acquired and
 * filled by the caller prior to invoking this routine.  This routine only
 * sends the PDU that was previously allocated and assembled.  As such, the
 * typical failure possibilities do not apply to this call.
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success. (error checks not yet implemented)
 */
CFE_Status_t CF_CFDP_SendFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Build an EOF PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn   Pointer to the transaction object
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 */
CFE_Status_t CF_CFDP_SendEof(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Build an ACK PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @note CF_CFDP_SendAck() takes a CF_TransactionSeq_t instead of getting it from transaction history because
 * of the special case where a FIN-ACK must be sent for an unknown transaction. It's better for
 * long term maintenance to not build an incomplete CF_History_t for it.
 *
 * @param txn        Pointer to the transaction object
 * @param ts       Transaction ACK status
 * @param dir_code File directive code being ACK'ed
 * @param cc       Condition code of transaction
 * @param peer_eid Remote entity ID
 * @param tsn      Transaction sequence number
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 */
CFE_Status_t CF_CFDP_SendAck(CF_Transaction_t *txn, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn);

/************************************************************************/
/** @brief Build a FIN PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 * @param dc    Final delivery status code (complete or incomplete)
 * @param fs    Final file status (retained or rejected, etc)
 * @param cc    Final CFDP condition code
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 */
CFE_Status_t CF_CFDP_SendFin(CF_Transaction_t *txn, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc);

/************************************************************************/
/** @brief Send a previously-assembled NAK PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn   Pointer to the transaction object
 * @param ph  Pointer to logical PDU buffer content
 *
 * @note Unlike other "send" routines, the NAK PDU must be acquired and
 * filled by the caller prior to invoking this routine.  This routine only
 * encodes and sends the previously-assembled PDU buffer.  As such, the
 * typical failure possibilities do not apply to this call.
 *
 * @returns CFE_Status_t status code
 * @retval CFE_SUCCESS on success.
 * @retval CF_SEND_PDU_NO_BUF_AVAIL_ERROR if message buffer cannot be obtained.
 */
CFE_Status_t CF_CFDP_SendNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Appends a single TLV value to the logical PDU data
 *
 * This function implements common functionality between SendEof and SendFin
 * which append a TLV value specifying the faulting entity ID.
 *
 * @par Assumptions, External Events, and Notes:
 *       ptlv_list must not be NULL.
 *       Only CF_CFDP_TLV_TYPE_ENTITY_ID type is currently implemented
 *
 * @param ptlv_list TLV list from current PDU buffer.
 * @param tlv_type  Type of TLV to append.  Currently must be CF_CFDP_TLV_TYPE_ENTITY_ID.
 */
void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type);

/************************************************************************/
/** @brief Unpack a basic PDU header from a received message.
 *
 * @par Description
 *       This interprets the common PDU header and the file directive header
 *       (if applicable) and populates the logical PDU buffer.
 *
 * @par Assumptions, External Events, and Notes:
 *       A new message has been received.
 *
 * @param chan_num The channel number for statistics purposes
 * @param ph       The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_ERROR for general errors
 * @retval CF_SHORT_PDU_ERROR if PDU too short
 */
CFE_Status_t CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a metadata PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a metadata PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_PDU_METADATA_ERROR on error
 */
CFE_Status_t CF_CFDP_RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a file data PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a file data PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_ERROR for general errors
 * @retval CF_SHORT_PDU_ERROR PDU too short
 */
CFE_Status_t CF_CFDP_RecvFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an EOF PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an end of file PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_SHORT_PDU_ERROR on error
 */
CFE_Status_t CF_CFDP_RecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an ACK PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an acknowledgment PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_SHORT_PDU_ERROR on error
 */
CFE_Status_t CF_CFDP_RecvAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an FIN PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a final PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_SHORT_PDU_ERROR on error
 */
CFE_Status_t CF_CFDP_RecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a NAK PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a negative/non-acknowledgment PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval CFE_SUCCESS on success
 * @retval CF_SHORT_PDU_ERROR on error
 */
CFE_Status_t CF_CFDP_RecvNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Dispatch received packet to its handler.
 *
 * This dispatches the PDU to the appropriate handler
 * based on the transaction state
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be null. It must be an initialized transaction.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 */
void CF_CFDP_DispatchRecv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Cancels a transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 *
 */
void CF_CFDP_CancelTransaction(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Helper function to set tx file state in a transaction.
 *
 * This sets various fields inside a newly-allocated transaction
 * structure appropriately for sending a file.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn          Pointer to the transaction state
 * @param cfdp_class Set to class 1 or class 2
 * @param keep       Whether to keep the local file
 * @param chan       CF channel number
 * @param priority   Priority of transfer
 *
 */
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *txn, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority);

/************************************************************************/
/** @brief Helper function to start a new RX transaction
 *
 * This sets various fields inside a newly-allocated transaction
 * structure appropriately for receiving a file.  Note that in the
 * receive direction, most fields are unknown until the MD is received,
 * and thus are left in their initial state here (generally 0).
 *
 * If there is no capacity for another RX transaction, this returns NULL.
 *
 * @param chan_num       CF channel number
 * @returns Pointer to new transaction
 *
 */
CF_Transaction_t *CF_CFDP_StartRxTransaction(uint8 chan_num);

/* functions to handle LVs (length-value, CFDP spec) */
/* returns number of bytes copied, or -1 on error */

/************************************************************************/
/** @brief Copy string data from a lv (length, value) pair.
 *
 * This copies a string value from an LV pair inside a PDU buffer.
 * In CF this is used for file names embedded within PDUs.
 *
 * @note This function assures that the output string is terminated
 * appropriately, such that it can be used as a normal C string.  As
 * such, the buffer size must be at least 1 byte larger than the maximum
 * string length.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_lv must not be NULL. buf must not be NULL.
 *
 * @param buf        Pointer to buffer to store string
 * @param buf_maxsz  Total size of buffer pointer to by buf (usable size is 1 byte less, for termination)
 * @param src_lv     Pointer to LV pair from logical PDU buffer
 *
 * @returns The resulting string length, NOT including termination character
 * @retval CF_ERROR on error
 */
int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv);

/************************************************************************/
/** @brief Arm the ACK timer
 *
 * @par Description
 *       Helper function to arm the ACK timer and set the flag.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn          Pointer to the transaction state
 */
void CF_CFDP_ArmAckTimer(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Receive state function to ignore a packet.
 *
 * @par Description
 *       This function signature must match all receive state functions.
 *       The parameter txn is ignored here.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 */
void CF_CFDP_RecvDrop(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Receive state function during holdover period.
 *
 * @par Description
 *       This function signature must match all receive state functions.
 *       Handles any possible spurious PDUs that might come in after the
 *       transaction is considered done.  This can happen if ACKs were
 *       lost in transmission causing the sender to retransmit PDUs even
 *       though we already completed the transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 */
void CF_CFDP_RecvHold(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Receive state function to process new rx transaction.
 *
 * @par Description
 *       An idle transaction has never had message processing performed on it.
 *       Typically, the first packet received for a transaction would be
 *       the metadata PDU. There's a special case for R2 where the metadata
 *       PDU could be missed, and filedata comes in instead. In that case,
 *       an R2 transaction must still be started.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. There must be a received message.
 *
 * @param txn    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 */
void CF_CFDP_RecvInit(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief List traversal function to close all files in all active transactions.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL.
 *
 * @param node       List node pointer
 * @param context Opaque pointer, not used in this function
 *
 * @returns integer traversal code
 * @retval Always CF_LIST_CONT indicate list traversal should not exit early.
 */
CF_CListTraverse_Status_t CF_CFDP_CloseFiles(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Cycle the current active tx or make a new one active.
 *
 * @par Description
 *       First traverses all tx transactions on the active queue. If at
 *       least one is found, then it stops. Otherwise it moves a
 *       transaction on the pending queue to the active queue and
 *       tries again to find an active one.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param chan Channel to cycle
 */
void CF_CFDP_CycleTx(CF_Channel_t *chan);

/************************************************************************/
/** @brief List traversal function that cycles the first active tx.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Description
 *       There can only be one active tx transaction per engine cycle.
 *       This function finds the first active, and then sends file
 *       data PDUs until there are no outgoing message buffers.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. Context must not be NULL.
 *
 * @param node    Pointer to list node
 * @param context Pointer to CF_CFDP_CycleTx_args_t object (passed through)
 *
 * @returns integer traversal code
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it's isn't found, which causes list traversal to continue
 */
CF_CListTraverse_Status_t CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Call R and then S tick functions for all active transactions.
 *
 * @par Description
 *       Traverses all transactions in the RX and TXW queues, and calls
 *       their tick functions. Note that the TXW queue is used twice:
 *       once for regular tick processing, and one for NAK response.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL.
 *
 * @param chan Channel to tick
 */
void CF_CFDP_TickTransactions(CF_Channel_t *chan);

/************************************************************************/
/** @brief Step each active playback directory.
 *
 * @par Description
 *       Check if a playback directory needs iterated, and if so does, and
 *       if a valid file is found initiates playback on it.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL, pb must not be NULL.
 *
 * @param chan  The channel associated with the playback
 * @param pb  The playback state
 */
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *chan, CF_Playback_t *pb);

/************************************************************************/
/** @brief Kick the dir playback if timer elapsed.
 *
 * @par Description
 *       This function waits for the polling directory interval timer,
 *       and if it has expired, starts a playback in the polling directory.
 *
 * @par Assumptions, External Events, and Notes:
 *       chan must not be NULL.
 *
 * @param chan  The channel associated with the playback
 */
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *chan);

/************************************************************************/
/** @brief List traversal function that calls a r or s tick function.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL, context must not be NULL.
 *
 * @param node    Pointer to list node
 * @param context Pointer to CF_CFDP_Tick_args_t object (passed through)
 *
 * @returns integer traversal code
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it's isn't found, which causes list traversal to continue
 */
CF_CListTraverse_Status_t CF_CFDP_DoTick(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Check if source file came from polling directory
 *
 *
 * @par Assumptions, External Events, and Notes:
 *
 * @retval true/false
 */
bool CF_CFDP_IsPollingDir(const char *src_file, uint8 chan_num);

/************************************************************************/
/** @brief Remove/Move file after transaction
 *
 * This helper is used to handle "not keep" file option after a transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *
 */
void CF_CFDP_HandleNotKeepFile(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Move File
 *
 * This helper is used to move a file.
 *
 * @par Assumptions, External Events, and Notes:
 *
 */
void CF_CFDP_MoveFile(const char *src, const char *dest_dir);

#endif /* !CF_CFDP_H */
