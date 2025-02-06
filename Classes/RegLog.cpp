#include "RegLog.h"

RegLog::RegLog() {
    logged = false;
}

RegLog::~RegLog() {
}

bool RegLog::login(string name, string pass) {
    if (logged) {
        clrMSG = "[Class - ERROR] You are already logged in!";
        cmd.CheckOutput(clrMSG, true);
        return true;
    }
    if (checkJSON(name, pass) == true) {
        logged = true;
        clrMSG = "[Class] Successfully logged in.";
        cmd.CheckOutput(clrMSG, false);
        return true;
    }
    else {
        clrMSG = "[Class - ERROR] Invalid username or password!";
        cmd.CheckOutput(clrMSG, true);
        return false;
    }
}

bool RegLog::regUser(string name, string pass) {
    JSONMethods Jason("/home/alex/RC/TrainApp/DataBases/Users.JSON");
    nlohmann::json userData = Jason.LoadJSON();
    existingUser = false;
    bool userCreated = false;

    if (logged) {
        clrMSG = "[Class - ERROR] You are already logged in!";
        cmd.CheckOutput(clrMSG, true);
        return false;
    }

    if (checkUSER(name, userData) == true) {
        existingUser = true;
        clrMSG = "[Class - ERROR] Register failed, username already in use!";
        cmd.CheckOutput(clrMSG, true);
    }

    if (existingUser == false) {
        nlohmann::json newUser ={
            {"name", name},
            {"pass", pass}
        };
        userData["users"].push_back(newUser);
        cout << "[Class] New user added: " << newUser.dump(4) << endl;

        Jason.SaveJSON(userData);
        userCreated = true;
    }

    return userCreated;
}

void RegLog::logout() {
    logged = false;
    clrMSG = "[Class] Successfully logged out!";
    cmd.CheckOutput(clrMSG, false);
}

bool RegLog::logCheck() {
    return logged;
}

bool RegLog::checkJSON(string name, string pass) {
    JSONMethods Jason("/home/alex/RC/TrainApp/DataBases/Users.JSON");
    nlohmann::json userData = Jason.LoadJSON();

    if (!userData.contains("users") || !userData["users"].is_array()) {
        clrMSG = "[Class - ERROR] JSON file doesn't have a valid 'users' array!";
        cmd.CheckOutput(clrMSG, true);
        return false;
    }

    for (auto user : userData["users"]) {
        if (user.contains("name") && user["name"] == name) {
            if (user.contains("pass") && user["pass"] == pass) {
                return true;
            }
        }
    }

    clrMSG = "[Class - ERROR] Register failed!\nPossible reasons:\nExisting username;\nCorrupted JSON";
    cmd.CheckOutput(clrMSG, true);
    return false;
}

bool RegLog::checkUSER(string name, nlohmann::json& userData) {

    if (!userData.contains("users") || !userData["users"].is_array()) {
        clrMSG = "[Class - ERROR] JSON file doesn't have a valid 'users' array!";
        cmd.CheckOutput(clrMSG, true);
        return false;
    }

    for (auto user : userData["users"]) {
        if (user.contains("name") && user["name"] == name) {
            return true;
        }
    }
    return false;
}