#include "Helpers.h"

Helpers::Helpers() {
}

Helpers::~Helpers() {
}

string Helpers::Menu(bool loggedIn) {

    string message;
    if (loggedIn == false) {
        cout << "Enter a command: "
        <<endl << "1. routes    -View trains."
        <<endl << "2. login     -Login user."
        <<endl << "3. register  -Register user."
        <<endl << "4. quit      -Exit application."
        <<endl; // Commands that can be used by a user that is not logged in.
        getline (cin, message); // Reading the input from the user;
    } else {
        cout << "Enter a command: "
        <<endl << "1. routes    -View trains."
        <<endl << "2. search    -Search trains."
        <<endl << "3. delay     -Signal delay of a train."
        <<endl << "4. early     -Signal early coming of a train."
        <<endl << "5. logout    -Logout user."
        <<endl << "6. quit      -Exit application."
        <<endl; // Commands that can be used by a user that is logged in.
        getline (cin, message); // Reading the input from the user;
    }

    return message;
}

void Helpers::Open() {
    cout << ORANGE << "==========================================" <<  endl
         << "            Welcome to TrainApp           " << endl
         << "=========================================="<< RESET << endl;
}

void Helpers::CheckOutput(string message, bool IsError) {
    if (IsError == true) {
        cout << RED << message << RESET << endl;
    }
    else {
        cout << GREEN << message << RESET << endl;
    }
}

void Helpers::PurpleText(string message) {
    cout << PURPLE << message << RESET << endl;
}

void Helpers::OrangeText(string message) {
    cout << ORANGE << message << RESET << endl;
}
