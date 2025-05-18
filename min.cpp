#include <iostream>
#include <conio.h>
using namespace std;
int i = 0;
struct student
{
    string roll, name, clas;
    float tot, obt, per;
} s[10];

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

void insert(student s[])
{
    system("cls"); // clear terminal
    cout << "\n\n\t\t\tInsert Record";
    cout << "\n\n Roll No. : ";
    cin >> s[i].roll;
    cout << "\n\t\tName : ";
    cin >> s[i].name;
    cout << "\n Class : ";
    cin >> s[i].clas;
    cout << "\n\t\t\tTotal Marks : ";
    cin >> s[i].tot;
    cout << "\n Obtained Marks : ";
    cin >> s[i].obt;
    s[i].per = (s[i].obt / s[i].tot) * 100;
}

void display(student s[])
{
    int c = 1;
    system("cls"); // clear terminal
    cout << "\n\n\t\t\t\tDisplay Record";
    if (i > 0)
    {

        for (int a = 0; a < i; a++)
        {
            cout << "\n\n\n Student " << c;
            cout << "\n\n Roll No. : " << s[a].roll;
            cout << "\n\t\t Name : " << s[a].name;
            cout << "\n Class : " << s[a].clas;
            cout << "\n\t\t\t Total Marks : " << s[a].tot;
            cout << "\n Obtained Marks : " << s[a].obt;
            cout << "\n\t\t\t Percentage % : " << s[a].per;
            c++;
        }
    }
    else
    {
        cout << "\n\n Database is Empty....";
    }
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
                    s[i].per = (s[i].obt / s[i].tot) * 100;
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
            // Code to delete record
            break;
        case 6:
            exit(0);
        default:
            cout << "\n\n Invalid Value...Please Try Again...";
        }

        getch();
    }
}