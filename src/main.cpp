#include <iostream>
#include <string>
#include <sqlite3.h>
#include <conio.h>

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

    Student(string r, string n, string c, float tot, float obt) : roll(r), name(n), clas(c), total(tot), obtained(obt)
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
    Database() : db(nullptr) {}

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
            sqlite3_close(db);
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
            "obtained REAL NOT NULL, "
            "percentage REAL NOT NULL);";

        char *errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK)
        {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    sqlite3 *getConnection() { return db; }
};

// Student Data Access
//-------------------
class StudentService
{
private:
    Database &database;

public:
    explicit StudentService(Database &db) : database(db) {}

    bool insert(const Student &s)
    {
        const char *sql = "INSERT INTO students (roll, name, class, total, obtained, percentage) VALUES (?, ?, ?, ?, ?, ?);";

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
        const char *sql = "SELECT roll, name, class, total, obtained, percentage FROM students;";
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

    bool search(const string &roll, Student &s)
    {
        const char *sql = "SELECT roll, name, class, total, obtained, percentage FROM students WHERE roll = ?;";
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

    bool update(const string &oldRoll, const Student &newStudent)
    {
        const char *sql =
            "UPDATE students SET roll = ?, name = ?, class = ?, total = ?, obtained = ?, percentage = ? WHERE roll = ?;";

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

    bool remove(const string &roll)
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
};

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
            cout << "\n6. Exit";
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
                return;
            default:
                cout << "\nInvalid choice. Please try again.";
                pause();
            }
        }
    }
};

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
    UI ui(service);

    ui.controlPanel();

    db.close();
    return 0;
}
