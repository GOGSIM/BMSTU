#pragma once

#include <exception>
#include <string>

class OutOfRangeException : public std::exception {
public:
    explicit OutOfRangeException(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string m_message;
};

class IncompatibleDimException : public std::exception {
public:
    explicit IncompatibleDimException(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string m_message;
};