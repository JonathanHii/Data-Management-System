#include <iostream>
#include <string>
#include <sqlite3.h>
#include <conio.h>

#include <thread>
#include <chrono>
#include <filesystem>
#include "../cpp-httplib/httplib.h"
#include "../cpp-httplib/json.hpp"
using json = nlohmann::json;

using namespace std;

// Model
//-------------------
class Student
{
public:
    string roll;
    string name;
    string clas;
    float total;
    float obtained;
    float percentage;

    Student() = default;

    Student(string r, string n, string c, float tot, float obt)
        : roll(r), name(n), clas(c), total(tot), obtained(obt)
    {
        percentage = (total != 0) ? (obtained / total) * 100.0f : 0;
    }
};

// Database Handler
//-------------------
class Database
{
private:
    sqlite3 *db;

public:
    Database() : db(nullptr) {} // set private db default to nullptr

    bool open(const string &dbName)
    {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK)
        {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
            return false;
        }
        return true;
    }

    void close()
    {
        if (db)
        {
            sqlite3_close(db); // frees recourses
            db = nullptr;
        }
    }

    bool createTable()
    {
        const char *sql =
            "CREATE TABLE IF NOT EXISTS students ("
            "roll TEXT PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "class TEXT NOT NULL, "
            "total REAL NOT NULL, "
            "obtained REAL NOT NULL, " // REAL is float numbers
            "percentage REAL NOT NULL);";

        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg); // executes sql code
        if (rc != SQLITE_OK)
        {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    sqlite3 *getConnection() { return db; }

    bool backup(const std::string &backupFileName)
    {
        try
        {
            // Using std::filesystem
            std::filesystem::copy_file("students.db", backupFileName, std::filesystem::copy_options::overwrite_existing);
            return true;
        }
        catch (std::filesystem::filesystem_error &e)
        {
            cerr << "Backup failed: " << e.what() << endl;
            return false;
        }
    }
};

// Student Data Access
//-------------------
class StudentService
{
private:
    Database &database;

public:
    explicit StudentService(Database &db) : database(db) {}

    bool insert(const Student &s) // takes a struct and inserts it into db
    {
        const char *sql =
            "INSERT INTO students (roll, name, class, total, obtained, percentage) "
            "VALUES (?, ?, ?, ?, ?, ?);";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(database.getConnection(), sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, s.roll.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, s.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, s.clas.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, s.total);
        sqlite3_bind_double(stmt, 5, s.obtained);
        sqlite3_bind_double(stmt, 6, s.percentage);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE)
        {
            cerr << "Insert failed: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }
        return true;
    }

    void displayAll()
    {
        const char *sql =
            "SELECT roll, name, class, total, obtained, percentage "
            "FROM students;";

        sqlite3_stmt *stmt;

        int rc = sqlite3_prepare_v2(database.getConnection(), sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(database.getConnection()) << endl;
            return;
        }

        int count = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            count++;
            cout << "\n\n Student " << count;
            cout << "\n Roll No. : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            cout << "\n Name : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            cout << "\n Class : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            cout << "\n Total Marks : " << sqlite3_column_double(stmt, 3);
            cout << "\n Obtained Marks : " << sqlite3_column_double(stmt, 4);
            cout << "\n Percentage % : " << sqlite3_column_double(stmt, 5);
        }

        if (count == 0)
        {
            cout << "\n\n Database is Empty....";
        }

        sqlite3_finalize(stmt);
    }

    bool search(const string &roll, Student &s) // takes roll number and a student class than updates that struct with info
    {

        const char *sql =
            "SELECT roll, name, class, total, obtained, percentage "
            "FROM students WHERE roll = ?;";

        sqlite3_stmt *stmt;

        int rc = sqlite3_prepare_v2(database.getConnection(), sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, roll.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            s.roll = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
            s.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            s.clas = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            s.total = static_cast<float>(sqlite3_column_double(stmt, 3));
            s.obtained = static_cast<float>(sqlite3_column_double(stmt, 4));
            s.percentage = static_cast<float>(sqlite3_column_double(stmt, 5));
            sqlite3_finalize(stmt);
            return true;
        }
        sqlite3_finalize(stmt);
        return false;
    }

    bool update(const string &oldRoll, const Student &newStudent) // tempoary student class
    {

        const char *sql =
            "UPDATE students SET roll = ?, name = ?, class = ?, total = ?, obtained = ?, percentage = ? "
            "WHERE roll = ?;";

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(database.getConnection(), sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare update statement: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, newStudent.roll.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, newStudent.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, newStudent.clas.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 4, newStudent.total);
        sqlite3_bind_double(stmt, 5, newStudent.obtained);
        sqlite3_bind_double(stmt, 6, newStudent.percentage);
        sqlite3_bind_text(stmt, 7, oldRoll.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE)
        {
            cerr << "Update failed: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }
        return true;
    }

    bool remove(const string &roll) // Takes a struct
    {
        const char *sql = "DELETE FROM students WHERE roll = ?;";
        sqlite3_stmt *stmt;

        int rc = sqlite3_prepare_v2(database.getConnection(), sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            cerr << "Failed to prepare statement: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, roll.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        int changes = sqlite3_changes(database.getConnection());
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE)
        {
            cerr << "Delete failed: " << sqlite3_errmsg(database.getConnection()) << endl;
            return false;
        }
        return changes > 0;
    }

    Database &getDatabase() { return database; } // getter for database class
};

