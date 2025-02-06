#pragma once
#include "Helpers.h"
#include "RegLog.h"
#include <iostream>
#include <string>
#include <json.hpp>
#include <fstream>
using namespace std;

class JSONMethods {
    string clrMSG;
    Helpers cmd;
    string JSONpath;
public:
    JSONMethods(string JSONpath);
    ~JSONMethods();
    nlohmann::json LoadJSON();
    void SaveJSON(nlohmann::json& userData);
};
