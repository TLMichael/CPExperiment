#include "LA.h"
using namespace std;

bool isBlank(char ch)
{
    switch (ch)
    {
        case ' ':
            column++;
            break;
        case '\t':
            column += 4;
            break;
        case '\r':
        case '\n':
            line++;
            column = 1;
            break;
        default:
            return false;
    }
    return true;
}

bool isDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool isLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

int Reserve(string strToken)
{
    int i;
    for(i = 1; i <= 13; i++)
    {
        if(strToken == key[i])
            return i;
    }
    return 0;
}

void Concat(string &str, char ch)
{
    str.push_back(ch);
}

void Retract(fstream &source)
{
    if(!source.eof())
        source.seekg(-1, ios::cur);
}


int LA()
{
    string infileName;
    cout << "请输入程序文件名：";
    cin >> infileName;

    fstream source;
    fstream output;
    source.open(infileName, ios::in);
    output.open("results", ios::out | ios::trunc);

    line = 1;
    column = 1;

    if(!source.is_open())
    {
        cout << "无法打开源文件！" << endl;
        exit(1);
    }
    if(!output.is_open())
    {
        cout << "无法打开输出文件！" << endl;
        exit(0);
    }

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    output << "生成时间：" << asctime(timeinfo);
    output << "源文件：" << infileName << endl << endl;

    string strToken;
    char ch;
    while(!source.eof())
    {
        source.get(ch);
        if(isBlank(ch))
        {
            strToken = "";
        }
        else if(isLetter(ch))
        {
            while(isLetter(ch) || isDigit(ch))
            {
                Concat(strToken, ch);
                column++;
                source.get(ch);
            }

            Retract(source);

            if(Reserve(strToken))
            {
                output << strToken << ", RESERVED" << endl;
            }
            else
            {
                output << strToken << ", ID" << endl;
            }

            strToken = "";
        }
        else if(isDigit(ch))
        {
            while (isDigit(ch))
            {
                Concat(strToken, ch);
                column++;
                source.get(ch);
            }

            if(isLetter(ch))
            {
                cout << "[Program ERROR][" << line << ", " << column << "] " << "Invalid ID: ";
                output << "[Program ERROR][" << line << ", " << column << "] " << "Invalid ID: ";
                while(isLetter(ch) || isDigit(ch))
                {
                    Concat(strToken, ch);
                    column++;
                    source.get(ch);
                }
                cout << strToken << endl;
                output << strToken << endl;
            }
            else
            {
                output << strToken << ", INT" << endl;
            }
            Retract(source);
            strToken = "";
        }
        else if(ch == '=')
        {
            column++;
            output << ch << ", COP" << endl;
        }
        else if(ch == '<')
        {
            column++;
            source.get(ch);
            if(ch == '>')
            {
                column++;
                output << "<>, COP" << endl;
            }
            else if(ch == '=')
            {
                column++;
                output << "<=, COP" << endl;
            }
            else
            {
                output << "<, COP" << endl;
                Retract(source);
            }
        }
        else if(ch == '>')
        {
            column++;
            source.get(ch);
            if(ch == '=')
            {
                column++;
                output << ">=, COP" << endl;
            }
            else
            {
                output << ">, COP" << endl;
                Retract(source);
            }
        }
        else if(ch == ':')
        {
            column++;
            source.get(ch);
            if(ch == '=')
            {
                column++;
                output << ":=, AOP" << endl;
            }
            else
            {
                cout << "[Program ERROR]" << " [" << line << "," << column <<"] " << "Missing '=' near the ':' ;" << endl;
                output << "[Program ERROR]" << " [" << line << "," << column <<"] " << "Missing '=' near the ':' ;" << endl;
                Retract(source);
            }
        }
        else if(ch == '+')
        {
            column++;
            output << ch << ", OOP" << endl;
        }
        else if(ch == '-')
        {
            column++;
            output << ch << ", OOP" << endl;
        }
        else if(ch == '*')
        {
            column++;
            output << ch << ", OOP" << endl;
        }
        else if(ch == '/')
        {
            column++;
            output << ch << ", OOP" << endl;
        }
        else if(ch == ';')
        {
            column++;
            output << ch << ", EOP" << endl;
        }
        else if(ch == '(')
        {
            column++;
            output << ch << ", SOP" << endl;
        }
        else if(ch == ')')
        {
            column++;
            output << ch << ", SOP" << endl;
        }
        else if(ch == ',')
        {
            column++;
            output << ch << ", SOP" << endl;
        }
        else
        {
            column++;
            output << ch << ", UNKNOWN" << endl;
        }

    }

    source.close();
    output.close();
    cout << endl;
    cout << "词法分析成功完成，结果已存入results文件中。" << endl;

    return 0;
}