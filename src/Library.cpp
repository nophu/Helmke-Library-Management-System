#include <iostream>

#include "Library.h"
#include "database.h"

Library::Library() : database("library.db") {
    if (database.open()) {
        database.initSchema();
    }
}

// TODO : Fill in methods
void Library::addItem(std::shared_ptr<LibraryItem> item) {
    std::shared_ptr<Book> book = std::dynamic_pointer_cast<Book>(item);

    // Insert book if not nullptr
    if (book) {
        database.insertOrUpdateBook(*book);
    }

    std::cout << "book added successfully!\n";
}

bool Library::removeItem(const std::string& callNumber) {
    return database.removeBook(callNumber);
}
        
/*std::shared_ptr<LibraryItem> Library::findItem(const std::string& callNumber) {
    
}*/
        
std::vector<std::shared_ptr<Book>> Library::getBooks() {
    std::vector<std::shared_ptr<Book>> items;

    // Query database for all books
    std::vector<Book> books = database.getAllBooks();

    // Add to items list
    for (const Book& book : books) {
        items.push_back(std::make_shared<Book>(book));
    }
    
    return items;
}

bool Library::addPatron(const Patron& patron) {
    return database.insertOrUpdatePatron(patron);
}

std::vector<Patron> Library::getPatrons() {
    return database.getAllPatrons();
}