// Function declarations
void runBackup(Database &db);

// UI and Control Panel
//-------------------
class UI
{
private:
    StudentService &service;

    void clearScreen()
    {
        system("cls");
    }

    void pause()
    {
        cout << "\nPress any key to continue...";
        getch();
    }

    void insertRecord()
    {
        char choice;
        do
        {
            clearScreen();
            cout << "\n\n\t\t\tInsert Record\n";
            Student s;

            cout << " Roll No. : ";
            cin >> s.roll;
            cout << " Name : ";
            cin >> s.name;
            cout << " Class : ";
            cin >> s.clas;
            cout << " Total Marks : ";
            cin >> s.total;
            cout << " Obtained Marks : ";
            cin >> s.obtained;
            s.percentage = (s.total != 0) ? (s.obtained / s.total) * 100.0f : 0;

            if (service.insert(s))
            {
                cout << "\nRecord inserted successfully.";
            }

            cout << "\n\n Do you want to add another record (y/n)? ";
            cin >> choice;
        } while (choice == 'y' || choice == 'Y');
    }

    void displayRecords()
    {
        clearScreen();
        cout << "\n\n\t\t\tDisplay Records\n";
        service.displayAll();
        pause();
    }

    void searchRecord()
    {
        clearScreen();
        cout << "\n\n\t\t\tSearch Record\n";
        string roll;
        cout << " Roll No. for search: ";
        cin >> roll;

        Student s;
        if (service.search(roll, s))
        {
            cout << "\nRoll No. : " << s.roll;
            cout << "\nName : " << s.name;
            cout << "\nClass : " << s.clas;
            cout << "\nTotal Marks : " << s.total;
            cout << "\nObtained Marks : " << s.obtained;
            cout << "\nPercentage : " << s.percentage;
        }
        else
        {
            cout << "\nRecord not found.";
        }
        pause();
    }

    void updateRecord()
    {
        clearScreen();
        cout << "\n\n\t\t\tUpdate Record\n";
        string oldRoll;
        cout << "Roll No. to update: ";
        cin >> oldRoll;

        Student existingStudent;
        if (!service.search(oldRoll, existingStudent))
        {
            cout << "\nRecord not found.";
            pause();
            return;
        }

        Student newStudent;

        cout << "\nEnter new details:\n";
        cout << "New Roll No. : ";
        cin >> newStudent.roll;
        cout << "Name : ";
        cin >> newStudent.name;
        cout << "Class : ";
        cin >> newStudent.clas;
        cout << "Total Marks : ";
        cin >> newStudent.total;
        cout << "Obtained Marks : ";
        cin >> newStudent.obtained;
        newStudent.percentage = (newStudent.total != 0) ? (newStudent.obtained / newStudent.total) * 100.0f : 0;

        if (service.update(oldRoll, newStudent))
        {
            cout << "\nRecord updated successfully.";
        }
        pause();
    }

    void deleteRecord()
    {
        clearScreen();
        cout << "\n\n\t\t\tDelete Record\n";
        string roll;
        cout << "Roll No. to delete: ";
        cin >> roll;

        if (service.remove(roll))
        {
            cout << "\nRecord deleted successfully.";
        }
        else
        {
            cout << "\nRecord not found.";
        }
        pause();
    }

public:
    explicit UI(StudentService &s) : service(s) {}

