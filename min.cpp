#include <iostream>
#include <conio.h>
using namespace std;

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

int main()
{   
    int choice;
    p:
    system("cls");
    control_panel();
    cin >> choice;
    switch (choice)
    {
        case 1:
            // Code to insert record
            break;
        case 2:
            // Code to display record
            break;
        case 3: 
            // Code to search record
            break;
        case 4:
            // Code to update record
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
    goto p;
}
