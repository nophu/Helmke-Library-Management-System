#include "database.h"
#include <iostream>

// converts a genre enum into text so it can be saved in the database
static std::string genreToString(GenreType genre) {
    switch (genre) { // same idea as typeToString in Patron
        case GenreType::Nonfiction: return "Nonfiction";
        case GenreType::Fiction:    return "Fiction";
        case GenreType::Fantasy:    return "Fantasy";
        case GenreType::Mystery:    return "Mystery";
        case GenreType::Romance:    return "Romance";
    }
    return "Nonfiction"; // fallback case
}

// converts text from the database back into a genre enum
static GenreType genreFromString(const std::string& s) {
    // text matches enum value
    if (s == "Fiction") return GenreType::Fiction;
    if (s == "Fantasy") return GenreType::Fantasy;
    if (s == "Mystery") return GenreType::Mystery;
    if (s == "Romance") return GenreType::Romance;
    return GenreType::Nonfiction; // default = nonfiction
}

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
    std::cout << "Database opened successfully!\n";
    return true;
}

// close db connection
void Database::close() {
    // check if the .db is open first
    if (db) {
        sqlite3_close(db); // close the .db
        db = nullptr; // reset pointer; prevent dangling pointer
        std::cout << "Database closed.\n";
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
        "    call_number  TEXT PRIMARY KEY,"
        "    item_type    TEXT NOT NULL,"
        "    title        TEXT NOT NULL,"
        "    author       TEXT,"
        "    director     TEXT,"
        "    isbn         TEXT,"
        "    genre        TEXT,"
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
        "    item_call_number TEXT NOT NULL REFERENCES items(call_number),"
        "    patron_id      INTEGER NOT NULL REFERENCES patrons(patron_id),"
        "    checkout_date  TEXT NOT NULL,"
        "    due_date       TEXT NOT NULL,"
        "    return_date    TEXT"
        ");";
    return execSQL(schema);
}

// save one book to the items table (or update it if the call number already exists)
bool Database::insertOrUpdateBook(const Book& b) {
    // string = query, item_type is always 'book' here
    const char* sql = "INSERT OR REPLACE INTO items "
        "(call_number, item_type, title, author, isbn, genre, status) "
        "VALUES(?, 'book', ?, ?, ?, ?, ?)";

    // initialize statement as a pointer
    sqlite3_stmt *stmt = nullptr;

    // prepare the statement/query
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if statement could not be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // status is saved as text, not true/false
    std::string status = b.getAvailability() ? "available" : "checked_out";

    // binding each getter as parameters
    sqlite3_bind_text(stmt, 1, b.getCallNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, b.getTitle().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, b.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, b.getIsbn().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, genreToString(b.getGenre()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, status.c_str(), -1, SQLITE_TRANSIENT);

    // run the prepared statement
    int result = sqlite3_step(stmt);

    // throw error if insert failed
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert book: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    // clean up prepared statement
    sqlite3_finalize(stmt);
    return true;
}

bool Database::removeBook(const std::string& callNumber) {
    // Prepare query
    const char* sql = "DELETE FROM items "
        "WHERE call_number = ?";

    sqlite3_stmt *stmt = nullptr;

    // Prepare statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // Bind callNumber to statement
    sqlite3_bind_text(stmt, 1, callNumber.c_str(), -1, SQLITE_TRANSIENT);

    // Run query
    int result = sqlite3_step(stmt);

    // throw error if insert failed
    if (result != SQLITE_DONE) {
        std::cerr << "Failed to remove book: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return false;
    }

    // Checks if anything was actually deleted
    int changes = sqlite3_changes(db);

    // clean up prepared statement
    sqlite3_finalize(stmt);
    
    // Return false if no changes were made
    return changes > 0;
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

// query for getting all books saved in the items table
std::vector<Book> Database::getAllBooks() {
    std::vector<Book> result;

    // query, only rows that are books
    const char* sql = "SELECT call_number, title, author, isbn, genre, status "
        "FROM items WHERE item_type = 'book'";

    // prepare SQL statement
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // throw error if SQL statement could not be prepared
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return std::vector<Book>();
    }

    // while loop so that we can get ALL books since we do not know how many books exist
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // convert SQLite's raw text into strings
        std::string callNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string author = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string isbn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string genreStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        // convert the genre text back into enum type
        GenreType genre = genreFromString(genreStr);

        // create book object
        Book book(title, author, isbn, genre, callNumber, true);

        // the book constructor ignores availability, so we set it ourselves
        book.setAvailability(status == "available");

        // push book to result
        result.push_back(book);
    }
    sqlite3_finalize(stmt);
    return result;
}