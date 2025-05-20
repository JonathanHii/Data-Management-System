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

void insert(student& s)
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
    const char* sql = "INSERT INTO students (roll, name, class, total, obtained, percentage) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
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

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cerr << "Insert failed: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        cout << "\nRecord inserted successfully.";
    }

    sqlite3_finalize(stmt);
}


void display()
{
    system("cls"); // clear terminal
    cout << "\n\n\t\t\t\tDisplay Records\n";

    const char* sql = "SELECT roll, name, class, total, obtained, percentage FROM students;";
    sqlite3_stmt* stmt;

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
        cout << "\n\n Roll No. : " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cout << "\n\t\t Name : " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        cout << "\n Class : " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
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

void search(student s[])
{
    int count = 0;
    string roll;
    system("cls");
    cout << "\n\n\t\t\tSearch Record";
    if (i > 0)
    {
        cout << "\n\n Roll No. For Search : ";
        cin >> roll;
        for (int a = 0; a < i; a++)
        {
            if (roll == s[a].roll)
            {
                cout << "\n\n Roll No. : " << s[a].roll;
                cout << "\n\t\t Name : " << s[a].name;
                cout << "\n Class : " << s[a].clas;
                cout << "\n\t\t\t Total Marks : " << s[a].tot;
                cout << "\n Obtained Marks : " << s[a].obt;
                cout << "\n\t\t\t Percentage % : " << s[a].per;
                count++;
            }
        }
        if (count == 0)
        {
            cout << "\n\n Record Not Found...";
        }
    }
    else
        cout << "\n\n Data Base is Empty...";
}

void update(student s[])
{
    system("cls");
    string roll;
    int count = 0;
    if (i > 0)
    {
        cout << "\n\n\t\t\t\tUpdate Record";
        if (i > 0)
        {
            cout << "\n\n Roll No. For Update : ";
            cin >> roll;
            for (int a = 0; a < i; a++)
            {
                if (roll == s[a].roll)
                {
                    cout << "\n\n New Roll No. : ";
                    cin >> s[a].roll;
                    cout << "\n\t\tName : ";
                    cin >> s[a].name;
                    cout << "\n Class : ";
                    cin >> s[a].clas;
                    cout << "\n\t\tTotal Marks : ";
                    cin >> s[a].tot;
                    cout << "\n Obtained Marks : ";
                    cin >> s[a].obt;
                    s[a].per = (s[a].obt / s[a].tot) * 100;
                    count++;
                }
            }
        }
        if (count == 0)
        {
            cout << "\n\n Record Not Found....";
        }
    }
    else
    {
        cout << "\n\n Database is Empty....";
    }
}

void del(student s[])
{
    system("cls");
    string roll, t_roll, t_name, t_class;
    float t_tot, t_obt, t_per;
    int count = 0;
    cout << "\n\n\t\t\t\tDelete Record";
    if (i > 0)
    {
        cout << "\n\n Roll No. For Delete : ";
        cin >> roll;
        for (int a = 0; a <= i; a++)
        {
            if (roll == s[a].roll)
            {
                for (int j = a; j < i; j++)
                {
                    t_roll = s[j + 1].roll;
                    t_name = s[j + 1].name;
                    t_class = s[j + 1].clas;
                    t_tot = s[j + 1].tot;
                    t_obt = s[j + 1].obt;
                    t_per = s[j + 1].per;
                    s[j].roll = t_roll;
                    s[j].name = t_name;
                    s[j].clas = t_class;
                    s[j].tot = t_tot;
                    s[j].obt = t_obt;
                    s[j].per = t_per;
                }
                cout << "\n\n Record is Deleted....";
                i--;
                count++;
            }
        }
        if (count == 0)
        {
            cout << "\n\n Record Not Found...";
        }
    }
    else
    {
        cout << "\n\n Database is empty";
    }
}

int main()
{
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
                insert(s);
                i++; // increase array index
                cout << "\n\n Do You Want To Add Another Record (y,n) : ";
                cin >> x;
            } while (x == 'y');
            break;

        case 2:
            display(s);
            break;
        case 3:
            search(s);
            break;
        case 4:
            update(s);
            break;
        case 5:
            del(s);
            break;
        case 6:
            exit(0);
        default:
            cout << "\n\n Invalid Value...Please Try Again...";
        }

        getch();
    }
}