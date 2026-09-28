#include "Patron.h"
#include <iostream>

// constructor
Patron::Patron(int id, const std::string& name, const std::string& email, PatronType type)
    : patronID(id),
    name(name),
    email(email),
    type(type),
    borrowLimit(defaultBorrowLimit(type)), // borrow limit is based on patron type, e.g. staff / student
    finesOwed(0.0) { // starting balance as a default is 0
}

// setter for patron name
void Patron::setName(const std::string& newName) {
    name = newName;
}

// setter for patron email
void Patron::setEmail(const std::string& newEmail) {
    email = newEmail;
}

// setter for patron type
void Patron::setType(PatronType newType) {
    type = newType;
}

// add an item to a specific patron's borrowed list
void Patron::addBorrowedItem(int itemID) {

}

// remove an item on a specific patron's borrowed list
void Patron::removeBorrowedItem(int itemID) {

}

// check if patron is at borrow limit
bool Patron::isAtBorrowLimit() const {

    return false;
}

// issue a fine to a specific patron
void Patron::addFine(double amount) {

}

// patron can pay for a fine
void Patron::payFine(double amount) {

}

// checking if patron is blocked due to fines
bool Patron::isBlockedForFines() const {

    return false;
}

// prints all info related to a specific patron
void Patron::displayInfo() const {
    std::cout << "[Patron::displayInfo not implemented]\n";
}

// getter methods
int Patron::getID() const {
    return patronID;
}

std::string Patron::getName() const {
    return name;
}

std::string Patron::getEmail() const {
    return email;
}

PatronType Patron::getType() const {
    return type;
}

int Patron::getBorrowLimit() const {
    return borrowLimit;
}

double Patron::getFinesOwed() const {
    return finesOwed;
}

const std::vector<int>& Patron::getBorrowedItems() const {
    return borrowedItemIDs;
}

// borrow limits for each patron type
int Patron::defaultBorrowLimit(PatronType type) {
    switch (type) {
        case PatronType::Student: return 5;
        case PatronType::Faculty: return 15;
        case PatronType::Staff:   return 10;
    }
    return 5;
}

// toString for printing out the patron type since it is an enum type
std::string Patron::typeToString(PatronType type) {
    switch (type) {
        case PatronType::Student: return "student";
        case PatronType::Faculty: return "faculty";
        case PatronType::Staff:   return "staff";
    }
    return "student";
}

// string -> enum since we are loading information from a database
PatronType Patron::typeFromString(const std::string& s) {
    if (s == "faculty") return PatronType::Faculty;
    if (s == "staff") return PatronType::Staff;
    return PatronType::Student;
}