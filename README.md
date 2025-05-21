# Data Management System

A simple C++ student record management system with SQLite database integration.  
**Upcoming features:** automatic database backup and a live statistics window, both powered by multithreading.

## Features

- Student record management (add, display, search, update, delete)
- Persistent storage using SQLite
- Console-based user interface
- Error handling and input validation

## Planned Features

- **Automatic Backup:**  
  The system will periodically create a backup copy of the database file in the background using a separate thread.
- **Live Statistics Window:**  
  A real-time statistics display (e.g., total students, average percentage) will update automatically in a separate thread, providing instant insights as data changes.

## How to Build

1. Make sure you have a C++ compiler and SQLite3 development files installed.
2. Compile the project (example for g++ on Windows):

- If using dynamic linking, ensure `sqlite3.dll` is in the same directory as the executable.

## How to Run

- Run `DataManagementSystem.exe` from the command line.
- Follow the on-screen menu to manage student records.

## Portability

- The program is designed for Windows (uses `conio.h` and `system("cls")`).
- To run on another computer, copy:
  - The `.exe` file
  - `sqlite3.dll` (if dynamically linked)
  - `students.db` (if you want to preserve existing data)

## Roadmap

- [x] SQLite database integration
- [ ] Automatic backup (multithreaded)
- [ ] Live statistics window (multithreaded)
- [ ] Simulated tests for large datasets

---

*Work in progress. Contributions and suggestions welcome!*