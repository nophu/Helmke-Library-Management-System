#ifndef LIBRARY_H
#define LIBRARY_H

#include <memory>
#include <string>
#include <vector>

#include "LibraryItem.h"
#include "database.h"

class Library {
    private:
        Database database;

    public:
        Library();

        void addItem(std::shared_ptr<LibraryItem> item);
        bool removeItem(const std::string& callNumber);
        
        std::shared_ptr<LibraryItem> findItem(const std::string& callNumber);
        
        std::vector<std::shared_ptr<Book>> getBooks();
};

#endif
