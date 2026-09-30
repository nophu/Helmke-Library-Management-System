#include "Database.h"
#include <iostream>

// constructor
Database::Database(const std::string& path) : db(nullptr), dbPath(path) {
}

// destructor since we should never leave a db connection open once we finish using it
Database::~Database() { close(); }

// open or create the .db file from SQLite
bool Database::open() {
    // try to open the .db
    int rc = sqlite3_open(dbPath.c_str(), &db);

    // throw error if .db cannot open
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    return true;
}

// close db connection
void Database::close() {
    // check if the .db is open first
    if (db) {
        sqlite3_close(db); // close the .db
        db = nullptr; // reset pointer; prevent dangling pointer
    }
}

// runs SQL queries from strings
bool Database::execSQL(const std::string& sql) {
    // pointer for handling error case
    char* errMsg = nullptr;

    // try to execute the query
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    // throw error if SQL does not run
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// create schemas, just holds the queries to create the tables
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

// insert or update queries
bool Database::insertOrUpdatePatron(const Patron& p) {
    // string = query
    const char* sql = "INSERT OR REPLACE INTO patrons "
        "(patron_id, name, email, patron_type, borrow_limit, fines_owed) "
        "VALUES(?, ?, ?, ?, ?, ?)";

    // initialize statement as a pointer
    sqlite3_stmt *stmt = nullptr;

    // prepare the statement/query
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if statement could not be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // binding each getter as parameters
    sqlite3_bind_int(stmt, 1, p.getID());
    sqlite3_bind_text(stmt, 2, p.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, p.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, Patron::typeToString(p.getType()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, p.getBorrowLimit());
    sqlite3_bind_double(stmt, 6, p.getFinesOwed());

    // run the prepared statement
    int result = sqlite3_step(stmt);

    // throw error if insert failed
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert patrons: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    // clean up prepared statement
    sqlite3_finalize(stmt);
    return result == SQLITE_DONE;
}

// query for finding all information on patron based on patron id
bool Database::getPatronByID(int patronID, Patron& outPatron) {
    // query
    const char* sql = "SELECT patron_id, name, email, patron_type, borrow_limit, fines_owed "
        "FROM patrons WHERE patron_id = ? ";

    // prepare SQL statement
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if SQL statement could not be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // bind patron id
    sqlite3_bind_int(stmt, 1, patronID);

    // run SQL statement
    int result = sqlite3_step(stmt);

    // if a row was found, pull out all columns (6 in our situation) to build the Patron
    if (result == SQLITE_ROW) {
        // pull raw values out of the row
        int id = sqlite3_column_int(stmt, 0);

        // reinterpret_cast = conversion of SQLite's raw text pointer into normal strings
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string typeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // since the type is correct, we do not need the reinterpret_cast for converting
        int borrowLimit = sqlite3_column_int(stmt, 4);
        double finesOwed = sqlite3_column_double(stmt, 5);

        // convert the string that represents the patron type back into enum type
        PatronType type = Patron::typeFromString(typeStr);

        // build the patron object
        outPatron = Patron(id, name, email, type, borrowLimit, finesOwed);

        // clean up SQL statement
        sqlite3_finalize(stmt);
        return true;
    }

    // clean up SQL statement
    sqlite3_finalize(stmt);
    return false;
}

// query for getting all information for all patrons
std::vector<Patron> Database::getAllPatrons() {
    std::vector<Patron> result;

    // query
    const char* sql = "SELECT patron_id, name, email, patron_type, borrow_limit, fines_owed "
        "FROM patrons";

    // prepare SQL statement
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if SQL statement could not be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return std::vector<Patron>();
    }

    // while loop so that we can get ALL patrons since we do not know how many patrons exist
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // pull raw values from each column per patron
        int id = sqlite3_column_int(stmt, 0);

        // convert SQLite's raw text into strings
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string typeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // no need to cast since they are already the correct type
        int borrowLimit = sqlite3_column_int(stmt, 4);
        double finesOwed = sqlite3_column_double(stmt, 5);

        // convert the string that represents the patron type back into enum type
        PatronType type = Patron::typeFromString(typeStr);

        // create patron object
        Patron p(id, name, email, type, borrowLimit, finesOwed);

        // push patron to result
        result.push_back(p);
    }
    sqlite3_finalize(stmt);
    return result;
}

// query for finding what patron has the most fines
bool Database::getPatronWithMostFines(Patron& outPatron) {
    // query
    const char* sql = "SELECT patron_id, name, email, patron_type, borrow_limit, fines_owed "
        "FROM patrons ORDER BY fines_owed DESC LIMIT 1";

    // prepare SQL statement
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if SQL statement cannot be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // run prepared statement
    int result = sqlite3_step(stmt);

    // if a row was found, pull out all columns (6 in our situation) to build the Patron
    if (result == SQLITE_ROW) {
        // pull raw values from each column per patron
        int id = sqlite3_column_int(stmt, 0);

        // convert SQLite's raw text into strings
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string typeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // no need to cast since they are already the correct type
        int borrowLimit = sqlite3_column_int(stmt, 4);
        double finesOwed = sqlite3_column_double(stmt, 5);

        // convert the string that represents the patron type back into enum type
        PatronType type = Patron::typeFromString(typeStr);

        // create patron object
        Patron p(id, name, email, type, borrowLimit, finesOwed);
        outPatron = p;

        // clean up prepared statement
        sqlite3_finalize(stmt);
        return true;
    }

    // clean up prepared statement
    sqlite3_finalize(stmt);
    return false;
}