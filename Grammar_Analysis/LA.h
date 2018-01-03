#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

/**
 * 保留字表
 */
const string key[14] = {"", "program", "const", "var", "procedure", "begin",
                        "if", "then", "while", "call", "read", "write", "end", "do"};
int line;
int column;

/**
 * 判断字符是否为空白字符
 * @param ch
 * @return true为真
 */
bool isBlank(char ch);

/**
 * 判断字符是否为数字
 * @param ch
 * @return true为真
 */
bool isDigit(char ch);

/**
 * 判断字符是否为字母
 * @param ch
 * @return true为真
 */
bool isLetter(char ch);

/**
 * 判断strToken是关键字还是用户自定义的标识符
 * @param strToken
 * @return 若为0，则表示strToken
 */
int Reserve(string strToken);

/**
 * 将搜索指示器回调一个字符位置
 */
void Retract(fstream &source);

/**
 * 将ch的内容送入strToken
 * @param str
 * @param ch
 * @return
 */
void Concat(string &str, char ch);

int LA();