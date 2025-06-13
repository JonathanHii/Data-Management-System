# Data Management System

A modern **C++ student record management system** featuring SQLite database integration and a built-in REST API server.



## Features

### Student Record Management
- Add, display, search, update, and delete student records.
- Each record includes:
  - **Roll Number**
  - **Name**
  - **Class**
  - **Total Marks**
  - **Obtained Marks**
  - **Percentage** (auto-calculated)

### Persistent Storage
- All data is stored in a local SQLite database (`students.db`) for durability and reliability.

### Console-Based User Interface
- Intuitive, menu-driven interface for managing records.
- Designed for **Windows** (uses `conio.h` for key input and `system("cls")` for screen clearing).

### Error Handling & Input Validation
- Handles invalid inputs and database errors gracefully.

### Automatic Database Backup
- Creates timestamped backup copies of the database file.
- Backups run in the background using a separate thread, keeping the main application responsive.
- Backups are stored in a `backup/` directory with filenames like `backup_YYYYMMDD_HHMMSS.db`.

### REST API Server
- Exposes student records via a RESTful API using [cpp-httplib](https://github.com/yhirose/cpp-httplib).
- Runs on a separate thread, allowing simultaneous use of the console UI and API.

#### API Endpoints
| Method | Endpoint                 | Description                                      |
|--------|-------------------------|--------------------------------------------------|
| GET    | `/students`             | Returns a JSON array of all student records      |
| GET    | `/students/{roll}`      | Returns a JSON object for the specified student  |
| POST   | `/students`             | Adds a new student record (expects JSON body)    |
| DELETE | `/students/{roll}`      | Deletes the student record with the given roll   |

- **Server Details:**  
  Runs on [`http://localhost:8080`](http://localhost:8080)  
  Allows integration with web and mobile applications.



## How to Build

1. Ensure you have a C++ compiler and SQLite3 development files installed.
2. Compile the project (example for g++ on Windows):

   > If using dynamic linking, ensure `sqlite3.dll` is in the same directory as the executable.



## How to Run

- Run `main.exe` from the command line.
- The REST API server starts automatically in the background.
- Use the on-screen menu to manage student records via the console.
- You can also interact with the REST API using tools like [Postman](https://www.postman.com/) or `curl`.



## Portability

- Designed for **Windows** (`conio.h` and `system("cls")`).
- To run on another computer, copy:
  - The `.exe` file
  - `sqlite3.dll`
  - `students.db` (if you want to preserve existing data)



## Example REST API Usage

### Get all students
```
GET http://localhost:8080/students
```

### Get a student by roll
```
GET http://localhost:8080/students/101
```

### Add a student
```
POST http://localhost:8080/students
Content-Type: application/json

{
  "roll": "102",
  "name": "Bob",
  "class": "10B",
  "total": 500,
  "obtained": 420
}
```

### Delete a student
```
DELETE http://localhost:8080/students/102
```