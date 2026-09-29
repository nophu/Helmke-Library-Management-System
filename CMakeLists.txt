cmake_minimum_required(VERSION 3.20)
project(Helmke_Library_Management_System CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)

add_executable(Helmke_Library_Management_System
        main.cpp
        Patron.cpp
        database.cpp
        Book.cpp
        LibraryItem.cpp
        database.cpp
)