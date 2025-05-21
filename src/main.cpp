#include <iostream>
#include <conio.h>
#include <sqlite3.h>

using namespace std;
sqlite3 *db;

void openDatabase()
{
    if (sqlite3_open("students.db", &db))
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }
}

void closeDatabase()
{
    sqlite3_close(db);
}

void createTable()
{
    const char *sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "roll TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "class TEXT NOT NULL, "
        "total REAL NOT NULL, " // REAL is floating point numbers
        "obtained REAL NOT NULL, "
        "percentage REAL NOT NULL);";

    char *errMsg = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg); // executes command.

    if (rc != SQLITE_OK)
    { // Not Successful result
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }
}

int i = 0;
struct student
{
    string roll, name, clas;
    float tot, obt, per;
};

void control_panel()
{
    cout << "\n\n\n\t\t\tControl Panel";
    cout << "\n 1. Insert Record";
    cout << "\n 2. Display Record";
    cout << "\n 3. Search Record";
    cout << "\n 4. Update Record";
    cout << "\n 5. Delete Record";
    cout << "\n 6. Exit";
    cout << "\n\n Enter Your Choice : ";
}

void insert(student &s)
{
    system("cls"); // clear terminal
    cout << "\n\n\t\t\tInsert Record";
    cout << "\n\n Roll No. : ";
    cin >> s.roll;
    cout << "\n\t\tName : ";
    cin >> s.name;
    cout << "\n Class : ";
    cin >> s.clas;
    cout << "\n\t\t\tTotal Marks : ";
    cin >> s.tot;
    cout << "\n Obtained Marks : ";
    cin >> s.obt;
    s.per = (s.obt / s.tot) * 100;

    // Prepare SQL Insert statement
    const char *sql =
        "INSERT INTO students "
        "(roll, name, class, total, obtained, percentage) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt; // sql c api pointer to the object

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); // compiles sql into prepared statement
    if (rc != SQLITE_OK)                                   // if not sucessful result
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind values to statement
    sqlite3_bind_text(stmt, 1, s.roll.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, s.clas.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, s.tot);
    sqlite3_bind_double(stmt, 5, s.obt);
    sqlite3_bind_double(stmt, 6, s.per);

    rc = sqlite3_step(stmt); // Execute the statement
    if (rc != SQLITE_DONE)
    {
        cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "\nRecord inserted successfully.";
    }

    sqlite3_finalize(stmt); // free recourses
}

void display()
{
    system("cls"); // clear terminal
    cout << "\n\n\t\t\t\tDisplay Records\n";

    const char *sql =
        "SELECT roll, name, class, total, obtained, percentage "
        "FROM students;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    int recordCount = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        recordCount++;
        cout << "\n\n Student " << recordCount;
        cout << "\n\n Roll No. : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        cout << "\n\t\t Name : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        cout << "\n Class : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        cout << "\n\t\t\t Total Marks : " << sqlite3_column_double(stmt, 3);
        cout << "\n Obtained Marks : " << sqlite3_column_double(stmt, 4);
        cout << "\n\t\t\t Percentage % : " << sqlite3_column_double(stmt, 5);
    }

    if (recordCount == 0)
    {
        cout << "\n\n Database is Empty....";
    }

    sqlite3_finalize(stmt);
}

void search()
{
    int count = 0;
    string roll;
    system("cls");
    cout << "\n\n\t\t\tSearch Record";

    cout << "\n\n Roll No. For Search : ";
    cin >> roll;

    const char *sql =
        "SELECT roll, name, class, total, obtained, percentage "
        "FROM students "
        "WHERE roll = ?;";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind roll number parameter
    sqlite3_bind_text(stmt, 1, roll.c_str(), -1, SQLITE_TRANSIENT);

    // Step through results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        cout << "\n\n Roll No. : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        cout << "\n\t\t Name : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        cout << "\n Class : " << reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        cout << "\n\t\t\t Total Marks : " << sqlite3_column_double(stmt, 3);
        cout << "\n Obtained Marks : " << sqlite3_column_double(stmt, 4);
        cout << "\n\t\t\t Percentage % : " << sqlite3_column_double(stmt, 5);
        count++;
    }

    if (count == 0)
    {
        cout << "\n\n Record Not Found...";
    }

    sqlite3_finalize(stmt);
}

void update()
{
    system("cls");
    string roll;
    int count = 0;

    cout << "\n\n\t\t\t\tUpdate Record";
    cout << "\n\n Roll No. For Update : ";
    cin >> roll;

    // First check if the record exists
    const char *check_sql =
        "SELECT COUNT(*) "
        "FROM students "
        "WHERE roll = ?;";

    sqlite3_stmt *check_stmt;

    int rc = sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(check_stmt, 1, roll.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(check_stmt);
    int exists = 0;
    if (rc == SQLITE_ROW)
    {
        exists = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);

    if (!exists)
    {
        cout << "\n\n Record Not Found....";
        return;
    }

    // Get new details
    string new_roll, name, clas;
    float tot, obt, per;

    cout << "\n\n New Roll No. : ";
    cin >> new_roll;
    cout << "\n\t\tName : ";
    cin >> name;
    cout << "\n Class : ";
    cin >> clas;
    cout << "\n\t\tTotal Marks : ";
    cin >> tot;
    cout << "\n Obtained Marks : ";
    cin >> obt;
    per = (obt / tot) * 100;

    // Prepare update SQL
    const char *update_sql =
        "UPDATE students "
        "SET roll = ?, name = ?, class = ?, total = ?, obtained = ?, percentage = ? "
        "WHERE roll = ?;";
    sqlite3_stmt *update_stmt;

    rc = sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind parameters
    sqlite3_bind_text(update_stmt, 1, new_roll.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(update_stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(update_stmt, 3, clas.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(update_stmt, 4, tot);
    sqlite3_bind_double(update_stmt, 5, obt);
    sqlite3_bind_double(update_stmt, 6, per);
    sqlite3_bind_text(update_stmt, 7, roll.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(update_stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "Update failed: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "\n\n Record updated successfully.";
    }

    sqlite3_finalize(update_stmt);
}

void del()
{
    system("cls");
    string roll;
    int count = 0;

    cout << "\n\n\t\t\t\tDelete Record";
    cout << "\n\n Roll No. For Delete : ";
    cin >> roll;

    // Prepare delete SQL
    const char *sql = "DELETE FROM students WHERE roll = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, roll.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "Delete failed: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        if (sqlite3_changes(db) > 0)
        {
            cout << "\n\n Record is Deleted....";
        }
        else
        {
            cout << "\n\n Record Not Found...";
        }
    }

    sqlite3_finalize(stmt);
}

int main()
{
    openDatabase();
    createTable();

    int choice;
    char x;
    while (true) // replaces goto and label
    {
        system("cls"); // clear console screen
        control_panel();
        cin >> choice; // inputs
        switch (choice)
        {
        case 1:
            do
            {
                student s;
                insert(s);
                cout << "\n\n Do You Want To Add Another Record (y,n) : ";
                cin >> x;
            } while (x == 'y');
            break;

        case 2:
            display();
            break;
        case 3:
            search();
            break;
        case 4:
            update();
            break;
        case 5:
            del();
            break;
        case 6:
            closeDatabase();
            exit(0);
        default:
            cout << "\n\n Invalid Value...Please Try Again...";
        }

        getch();
    }
}