    void controlPanel()
    {
        int choice;
        while (true)
        {
            clearScreen();
            cout << "\n\n\t\t\tControl Panel";
            cout << "\n1. Insert Record";
            cout << "\n2. Display Records";
            cout << "\n3. Search Record";
            cout << "\n4. Update Record";
            cout << "\n5. Delete Record";
            cout << "\n6. Backup Database";
            cout << "\n7. Exit";

            cout << "\n\nEnter your choice: ";
            cin >> choice;

            switch (choice)
            {
            case 1:
                insertRecord();
                break;
            case 2:
                displayRecords();
                break;
            case 3:
                searchRecord();
                break;
            case 4:
                updateRecord();
                break;
            case 5:
                deleteRecord();
                break;
            case 6:
            {
                // Start backup in a separate thread
                std::thread backupThread(runBackup, std::ref(service.getDatabase()));
                backupThread.detach(); // Let the backup run independently
                cout << "Backup started in the background.\n";
                pause();
                break;
            }
            case 7:
                return;

            default:
                cout << "\nInvalid choice. Please try again.";
                pause();
            }
        }
    }
};

// Backup function
void runBackup(Database &db)
{
    // For example, create a timestamped backup file name
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "backup_%Y%m%d_%H%M%S.db", &tm);

    std::string backupDir = "backup/";
    std::filesystem::create_directories(backupDir); // Create folder if it doesn't exist

    std::string fullBackupPath = backupDir + std::string(buffer);

    cout << "\nStarting backup to file: " << buffer << endl;

    if (db.backup(fullBackupPath))
    {
        cout << "Backup completed successfully.\n";
    }
    else
    {
        cout << "Backup failed.\n";
    }
}

void startRestApiServer(StudentService &service)
{
    httplib::Server svr;

    // GET all students
    svr.Get("/students", [&](const httplib::Request &, httplib::Response &res) {
        std::ostringstream oss;
        oss << "[";
        bool first = true;

        const char *sql = "SELECT roll, name, class, total, obtained, percentage FROM students;";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(service.getDatabase().getConnection(), sql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                if (!first) oss << ",";
                first = false;
                oss << "{"
                    << "\"roll\":\"" << sqlite3_column_text(stmt, 0) << "\","
                    << "\"name\":\"" << sqlite3_column_text(stmt, 1) << "\","
                    << "\"class\":\"" << sqlite3_column_text(stmt, 2) << "\","
                    << "\"total\":" << sqlite3_column_double(stmt, 3) << ","
                    << "\"obtained\":" << sqlite3_column_double(stmt, 4) << ","
                    << "\"percentage\":" << sqlite3_column_double(stmt, 5)
                    << "}";
            }
            sqlite3_finalize(stmt);
        }

        oss << "]";
        res.set_content(oss.str(), "application/json");
    });

    // GET single student by roll
    svr.Get(R"(/students/(\w+))", [&](const httplib::Request &req, httplib::Response &res) {
        Student s;
        std::string roll = req.matches[1];
        if (service.search(roll, s))
        {
            std::ostringstream oss;
            oss << "{"
                << "\"roll\":\"" << s.roll << "\","
                << "\"name\":\"" << s.name << "\","
                << "\"class\":\"" << s.clas << "\","
                << "\"total\":" << s.total << ","
                << "\"obtained\":" << s.obtained << ","
                << "\"percentage\":" << s.percentage
                << "}";
            res.set_content(oss.str(), "application/json");
        }
        else
        {
            res.status = 404;
            res.set_content("{\"error\":\"Student not found\"}", "application/json");
        }
    });

    // POST new student (expects JSON)
    svr.Post("/students", [&](const httplib::Request &req, httplib::Response &res) {
        try
        {
            auto body = req.body;
            auto j = nlohmann::json::parse(body);
            Student s(j["roll"], j["name"], j["class"], j["total"], j["obtained"]);

            if (service.insert(s))
            {
                res.status = 201;
                res.set_content("{\"status\":\"created\"}", "application/json");
            }
            else
            {
                res.status = 400;
                res.set_content("{\"error\":\"Insert failed\"}", "application/json");
            }
        }
        catch (...)
        {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid input\"}", "application/json");
        }
    });

    // DELETE student
    svr.Delete(R"(/students/(\w+))", [&](const httplib::Request &req, httplib::Response &res) {
        std::string roll = req.matches[1];
        if (service.remove(roll))
        {
            res.set_content("{\"status\":\"deleted\"}", "application/json");
        }
        else
        {
            res.status = 404;
            res.set_content("{\"error\":\"Not found\"}", "application/json");
        }
    });

    std::cout << "REST API Server running at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}

// Main Function
//-------------------
int main()
{
    Database db;
    if (!db.open("students.db"))
    {
        return 1;
    }
    db.createTable();

    StudentService service(db);

    // Launch REST server on separate thread
    std::thread restApiThread(startRestApiServer, std::ref(service));
    restApiThread.detach(); // run independently

    UI ui(service);
    ui.controlPanel();

    db.close();
    return 0;
}
