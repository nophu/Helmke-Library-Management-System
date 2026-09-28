# Helmke Library Management System

A menu-driven C++ program for CS 33100 that helps Helmke Library staff catalog items, register patrons, process checkouts and returns, and track overdue fines — replacing manual tracking with spreadsheets or paper logs.

## Team

| Name | Subsystem |
|---|---|
| Bennet Ripplinger | Catalog & Item Classes |
| Nolan Phuong | Patrons, Fines & Persistence (Database) |
| Alexander Yauchler | Circulation & System Core |

## Features

**Catalog Management**
- Add/edit/remove items (title, author, ISBN, genre, call number)
- Track item status: available, checked out
- Two item types — `Book` and `DVD` — via inheritance/polymorphism

**Patron Management**
- Register/edit patrons (name, student ID, email, patron type — student/faculty/staff)
- Track each patron's currently borrowed items
- Borrowing limits by patron type (students: 5 items, faculty: 15)

**Checkout / Circulation**
- Check out and return items
- Due dates that vary by patron type (students: 3 weeks, faculty: a semester)

**Fines & Fees**
- Overdue fine calculation (flat rate per day)
- Fine caps (never more than replacement cost)
- Pay off outstanding fines
- Checkout blocked if a patron owes too much

**Search & Browsing**
- Search catalog by title, author, genre, or call number
- Filter by availability

**Reports**
- Currently overdue items
- Patron with the most outstanding fines

**Persistence**
- SQLite database (`data/library.db`) — no server required, single local file
- Saves/loads catalog, patrons, and transaction history automatically

### Stretch goals (post-core)
Renewals, reservation/hold queue, operator overloading, staff login vs. basic view, additional reports (most/never checked out).

## Tech stack

- C++17
- SQLite3 (`libsqlite3-dev`) for persistence — no server, single `.db` file
- Standard library containers (`vector`, etc.) for in-memory catalog/patron/transaction data

## Project structure

```
include/          Header files (class declarations)
  LibraryItem.h      base class for catalog items
  Book.h, DVD.h      derived item types
  Patron.h           patron/member class
  Transaction.h      checkout/return records
  Library.h          manager class tying everything together
  Database.h         SQLite persistence layer

src/              Implementation files
  LibraryItem.cpp, Book.cpp, DVD.cpp
  Patron.cpp
  Transaction.cpp
  Library.cpp
  Database.cpp
  main.cpp           menu-driven entry point

data/             library.db lives here (created automatically on first run)

Makefile
README.md
```

## Setup

Requires a C++17 compiler and SQLite3 dev headers.

```bash
# Linux / WSL
sudo apt install libsqlite3-dev

# Mac (Homebrew)
brew install sqlite3
```

## Build & run

```bash
make
./library_system
```
