#include <sstream>
#include <algorithm>
#include <cctype>
#include "parser.h"

ParseResult Parser::Parse(const std::string& data)
{
    std::vector<std::string> tokens;
    std::istringstream iss(data);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return ParseResult(Result(Error::INVALID_COMMAND, "empty command"));
    }
    
    std::string cmdStr = tokens[0];
    std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::tolower);
    
    auto it = cmdMap_.find(cmdStr);
    if (it == cmdMap_.end()) {
        return ParseResult(Result(Error::INVALID_COMMAND, "unknown command: " + tokens[0]));
    }
    
    Command cmd = it->second;
    Result validationResult;
    
    switch (cmd) {
        case Command::INSERT:
            if (!ValidateInsert(tokens, validationResult)) {
                return ParseResult(validationResult);
            }
            break;
        case Command::TRUNCATE:
            if (!ValidateTruncate(tokens, validationResult)) {
                return ParseResult(validationResult);
            }
            break;
        case Command::INTERSECTION:
        case Command::SYMMETRIC_DIFFERENCE:
            if (!ValidateJoinCommand(tokens, validationResult)) {
                return ParseResult(validationResult);
            }
            break;
        default:
            return ParseResult(Result(Error::UNKNOWN, "unhandled command"));
    }
    
    return ParseResult(cmd, tokens);
}

bool Parser::ValidateInsert(const std::vector<std::string>& tokens, Result& result)
{
    if (tokens.size() != 4) {
        result = Result(Error::INVALID_ARGUMENTS_COUNT, "INSERT requires 3 arguments (table id name)");
        return false;
    }
    
    if (tokens[1] != "A" && tokens[1] != "B") {
        result = Result(Error::TABLE_NOT_FOUND, "table must be A or B");
        return false;
    }
    
    try {
        std::stoi(tokens[2]);
    } catch (...) {
        result = Result(Error::INVALID_ARGUMENTS_VALUE, "id must be a number");
        return false;
    }
    
    return true;
}

bool Parser::ValidateTruncate(const std::vector<std::string>& tokens, Result& result)
{
    if (tokens.size() != 2) {
        result = Result(Error::INVALID_ARGUMENTS_COUNT, "TRUNCATE requires 1 argument (table)");
        return false;
    }
    
    if (tokens[1] != "A" && tokens[1] != "B") {
        result = Result(Error::TABLE_NOT_FOUND, "table must be A or B");
        return false;
    }
    
    return true;
}

bool Parser::ValidateJoinCommand(const std::vector<std::string>& tokens, Result& result)
{
    if (tokens.size() != 1) {
        result = Result(Error::INVALID_ARGUMENTS_COUNT, "command requires no arguments");
        return false;
    }
    return true;
}