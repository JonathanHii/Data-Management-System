# Data Management System

A simple C++ student record management system with SQLite database integration.  
**Upcoming features:** automatic database backup, a live statistics window, role-based access control, and a REST API server—all powered by multithreading.

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
- **Role-Based Access Control:**  
  Add user login (admin/teacher/student) with different permissions, demonstrating security and multi-user design.
- **REST API Server:**  
  Expose your database as a REST API (using C++ frameworks like Pistache or Crow), making your project accessible to web and mobile apps. The server will run on a separate thread.

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

## Roadmap

- [x] SQLite database integration
- [ ] Automatic backup (multithreaded)
- [ ] Live statistics window (multithreaded)
- [ ] Simulated tests for large datasets
- [ ] Role-based access control
- [ ] REST API server

---

*Work in progress. Contributions and suggestions welcome!*
