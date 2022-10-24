#ifndef FW_STRINGUTILS_HPP
#define FW_STRINGUTILS_HPP
#include <FpConfig.hpp>

namespace Fw {
namespace StringUtils {

/**
 * \brief copy string with null-termination guaranteed
 *
 * Standard implementations of strncpy fail to guarantee the termination of a
 * string with the null terminator. This implementation guarantees the string is
 * properly null-terminated at the possible expense of the last character of the
 * copied string being lost. The user is responsible for providing a destination
 * large enough for the content and a null-character. Other behavior retains the
 * behavior of strncpy.
 *
 * \param destination: destination buffer to hold copied contents
 * \param source: source buffer to read content to copy
 * \param num: length of destination buffer
 * \return destination buffer
 */
char* string_copy(char* destination, const char* source, U32 num);

/**
 * \brief get the length of the source string or max_len if the string is
 * longer than max_len.
 *
 * \param source: string to calculate the length
 * \param max_len: the maximum length of the source string
 * \return length of the source string or max_len
 */
U32 string_length(const CHAR* source, U32 max_len);

}       // namespace StringUtils
}       // namespace Fw
#endif  // FW_STRINGUTILS_HPP
