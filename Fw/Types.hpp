
#ifndef FW_TYPES_HPP_
#define FW_TYPES_HPP_
#include <config/FppConstantsAc.hpp>
#include <limits>
extern "C" {
    #include <Fw/Types.h>
}


// Define max length of assert string
// Note: This constant truncates file names in assertion failure event reports
#ifndef FW_ASSERT_TEXT_SIZE
#define FW_ASSERT_TEXT_SIZE FwAssertTextSize  //!< Size of string used to store assert description
#endif
#endif // FW_TYPES_HPP
