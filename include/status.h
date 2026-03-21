#pragma once

#include <string>

enum class Status
{
    OK,
    ERROR,
    LOAD,
    SKIP
};

inline std::string statusToString(Status status)
{
    switch (status) {
        case Status::OK: 
            return "OK";
        case Status::ERROR: 
            return "ERROR";
        case Status::LOAD: 
            return "LOAD";
        case Status::SKIP: 
            return "SKIP";
        default: 
            return "UNKNOWN";
    }
}