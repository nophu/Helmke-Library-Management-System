#include "Book.h"

Book::Book(std::string title, std::string author, std::string isbn, GenreType genre, std::string callNumber, bool isAvailable)
    : 
    LibraryItem(title, callNumber), 
    author(author), 
    isbn(isbn), 
    genre(genre) 
{}

void Book::setAuthor(const std::string& author) {
    this->author = author;
}

void Book::setGenre(const std::string& genre) {
    if (genre == "Nonfiction") 
        this->genre = GenreType::Nonfiction;
    else if (genre == "Fiction") 
        this->genre = GenreType::Fiction;
    else if (genre == "Fantasy") 
        this->genre = GenreType::Fantasy;
    else if (genre == "Mystery") 
        this->genre = GenreType::Mystery;
    else if (genre == "Romance") 
        this->genre = GenreType::Romance;
}

std::string Book::getAuthor() const { return author; }
std::string Book::getIsbn() const { return isbn; }
GenreType Book::getGenre() const { return genre; }
