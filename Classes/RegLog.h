#pragma once
#include <json.hpp>
#include <string>
#include <iostream>
#include "Helpers.h"
#include "JSONMethods.h"
using namespace std;

class RegLog {
private:
    string clrMSG;
    Helpers cmd;
    bool logged;
    bool existingUser;
public:
    RegLog();
    ~RegLog();
    bool login(string name, string pass); // Logare (Setam logged = true)
    bool regUser(string name, string pass); // Inregistrare
    void logout(); // Delogare (Setam logged = false)
    bool logCheck(); // Verificam status logged;
    bool checkJSON(string name, string pass); //Verificam fisierul JSON daca combinatia NUME+PAROLA exista
    bool checkUSER(string name, nlohmann::json& userData); //CheckJSON dar ptr register
};
