#include "Exceptions.h"

OutOfRangeException::OutOfRangeException(const std::string& message)
    : m_message(message) {
}

const char* OutOfRangeException::what() const noexcept {
    return m_message.c_str();
}

IncompatibleDimException::IncompatibleDimException(const std::string& message)
    : m_message(message) {
}

const char* IncompatibleDimException::what() const noexcept {
    return m_message.c_str();
}