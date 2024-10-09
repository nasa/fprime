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
char* string_copy(char* destination, const char* source, FwSizeType num);

/**
 * \brief get the length of the source string
 *
 * If no string termination character is detected within buffer_size number of characters then buffer_size is returned.
 * When buffer_size is returned, it can be assumed that the source string is invalid within a bound of buffer_size.
 *
 * \param source: string to calculate the length
 * \param buffer_size: the size of the buffer containing source string.
 * \return length of the source string or buffer_size if no \0 is found within buffer_size characters.
 */
FwSizeType string_length(const CHAR* source, FwSizeType buffer_size);

/**
 * \brief find the first occurrence of a substring
 *
 * \param source_string: string to search for the substring
 * \param source_size: the size of the source string
 * \param substring: string to search for
 * \param sub_size: the size of the string to search for
 * \return index of substring, -1 if not found
 */
FwSignedSizeType substring_find(const CHAR* source_string, FwSizeType source_size, const CHAR* sub_string, FwSizeType sub_size);

enum StringToNumberStatus {
    SUCCESSFUL_CONVERSION, //!< Output should be valid
    NULL_INPUT,     //!< A null string was supplied
    INVALID_STRING, //!< No \0 detected within the supplied length
    INVALID_BASE,   //!< Base was not supplied as 0, or 2-36
    INVALID_NUMBER, //!< String did not contain a valid number matching supplied base
    INVALID_RANGE,  //!<
};

#if FW_HAS_64_BIT
/**
 * \brief converts a string to a U64
 *
 * Function to convert a string to a U64. String is provided via the input parameter and output is set in the output
 * reference parameter. If no \0 is found within buffer_size, then this function will return an error, while setting
 * output to 0.  If next is non-null, then a pointer to the character after the end of the number will be returned upon
 * successful conversion.  Otherwise next will be set to null pointer.
 *
 * If base is 0, then the numerical base of the number is inferred from the string itself via the prefix (e.g 0x) and
 * lacking a prefix is assumed to be decimal (base 10). Otherwise, base may be 2-36.
 *
 * If the converted number does not fit within the range of the type of output, output is set to min or max
 * appropriately and INVALID_RANGE is returned.
 *
 * It is an error to supply a null input string.
 * It is an error to supply a string that does not terminate with a \0 within buffer_size characters
 * The numerical conversion may also return errors.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, U64& output, char** next, U8 base=0);

/**
 * \brief converts a string to a I64
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, I64& output, char** next, U8 base=0);
#endif
#if FW_HAS_32_BIT
/**
 * \brief converts a string to a U32
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, U32& output, char** next, U8 base=0);

/**
 * \brief converts a string to a I32
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, I32& output, char** next, U8 base=0);
#endif
#if FW_HAS_16_BIT
/**
 * \brief converts a string to a U16
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, U16& output, char** next, U8 base=0);

/**
 * \brief converts a string to a I16
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, I16& output, char** next, U8 base=0);
#endif

/**
 * \brief converts a string to a U8
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, U8& output, char** next, U8 base=0);

/**
 * \brief converts a string to a I8
 *
 * See string_to_number (above) for full explanation.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, I8& output, char** next, U8 base=0);

/**
 * \brief converts a string to a F32
 *
 * See string_to_number (above) for full explanation. `base` is not supported on floating point conversions.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, F32& output, char** next);

/**
 * \brief converts a string to a F64
 *
 * See string_to_number (above) for full explanation. `base` is not supported on floating point conversions.
 *
 * \param input: input string
 * \param buffer_size: maximum length of string bounding the conversion
 * \param base: base of the number. 0 to detect decimal, octal, hexadecimal. 2-36 to use specified base.
 * \param output: (output) will contain the converted number or 0 on error
 * \param next: (output) will contain a pointer to the next character after the number and null pointer on error
 * \return SUCCESSFUL_CONVERSION when output is valid, something else on error.
 */
StringToNumberStatus string_to_number(const CHAR* input, FwSizeType buffer_size, F64& output, char** next);




}  // namespace StringUtils
}  // namespace Fw
#endif  // FW_STRINGUTILS_HPP
