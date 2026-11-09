#include "LibraryItem.h"
#include <iostream>

// Constructor
LibraryItem::LibraryItem(std::string title, std::string callNumber)
    : 
    title(title),
    callNumber(callNumber),
    isAvailable(true) // Book will always be available when we first create it
{}

// Setters
void LibraryItem::setTitle(const std::string& title) {
    this->title = title;
}
void LibraryItem::setAvailability(const bool isAvailable) {
    this->isAvailable = isAvailable;
}

// Getters
std::string LibraryItem::getTitle() const { return title; }
std::string LibraryItem::getCallNumber() const { return callNumber; }
bool LibraryItem::getAvailability() const { return isAvailable; }