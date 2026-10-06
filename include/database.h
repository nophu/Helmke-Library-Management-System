#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector> // for resizable arrays 
#include "Patron.h"

class Database {
private:
    sqlite3* db;
    std::string dbPath;

public:
    explicit Database(const std::string& path);
    ~Database();

    bool open();
    void close();
    bool initSchema();
    bool execSQL(const std::string& sql);

    bool insertOrUpdatePatron(const Patron& p);
    bool getPatronByID(int patronID, Patron& outPatron);
    std::vector<Patron> getAllPatrons();
    bool getPatronWithMostFines(Patron& outPatron);
};

#endif