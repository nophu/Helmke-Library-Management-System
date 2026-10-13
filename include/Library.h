#ifndef LIBRARY_H
#define LIBRARY_H

#include <memory>
#include <string>
#include <vector>

#include "LibraryItem.h"

class Library {
    private:
        std::vector<std::shared_ptr<LibraryItem>> items;

    public:
        void addItem(std::shared_ptr<LibraryItem> item);
        bool removeItem(const std::string& callNumber);
        
        std::shared_ptr<LibraryItem> findItem(const std::string& callNumber);
        
        const std::vector<std::shared_ptr<LibraryItem>>& getItems();
};

#endif
