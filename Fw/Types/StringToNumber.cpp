#include <Fw/Types/StringUtils.hpp>
#include <cstdlib>
#include <cerrno>

Fw::StringUtils::StringToNumberStatus string_to_helper_input_check(const CHAR* input, FwSizeType buffer_size, U8 base) {
    Fw::StringUtils::StringToNumberStatus status = Fw::StringUtils::StringToNumberStatus::SUCCESSFUL_CONVERSION;
    // Check for null input string
    if (input == nullptr) {
        status = Fw::StringUtils::NULL_INPUT;
    }
    // Invalid base e.g. not 0, 2-36
    else if (base == 1 || base > 36) {
        status = Fw::StringUtils::INVALID_BASE;
    } else {
        // Check the string is bounded within the specified length
        FwSizeType length = Fw::StringUtils::string_length(input, buffer_size);
        if (length == buffer_size) {
            status = Fw::StringUtils::INVALID_STRING;
        }
    }
    errno = 0;
    return status;
}

Fw::StringUtils::StringToNumberStatus string_to_helper_output_check(Fw::StringUtils::StringToNumberStatus status, const char* original_input, char*& internal_next, char** external_next) {
    // Check range, if error then
    if (errno == ERANGE) {
        status = Fw::StringUtils::INVALID_RANGE;
    }
    // Invalid number conversion
    else if ((internal_next == original_input) || (internal_next == nullptr)) {
        internal_next = nullptr;
        status = Fw::StringUtils::INVALID_NUMBER;
    }
    // Set output pointer in all cases
    if (external_next != nullptr) {
        *external_next = internal_next;
    }
    errno = 0;
    return status;
}

// Template for internal implementation only
// \tparam T: input type (U8, I8, U64, I64)
// \tparam Tinternal: function api type
// \tparam F: conversion function to use
template <typename T, typename Tinternal, Tinternal (*F)(const char*, char**, int)>
Fw::StringUtils::StringToNumberStatus string_to_number_as_template(const CHAR* input, FwSizeType buffer_size, T& output, char** next, U8 base) {
    static_assert(std::numeric_limits<T>::is_integer, "Type must be integer");
    static_assert(std::numeric_limits<Tinternal>::is_integer, "Type must be integer");
    static_assert(std::numeric_limits<T>::is_signed == std::numeric_limits<Tinternal>::is_signed, "Signedness must match");
    static_assert(std::numeric_limits<T>::max() <= std::numeric_limits<Tinternal>::max(), "Invalid internal type chosen");
    static_assert(std::numeric_limits<T>::min() >= std::numeric_limits<Tinternal>::min(), "Invalid internal type chosen");

    char* output_next = nullptr;
    Fw::StringUtils::StringToNumberStatus status = string_to_helper_input_check(input, buffer_size, base);
    if (status == Fw::StringUtils::SUCCESSFUL_CONVERSION) {
        Tinternal output_api = F(input, &output_next, base);
        if (output_api > std::numeric_limits<T>::max()) {
            status = Fw::StringUtils::StringToNumberStatus::INVALID_RANGE;
            output_api = std::numeric_limits<T>::max();
        }
        if (output_api < std::numeric_limits<T>::min()) {
            status = Fw::StringUtils::StringToNumberStatus::INVALID_RANGE;
            output_api = std::numeric_limits<T>::min();
        }
        output = static_cast<T>(output_api);
    }
    status = string_to_helper_output_check(status, input, output_next, next);
    return status;
}

#if FW_HAS_64_BIT
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, U64& output, char** next, U8 base) {
    return string_to_number_as_template<U64, unsigned long long, strtoull>(input, buffer_size, output, next, base);
}

Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, I64& output, char** next, U8 base) {
    return string_to_number_as_template<I64, long long, strtoll>(input, buffer_size, output, next, base);
}
#endif
#if FW_HAS_32_BIT
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, U32& output, char** next, U8 base) {
    return string_to_number_as_template<U32, unsigned long long, strtoull>(input, buffer_size, output, next, base);
}


Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, I32& output, char** next, U8 base) {
    return string_to_number_as_template<I32, long long, strtoll>(input, buffer_size, output, next, base);
}
#endif
#if FW_HAS_16_BIT
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, U16& output, char** next, U8 base) {
    return string_to_number_as_template<U16, unsigned long long, strtoull>(input, buffer_size, output, next, base);
}
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, I16& output, char** next, U8 base) {
    return string_to_number_as_template<I16, long long, strtoll>(input, buffer_size, output, next, base);
}
#endif
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, U8& output, char** next, U8 base) {
    return string_to_number_as_template<U8, unsigned long long, strtoull>(input, buffer_size, output, next, base);
}
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, I8& output, char** next, U8 base) {
    return string_to_number_as_template<I8, long long, strtoll>(input, buffer_size, output, next, base);
}
Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, F64& output, char** next) {
    char* output_next = nullptr;
    Fw::StringUtils::StringToNumberStatus status = string_to_helper_input_check(input, buffer_size, 0);
    if (status == SUCCESSFUL_CONVERSION) {
        output = strtod(input, &output_next);
    }
    status = string_to_helper_output_check(status, input, output_next, next);
    return status;
}

Fw::StringUtils::StringToNumberStatus Fw::StringUtils::string_to_number(const CHAR* input, FwSizeType buffer_size, F32& output, char** next) {
    char* output_next = nullptr;
    Fw::StringUtils::StringToNumberStatus status = string_to_helper_input_check(input, buffer_size, 0);
    if (status == SUCCESSFUL_CONVERSION) {
        output = strtof(input, &output_next);
    }
    status = string_to_helper_output_check(status, input, output_next, next);
    return status;
}
