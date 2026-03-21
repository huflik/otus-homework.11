#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "sqlite3.h"
#include "result.h"
#include "command.h"

class DbManager
{
public:
    static DbManager& GetInstance();
    
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;
    
    Result Initialize();
    ExecutionResult Execute(Command cmd, const std::vector<std::string>& tokens);
    
private:
    DbManager();
    ~DbManager();
    
    bool ExecuteQuery(const std::string& query, std::string* errorMsg = nullptr);
    
    Result HandleInsert(const std::vector<std::string>& tokens);
    Result HandleTruncate(const std::vector<std::string>& tokens);
    
    std::vector<JoinResult> GetIntersection();
    std::vector<JoinResult> GetSymmetricDifference();
    std::string FormatJoinResults(const std::vector<JoinResult>& results);
    
    sqlite3* db_;
    std::string dbPath_;
    mutable std::mutex mutex_;
    bool initialized_;
};