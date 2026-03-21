#include <sstream>
#include "dbmanager.h"

DbManager& DbManager::GetInstance()
{
    static DbManager instance;
    return instance;
}

DbManager::DbManager() : db_(nullptr), dbPath_("join_server.db"), initialized_(false) {}

DbManager::~DbManager()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (db_) {
        sqlite3_close(db_);
    }
}

Result DbManager::Initialize()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return Result(Status::OK);
    }
    
    if (sqlite3_open(dbPath_.c_str(), &db_) != SQLITE_OK) {
        return Result(Error::DATABASE_ERROR, "Can't open database: " + std::string(sqlite3_errmsg(db_)));
    }
    
    const char* createTables = 
        "CREATE TABLE IF NOT EXISTS A ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL);"
        "CREATE TABLE IF NOT EXISTS B ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL);";
    
    if (!ExecuteQuery(createTables)) {
        return Result(Error::DATABASE_ERROR, "Failed to create tables");
    }
    
    initialized_ = true;
    return Result(Status::OK);
}

bool DbManager::ExecuteQuery(const std::string& query, std::string* errorMsg)
{
    if (!db_) {
        if (errorMsg) *errorMsg = "database not initialized";
        return false;
    }
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        if (errorMsg && errMsg) {
            *errorMsg = errMsg;
        }
        if (errMsg) {
            sqlite3_free(errMsg);
        }
        return false;
    }
    
    return true;
}

ExecutionResult DbManager::Execute(Command cmd, const std::vector<std::string>& tokens)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!db_) {
        return ExecutionResult(Result(Error::DATABASE_ERROR, "database not initialized"));
    }
    
    switch (cmd) {
        case Command::INSERT:
            return ExecutionResult(HandleInsert(tokens));
        case Command::TRUNCATE:
            return ExecutionResult(HandleTruncate(tokens));
        case Command::INTERSECTION:
            return ExecutionResult(FormatJoinResults(GetIntersection()));
        case Command::SYMMETRIC_DIFFERENCE:
            return ExecutionResult(FormatJoinResults(GetSymmetricDifference()));
        default:
            return ExecutionResult(Result(Error::UNKNOWN, "unhandled command"));
    }
}

Result DbManager::HandleInsert(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 4) {
        return Result(Error::INVALID_ARGUMENTS_COUNT, "not enough arguments");
    }
    
    std::string table = tokens[1];
    int id = std::stoi(tokens[2]);
    std::string name = tokens[3];
    
    std::ostringstream checkQuery;
    checkQuery << "SELECT id FROM " << table << " WHERE id = " << id << ";";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, checkQuery.str().c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return Result(Error::DATABASE_ERROR, "failed to check existence");
    }
    
    bool exists = false;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        exists = true;
    }
    
    sqlite3_finalize(stmt);
    
    if (exists) {
        return Result(Error::DUPLICATE_ID, "duplicate " + std::to_string(id));
    }
    
    std::ostringstream query;
    query << "INSERT INTO " << table << " (id, name) VALUES (" 
          << id << ", '" << name << "');";
    
    std::string error;
    if (!ExecuteQuery(query.str(), &error)) {
        return Result(Error::DATABASE_ERROR, error.empty() ? "insert failed" : error);
    }
    
    return Result(Status::OK);
}

Result DbManager::HandleTruncate(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        return Result(Error::INVALID_ARGUMENTS_COUNT, "not enough arguments");
    }
    
    std::string table = tokens[1];
    
    std::ostringstream query;
    query << "DELETE FROM " << table << ";";
    
    if (!ExecuteQuery(query.str())) {
        return Result(Error::DATABASE_ERROR, "failed to truncate table " + table);
    }
    
    return Result(Status::OK);
}

std::vector<JoinResult> DbManager::GetIntersection()
{
    std::vector<JoinResult> results;
    const char* query = 
        "SELECT A.id, A.name, B.name FROM A "
        "INNER JOIN B ON A.id = B.id "
        "ORDER BY A.id;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return results;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        JoinResult row;
        row.id = sqlite3_column_int(stmt, 0);
        
        const char* nameA = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row.nameA = nameA ? nameA : "";
        
        const char* nameB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row.nameB = nameB ? nameB : "";
        
        results.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    
    return results;
}

std::vector<JoinResult> DbManager::GetSymmetricDifference()
{
    std::vector<JoinResult> results;
    const char* query = 
        "SELECT id, name, '' FROM A WHERE id NOT IN (SELECT id FROM B) "
        "UNION "
        "SELECT id, '', name FROM B WHERE id NOT IN (SELECT id FROM A) "
        "ORDER BY id;";
    
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, query, -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return results;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        JoinResult row;
        row.id = sqlite3_column_int(stmt, 0);
        
        const char* nameA = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row.nameA = nameA ? nameA : "";
        
        const char* nameB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row.nameB = nameB ? nameB : "";
        
        results.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    
    return results;
}

std::string DbManager::FormatJoinResults(const std::vector<JoinResult>& results)
{
    std::ostringstream oss;
    for (const auto& row : results) {
        oss << "< " << row.id << "," << row.nameA << "," << row.nameB << "\n";
    }
    return oss.str();
}