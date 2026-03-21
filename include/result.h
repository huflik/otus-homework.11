#pragma once

#include <string>
#include <vector>
#include "status.h"
#include "error.h"
#include "command.h"

struct Result
{
    Status status;
    Error error;
    std::string errorMessage;
    
    Result() : status(Status::OK), error(Error::NONE) {}
    Result(Status s) : status(s), error(Error::NONE) {}
    Result(Error e, const std::string& msg = "") 
        : status(Status::ERROR), error(e), errorMessage(msg) {}
    
    bool isOk() const { return status == Status::OK; }
    bool isError() const { return status == Status::ERROR; }
    bool isLoad() const { return status == Status::LOAD; }
    
    std::string toString() const {
        if (status == Status::OK) {
            return "OK";
        } else if (status == Status::ERROR) {
            if (!errorMessage.empty()) {
                return "ERR " + errorMessage;
            }
            return "ERR " + errorToString(error);
        }
        return "";
    }
};

struct ParseResult
{
    Command command;
    Result result;
    std::vector<std::string> tokens;
    
    ParseResult() : command(Command::UNKNOWN) {}
    ParseResult(Command cmd, const std::vector<std::string>& tok) 
        : command(cmd), tokens(tok) {}
    ParseResult(const Result& res) : command(Command::UNKNOWN), result(res) {}
    
    bool IsValid() const { return result.isOk(); }
    bool IsError() const { return result.isError(); }
};

struct ExecutionResult
{
    Result result;
    std::string data;
    
    ExecutionResult() = default;
    ExecutionResult(const Result& res) : result(res) {}
    ExecutionResult(const std::string& d) : result(Status::LOAD), data(d) {}
};

struct JoinResult
{
    int id;
    std::string nameA;
    std::string nameB;
};