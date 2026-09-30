#ifndef PATRON_H
#define PATRON_H

#include <string>
#include <vector> // for resizable arrays

enum class PatronType { Student, Faculty, Staff };

// 'Class' instead of Struct because Class is private by default while Struct is public by default, and we need to protect data like fines & borrow limits
class Patron {
private:
    int patronID;
    std::string name;
    std::string email;
    PatronType type;
    int borrowLimit;
    double finesOwed;
    std::vector<int> borrowedItemIDs;

    static constexpr double FINE_CAP = 25.00; // maybe changes this ?? 
    static constexpr double BLOCK_THRESHOLD = 10.00; // maybe change this too ?? 

public:

    Patron(int id, const std::string& name, const std::string& email, PatronType type);

    // full constructor/initializer, used for rebuilding a patron with already-known values (since we'd be loading patron information from the database)
    Patron(int id, const std::string& name, const std::string& email, PatronType type, int borrowLimit, double finesOwed);

    void setName(const std::string& newName);
    void setEmail(const std::string& newEmail);
    void setType(PatronType newType);

    void addBorrowedItem(int itemID);
    void removeBorrowedItem(int itemID);
    bool isAtBorrowLimit() const;

    void addFine(double amount);
    void payFine(double amount);
    bool isBlockedForFines() const;

    void displayInfo() const;

    int getID() const;
    std::string getName() const;
    std::string getEmail() const;
    PatronType getType() const;
    int getBorrowLimit() const;
    double getFinesOwed() const;
    const std::vector<int>& getBorrowedItems() const;

    static int defaultBorrowLimit(PatronType type);
    static std::string typeToString(PatronType type);
    static PatronType typeFromString(const std::string& s);
};

#endif