# Data Management System

A simple C++ student record management system with SQLite database integration.

## Features

- Student record management (add, display, search, update, delete)
- Persistent storage using SQLite
- Console-based user interface
- Error handling and input validation
- **Automatic Backup:**  
  The system periodically creates a backup copy of the database file in the background using a separate thread.
- **Role-Based Access Control:**  
  User login (admin/teacher/student) with different permissions, demonstrating security and multi-user design.
- **REST API Server:**  
  Exposes the database as a REST API (using C++ frameworks like Pistache or Crow), making the project accessible to web and mobile apps. The server runs on a separate thread.

## How to Build

1. Make sure you have a C++ compiler and SQLite3 development files installed.
2. Compile the project (example for g++ on Windows):

   - If using dynamic linking, ensure `sqlite3.dll` is in the same directory as the executable.

## How to Run

- Run `main.exe` from the command line.
- Follow the on-screen menu to manage student records.

## Portability

- The program is designed for Windows (uses `conio.h` and `system("cls")`).
- To run on another computer, copy:
  - The `.exe` file
  - `sqlite3.dll`
  - `students.db` (if you want to preserve existing data)

---

## Roadmap

- [x] SQLite database integration
- [x] Automatic backup (multithreaded)
- [ ] Role-based access control
- [ ] REST API server

---

*Work in progress. Contributions and suggestions welcome!*
