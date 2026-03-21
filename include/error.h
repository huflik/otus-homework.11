#pragma once

#include <string>

enum class Error
{
    NONE,
    INVALID_COMMAND,
    INVALID_ARGUMENTS_COUNT,
    INVALID_ARGUMENTS_VALUE,
    DUPLICATE_ID,
    TABLE_NOT_FOUND,
    DATABASE_ERROR,
    UNKNOWN
};

inline std::string errorToString(Error error)
{
    switch (error) {
        case Error::INVALID_COMMAND: 
            return "invalid command";
        case Error::INVALID_ARGUMENTS_COUNT: 
            return "invalid arguments amount";
        case Error::INVALID_ARGUMENTS_VALUE: 
            return "invalid arguments value";
        case Error::DUPLICATE_ID: 
            return "duplicate id";
        case Error::TABLE_NOT_FOUND: 
            return "table not found";
        case Error::DATABASE_ERROR: 
            return "database error";
        case Error::UNKNOWN: 
            return "unknown error";
        default: 
            return "";
    }
}