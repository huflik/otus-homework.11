#pragma once

#include <string>
#include <vector>
#include <map>
#include "command.h"
#include "result.h"

class Parser
{
public:
    Parser() = default;
    ~Parser() = default;
    
    ParseResult Parse(const std::string& data);

private:
    bool ValidateInsert(const std::vector<std::string>& tokens, Result& result);
    bool ValidateTruncate(const std::vector<std::string>& tokens, Result& result);
    bool ValidateJoinCommand(const std::vector<std::string>& tokens, Result& result);
    
    std::map<std::string, Command> cmdMap_ = {
        {"insert", Command::INSERT},
        {"truncate", Command::TRUNCATE},
        {"intersection", Command::INTERSECTION},
        {"symmetric_difference", Command::SYMMETRIC_DIFFERENCE}
    };
};