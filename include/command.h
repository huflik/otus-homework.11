#pragma once

#include <string>

enum class Command
{
    INSERT,
    TRUNCATE,
    INTERSECTION,
    SYMMETRIC_DIFFERENCE,
    UNKNOWN
};

inline std::string commandToString(Command cmd)
{
    switch (cmd) {
        case Command::INSERT: 
            return "INSERT";
        case Command::TRUNCATE: 
            return "TRUNCATE";
        case Command::INTERSECTION: 
            return "INTERSECTION";
        case Command::SYMMETRIC_DIFFERENCE: 
            return "SYMMETRIC_DIFFERENCE";
        default: 
            return "UNKNOWN";
    }
}