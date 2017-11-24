//
// Created by michael on 17-11-24.
//

#include "GA.h"
using namespace std;

Unit unit, former_unit;
int errorStack[100] = {-1};
int *errorType = errorStack;
bool error = false;
fstream gaSource;
fstream gaOutput;

void ReadLine()
{
    if(getline(gaSource, line)) {
        itLine = line.begin();
        while (*itLine == '#' || line.empty()) {
            getline(gaSource, line);
            itLine = line.begin();
        }

        former_unit = unit;

        istringstream iss(line);

        if (*itLine == '^') {
            char ch;
            iss >> ch;
            iss >> unit.value;
            iss >> unit.key;
            iss >> unit.line;
            iss >> unit.column;
            getline(gaSource, line);
        } else {
            iss >> ch;
            iss >> unit.value;
            iss >> unit.key;
            iss >> unit.line;
            iss >> unit.column;
        }
    }else{

    }
}

void ThrowError(int type);
void Exp();
void Factor();
void Term();
void Lexp();
void Body();
void Statement();
void Block();
void Proc();
void Vardec1();
void Condec1();
void Prog();
void OpenFile()
{
    gaSource.open("la_output", ios::in);
    gaOutput.open("ga_output", ios::out | ios::trunc);

    if(!gaSource.is_open())
    {
        cout << "Cannot open the gaSource file!\a" << endl;
        exit(1);
    }
    if(!gaOutput.is_open())
    {
        cout << "Cannot open the gaOutput file!\a" << endl;
    }else{
        // Header of the file (DataTime & File name & Lang set)

        time_t rawtime;
        struct tm * timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        gaOutput << "# Grammar Analysis Result" << endl;
        gaOutput << "# Generated Time: " << asctime(timeinfo);
        gaOutput << "# Language Set: PL/0" << endl;
        gaOutput << endl;
    }
}
void CloseFile();
void PrintErrorStack();

int GA()
{
    OpenFile();
    Proc();
    CloseFile();
    while(error)
    {
        error = false;
        errorType = errorStack;
        OpenFile();
        Prog();
        CloseFile();
    }

    return 0;
}