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
 * Declarations and prototypes for cf_extern_typedefs module
 */

#ifndef CF_EXTERN_TYPEDEFS_HPP
#define CF_EXTERN_TYPEDEFS_HPP

#include "Svc/Ccsds/CfdpManager/FppConstantsAc.hpp"

namespace Svc {
namespace Ccsds {

/**
 * @brief Values for CFDP file transfer class
 *
 * The CFDP specification prescribes two classes/modes of file
 * transfer protocol operation - unacknowledged/simple or
 * acknowledged/reliable.
 *
 * Defined per section 7.1 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_CLASS_1 = 0, /**< \brief CFDP class 1 - Unreliable transfer */
    CF_CFDP_CLASS_2 = 1, /**< \brief CFDP class 2 - Reliable transfer */
} CF_CFDP_Class_t;

/**
 * @brief CF queue identifiers
 */
typedef enum
{
    CF_QueueIdx_PEND      = 0, /**< \brief first one on this list is active */
    CF_QueueIdx_TXA       = 1,
    CF_QueueIdx_TXW       = 2,
    CF_QueueIdx_RX        = 3,
    CF_QueueIdx_HIST      = 4,
    CF_QueueIdx_HIST_FREE = 5,
    CF_QueueIdx_FREE      = 6,
    CF_QueueIdx_NUM       = 7
} CF_QueueIdx_t;

/**
 * @brief Cache of source and destination filename
 *
 * This pairs a source and destination file name together
 * to be retained for future reference in the transaction/history
 */
typedef struct CF_TxnFilenames
{
    char src_filename[FppConstant_CfdpManagerMaxFileSize::CfdpManagerMaxFileSize];
    char dst_filename[FppConstant_CfdpManagerMaxFileSize::CfdpManagerMaxFileSize];
} CF_TxnFilenames_t;


}  // namespace Ccsds
}  // namespace Svc

#endif /* CF_EXTERN_TYPEDEFS_HPP */
