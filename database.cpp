#include "Database.h"
#include <iostream>

Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
}

Database::~Database() {
    close();
}

bool Database::open() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::execSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::initSchema() {
    const std::string schema =
        "CREATE TABLE IF NOT EXISTS items ("
        "    item_id      INTEGER PRIMARY KEY,"
        "    item_type    TEXT NOT NULL,"
        "    title        TEXT NOT NULL,"
        "    author       TEXT,"
        "    director     TEXT,"
        "    isbn         TEXT,"
        "    genre        TEXT,"
        "    call_number  TEXT,"
        "    runtime_min  INTEGER,"
        "    status       TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS patrons ("
        "    patron_id     INTEGER PRIMARY KEY,"
        "    name          TEXT NOT NULL,"
        "    email         TEXT,"
        "    patron_type   TEXT NOT NULL,"
        "    borrow_limit  INTEGER NOT NULL,"
        "    fines_owed    REAL NOT NULL DEFAULT 0"
        ");"
        "CREATE TABLE IF NOT EXISTS transactions ("
        "    transaction_id INTEGER PRIMARY KEY,"
        "    item_id        INTEGER NOT NULL REFERENCES items(item_id),"
        "    patron_id      INTEGER NOT NULL REFERENCES patrons(patron_id),"
        "    checkout_date  TEXT NOT NULL,"
        "    due_date       TEXT NOT NULL,"
        "    return_date    TEXT"
        ");";
    return execSQL(schema);
}

bool Database::insertOrUpdatePatron(const Patron& p) {
    return false;
}

bool Database::getPatronByID(int patronID, Patron& outPatron) {
    return false;
}

std::vector<Patron> Database::getAllPatrons() {
    std::vector<Patron> result;
    return result;
}

bool Database::getPatronWithMostFines(Patron& outPatron) {
    return false;
}