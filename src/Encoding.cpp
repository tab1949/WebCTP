#include <unicode/ucnv.h>
#include <unicode/ustring.h>
#include <unicode/errorcode.h>
#include <unicode/utypes.h>
#include <vector>
#include <stdexcept>
#include <string>
#include "Encoding.hpp"

namespace tabxx {
namespace {

thread_local UConverter* gbk_converter = nullptr;
thread_local UConverter* utf8_converter = nullptr;
thread_local std::vector<UChar> unicode_buffer;
thread_local std::string output_buffer;

inline UConverter* init_converter(const char* name, UConverter*& cache) {
    if (!cache) {
        UErrorCode status = U_ZERO_ERROR;
        cache = ucnv_open(name, &status);
        if (U_FAILURE(status)) {
            const char* error_name = u_errorName(status);
            throw std::runtime_error(
                std::string("Failed to initialize ") + name + 
                " converter: " + (error_name ? error_name : "unknown error")
            );
        }
    }
    return cache;
}

inline void throw_conversion_error(const char* stage, UErrorCode status) {
    const char* error_name = u_errorName(status);
    throw std::runtime_error(
        std::string("Encoding conversion failed at ") + stage + 
        ": " + (error_name ? error_name : "unknown error")
    );
}

} // namespace
} // namespace tabxx

std::string tabxx::u8(const std::string& gbk) {
    if (gbk.empty()) return {};
    
    auto* gbk_conv = init_converter("GBK", gbk_converter);
    auto* utf8_conv = init_converter("UTF-8", utf8_converter);
    
    ucnv_reset(gbk_conv);
    ucnv_reset(utf8_conv);
    
    const int32_t gbk_len = static_cast<int32_t>(gbk.size());
    UErrorCode status = U_ZERO_ERROR;
    
    // GBK -> Unicode
    const size_t unicode_needed = static_cast<size_t>(gbk_len);
    if (unicode_buffer.size() < unicode_needed) {
        unicode_buffer.resize(unicode_needed);
    }
    
    int32_t unicode_length = ucnv_toUChars(
        gbk_conv,
        unicode_buffer.data(), 
        static_cast<int32_t>(unicode_buffer.size()),
        gbk.data(),
        gbk_len,
        &status
    );
    
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        const size_t unicode_required = static_cast<size_t>(-unicode_length);
        if (unicode_buffer.size() < unicode_required) {
            unicode_buffer.resize(unicode_required);
        }
        ucnv_reset(gbk_conv);
        status = U_ZERO_ERROR;
        unicode_length = ucnv_toUChars(
            gbk_conv,
            unicode_buffer.data(),
            static_cast<int32_t>(unicode_buffer.size()),
            gbk.data(),
            gbk_len,
            &status
        );
    }
    
    if (U_FAILURE(status)) {
        throw_conversion_error("GBK to Unicode", status);
    }
    if (unicode_length <= 0) {
        throw std::runtime_error("GBK to Unicode conversion produced zero-length result");
    }
    
    // Unicode -> UTF-8
    const size_t utf8_needed = static_cast<size_t>(unicode_length * 3);
    if (output_buffer.size() < utf8_needed) {
        output_buffer.resize(utf8_needed);
    }
    
    status = U_ZERO_ERROR;
    int32_t utf8_length = ucnv_fromUChars(
        utf8_conv,
        output_buffer.data(),
        static_cast<int32_t>(output_buffer.size()),
        unicode_buffer.data(),
        unicode_length,
        &status
    );
    
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        const size_t utf8_required = static_cast<size_t>(-utf8_length);
        if (output_buffer.size() < utf8_required) {
            output_buffer.resize(utf8_required);
        }
        ucnv_reset(utf8_conv);
        status = U_ZERO_ERROR;
        utf8_length = ucnv_fromUChars(
            utf8_conv,
            output_buffer.data(),
            static_cast<int32_t>(output_buffer.size()),
            unicode_buffer.data(),
            unicode_length,
            &status
        );
    }
    
    if (U_FAILURE(status)) {
        throw_conversion_error("Unicode to UTF-8", status);
    }
    if (utf8_length <= 0) {
        throw std::runtime_error("Unicode to UTF-8 conversion produced zero-length result");
    }
    
    return std::string(output_buffer.data(), static_cast<size_t>(utf8_length));
}