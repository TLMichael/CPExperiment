//
// Created by michael on 17-11-24.
//

#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

class Unit {
public:
    std::string value;
    std::string key;
    int line;
    int column;

    void print();
};

void Unit::print() {
    std::cout << "-----------------" << std::endl;
    std::cout << "Value: " << value << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "Position: [" << line << "," << column << "]" << std::endl;
    std::cout << "-----------------" << std::endl;
}

Unit unit, former_unit;
int errorStack[100] = {-1};
int *errorType = errorStack;
bool error = false;
fstream gaSource;
fstream gaOutput;

string line;
string::iterator itLine;

/**
 * Read file line by line.
 * @return Global variables: unit.value & unit.key.
 */
void ReadLine();

/**
 * Throw an error.
 * @param type Error type.
 */
void ThrowError(int type);

/**
 * <factor>¡ú<id>|<integer>|(<exp>)
 */
void Factor();

/**
 * <term> ¡ú <factor>{<mop><factor>}
 */
void Term();

/**
 * <exp> ¡ú [+|-]<term>{<aop><term>}
 */
void Exp();

/**
 * <lexp> ¡ú <exp> <lop> <exp>|odd <exp>
 */
void Lexp();

/**
 * <statement> ¡ú <id> := <exp>
               |if <lexp> then <statement>[else <statement>]
               |while <lexp> do <statement>
               |call <id>[£¨<exp>{,<exp>}£©]
               |<body>
               |read (<id>{£¬<id>})
               |write (<exp>{,<exp>})
 */
void Statement();

/**
 * <body> ¡ú begin <statement>{;<statement>}end
 */
void Body();

/**
 * <proc> ¡ú procedure <id>£¨<id>{,<id>}£©;<block>{;<proc>}
 */
void Proc();

/**
 * <vardecl> ¡ú var <id>{,<id>};
 * <id> ¡ú l{l|d}
 * l represent letter.
 */
void Vardecl();

/**
 * Const variables declaration.
 * <condecl> ¡ú const <const>{,<const>};
 * <const> ¡ú <id>:=<integer>
 * <id> ¡ú l{l|d}
 * l represent letter.
 */
void Condecl();

/**
 * <block> ¡ú [<condecl>][<vardecl>][<proc>]<body>
 */
void Block();

/**
 * <prog> ¡ú program <id>; <block>
 */
void Prog();

/**
 * Open IO files.
 */
void OpenFile();

/**
 * Close IO files.
 */
void CloseFile();

/**
 * Print the error stack out.
 */
void PrintErrorStack();

/**
 * Grammar analysis main program
 * @return
 */
int GA();
