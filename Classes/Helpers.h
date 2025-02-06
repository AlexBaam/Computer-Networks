#pragma once
#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

#define RESET "\033[0m" // Reset text color
#define RED "\033[31m" // Red text
#define GREEN "\033[32m" // Green text
#define PURPLE "\033[95m" // Purple text
#define ORANGE "\033[38;5;208m" // Orange text

class Helpers {

public:
    Helpers();
    ~Helpers();
    string Menu(bool loggedIn);
    void Open();
    void CheckOutput(string message, bool IsError);
    void PurpleText(string message);
    void OrangeText(string message);
};
