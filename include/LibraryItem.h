#ifndef LIBRARYITEM_H
#define LIBRARYITEM_H

#include <string>

class LibraryItem {
    private:
        std::string title;
        std::string callNumber;
        bool isAvailable;

    public:
        LibraryItem(std::string title, std::string callNumber, bool isAvailable);

        void setTitle(const std::string& title);
        void setAvailability(const bool isAvailable);

        std::string getTitle() const;
        std::string getCallNumber() const;
        bool getAvailability() const;
};

#endif