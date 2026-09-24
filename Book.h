#ifndef BOOK_H
#define BOOK_H

#include <string>

#include "LibraryItem.h"

// Change to genre list from an API in the future
enum class GenreType { 
    Nonfiction, Fiction, Fantasy, Mystery, Romance 
};

class Book : public LibraryItem {
    private:
        std::string author;
        std::string isbn;
        GenreType genre;

    public:
        Book(std::string title, std::string author, std::string isbn, GenreType genre, std::string callNumber, bool isAvailable);

        void setAuthor(const std::string& author);
        void setGenre(const std::string& genre);

        std::string getAuthor() const;
        std::string getIsbn() const;
        GenreType getGenre() const;
};

#endif