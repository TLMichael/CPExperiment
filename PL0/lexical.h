#include "define.h"

#define FILE_PATH "test.txt"
#define LINELEG 100		//定义文件每行的最大读取字符数
#define CODENUM 100
char line[LINELEG];
int CH;	//行号变量
//*************************************************
void GetChar(char &ch) {					//获取下一个字符
    ch = line[CH];
    CH++;
}
//*************************************************
void GetBC(char &ch) {					//获取下一个非空字符
    ch = line[CH];
    CH++;
    while (ch == ' ' || ch == '\t') {
        ch = line[CH];
        CH++;
    }
}
//*************************************************
void Concat(char *strToken, char ch) {	//将字符加入临时字符串末尾
    char *str = strToken;
    while (*strToken != '\0') {
        strToken++;
    }
    *strToken = ch;
    strToken = str;
}
//*************************************************
bool IsLetter(char ch) {					//判断字符是否是字母
    if (ch >= 'a'&&ch <= 'z') {
        return true;
    }
    else if (ch >= 'A'&&ch <= 'Z') {
        return true;
    }
    else
        return false;
}
//*************************************************
bool IsDigit(char ch) {					//判断字符是否是数字
    if (ch >= '0'&&ch <= '9') {
        return true;
    }
    else
        return false;
}
//*************************************************
void Reserve(char *strToken, word &lex) {	//判断读出的临时字符串是否是保留字或标识符
    if (strcmp(strToken, "program") == 0)
        lex.SYM = programS;
    else if (strcmp(strToken, "var") == 0)
        lex.SYM = varS;
    else if (strcmp(strToken, "procedure") == 0)
        lex.SYM = procedureS;
    else if (strcmp(strToken, "begin") == 0)
        lex.SYM = beginS;
    else if (strcmp(strToken, "end") == 0)
        lex.SYM = endS;
    else if (strcmp(strToken, "if") == 0)
        lex.SYM = ifS;
    else if (strcmp(strToken, "then") == 0)
        lex.SYM = thenS;
    else if (strcmp(strToken, "else") == 0)
        lex.SYM = elseS;
    else if (strcmp(strToken, "while") == 0)
        lex.SYM = whileS;
    else if (strcmp(strToken, "do") == 0)
        lex.SYM = doS;
    else if (strcmp(strToken, "call") == 0)
        lex.SYM = callS;
    else if (strcmp(strToken, "read") == 0)
        lex.SYM = readS;
    else if (strcmp(strToken, "write") == 0)
        lex.SYM = writeS;
    else if (strcmp(strToken, "odd") == 0)
        lex.SYM = oddS;
    else if (strcmp(strToken, "const") == 0)
        lex.SYM = constS;
    else {
        lex.SYM = ident;
        strcpy(lex.ID, strToken);//用户自定义标识符
    }
}
//*************************************************
void Retract(char &ch) {					//返回读字符前的位置
    CH--;
    ch = NULL;
}
//*************************************************
void lexical() {
    char ch;
    int lineN = 0;
    char strToken[30] = { "" };	//临时字符串
    FILE *fp2 = NULL;
    FILE *fp1 = NULL;
    fp1 = fopen(FILE_PATH, "r");	//打开源程序文件
    if (fp1 == NULL) {
        printf("文件打开失败！\n");
        exit(0);
    }
    fp2 = fopen("lexical.txt", "wb");
    if (fp2 == NULL) {
        printf("文件打开失败！\n");
        exit(0);
    }
    while (!feof(fp1)) {			//识别源文件中各个单词、字符的类型
        fgets(line, LINELEG, fp1);	//从文件中读出一行存入数组
        lineN++;
        tab.lineN = lineN;
        CH = 0;
        GetBC(ch);
        while (ch != '\0'&&ch != '\n') {
            if (IsLetter(ch)) {		//若字符是字母则继续读取
                while (IsLetter(ch) || IsDigit(ch)) {
                    Concat(strToken, ch);
                    GetChar(ch);
                }
                Retract(ch);
                Reserve(strToken, tab);	//判断单词是保留字还是标识符
            }
            else if (IsDigit(ch)) {	//若字符是数字则继续读取
                while (IsDigit(ch)) {
                    Concat(strToken, ch);
                    GetChar(ch);
                }
                Retract(ch);
                tab.SYM = number;
                tab.NUM = atoi(strToken);	//将数值存入表中
            }
            else if (ch == '=')		//否则判断字符类型
                tab.SYM = equal;//等号
            else if (ch == '+')
                tab.SYM = plus;//加号
            else if (ch == '-')
                tab.SYM = minus;//减号
            else if (ch == '*') {
                GetChar(ch);
                if (ch == '*')
                    tab.SYM = involution;//乘方号
                else {
                    Retract(ch);
                    tab.SYM = multiply;//乘号
                }
            }
            else if (ch == '/') {
                tab.SYM = divide;//除号
            }
            else if (ch == '>') {
                GetChar(ch);
                if (ch == '=')
                    tab.SYM = notless;//大于等于号
                else {
                    Retract(ch);
                    tab.SYM = more;//大于号
                }
            }
            else if (ch == '<') {
                GetChar(ch);
                if (ch == '=')
                    tab.SYM = notmore;//小于等于号
                else if (ch == '>')
                    tab.SYM = notequal;//不等于号
                else {
                    Retract(ch);
                    tab.SYM = less;//小于号
                }
            }
            else if (ch == ':') {
                GetChar(ch);
                if (ch == '=') {
                    tab.SYM = assignment;//赋值号
                }
                else
                    Retract(ch);
            }
            else if (ch == ',')
                tab.SYM = comma;//逗号
            else if (ch == ';')
                tab.SYM = semicolon;//分号
            else if (ch == '(')
                tab.SYM = leftbrackets;//左括号
            else if (ch == ')')
                tab.SYM = rightbrackets;//右括号
            else {
                while (ch != '\0'&&ch != 10 && ch != 9) {
                    Concat(strToken, ch);
                    GetBC(ch);
                }
                Retract(ch);
                tab.SYM = undefine;//未定义字符
                printf("\n错误！第%d行存在未定义字符'%s'\n", lineN, strToken);
            }
            fwrite(&tab, sizeof(struct word), 1, fp2);
            memset(strToken, 0, sizeof(strToken));
            GetBC(ch);
        }
        memset(line, 0, LINELEG);
    }
    printf("\n词法分析结束\n");
    fclose(fp1);
    fclose(fp2);
}
//*************************************************
void savelex() {							//输出词法分析结果
    FILE *fp1 = NULL, *fp2 = NULL;
    char line[5];
    fp1 = fopen("lexical.txt", "rb");
    if (fp1 == NULL) {
        printf("文件打开失败！\n");
        exit;
    }
    fp2 = fopen("result.txt", "w");
    if (fp2 == NULL) {
        printf("文件打开失败！\n");
        exit;
    }
    fputs("内容", fp2);
    fputs("\t\t", fp2);
    fputs("类型", fp2);
    fputs("\t\t\t", fp2);
    fputs("行号", fp2);
    fputs("\t\t\n", fp2);
    while (!feof(fp1)) {
        fread(&tab, sizeof(struct word), 1, fp1);
        switch (tab.SYM) {
            case ident:
                fputs(tab.ID, fp2);
                fputs("\t\t", fp2);
                fputs("ident", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case programS:
                fputs("program", fp2);
                fputs("\t\t", fp2);
                fputs("programS", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case varS:
                fputs("var", fp2);
                fputs("\t\t", fp2);
                fputs("varS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case procedureS:
                fputs("procedure", fp2);
                fputs("\t", fp2);
                fputs("procedureS", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case beginS:
                fputs("begin", fp2);
                fputs("\t\t", fp2);
                fputs("beginS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case endS:
                fputs("end", fp2);
                fputs("\t\t", fp2);
                fputs("endS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case ifS:
                fputs("if", fp2);
                fputs("\t\t", fp2);
                fputs("ifS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case thenS:
                fputs("then", fp2);
                fputs("\t\t", fp2);
                fputs("thenS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case elseS:
                fputs("else", fp2);
                fputs("\t\t", fp2);
                fputs("elseS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case whileS:
                fputs("while", fp2);
                fputs("\t\t", fp2);
                fputs("whileS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case doS:
                fputs("do", fp2);
                fputs("\t\t", fp2);
                fputs("doS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case callS:
                fputs("call", fp2);
                fputs("\t\t", fp2);
                fputs("callS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case readS:
                fputs("read", fp2);
                fputs("\t\t", fp2);
                fputs("readS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case writeS:
                fputs("write", fp2);
                fputs("\t\t", fp2);
                fputs("writeS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case oddS:
                fputs("odd", fp2);
                fputs("\t\t", fp2);
                fputs("oddS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case constS:
                fputs("const", fp2);
                fputs("\t\t", fp2);
                fputs("constS", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case number:
                char str[10];
                itoa(tab.NUM, str, 10);
                fputs(str, fp2);
                fputs("\t\t", fp2);
                fputs("number", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case equal:
                fputs("=", fp2);
                fputs("\t\t", fp2);
                fputs("equal", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case plus:
                fputs("+", fp2);
                fputs("\t\t", fp2);
                fputs("plus", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case minus:
                fputs("-", fp2);
                fputs("\t\t", fp2);
                fputs("minus", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case multiply:
                fputs("*", fp2);
                fputs("\t\t", fp2);
                fputs("multiply", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case involution:
                fputs("**", fp2);
                fputs("\t\t", fp2);
                fputs("involution", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case divide:
                fputs("/", fp2);
                fputs("\t\t", fp2);
                fputs("divide", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case more:
                fputs(">", fp2);
                fputs("\t\t", fp2);
                fputs("more", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case notless:
                fputs(">=", fp2);
                fputs("\t\t", fp2);
                fputs("notless", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case less:
                fputs("<", fp2);
                fputs("\t\t", fp2);
                fputs("less", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case notmore:
                fputs("<=", fp2);
                fputs("\t\t", fp2);
                fputs("notmore", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case notequal:
                fputs("<>", fp2);
                fputs("\t\t", fp2);
                fputs("notequal", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case assignment:
                fputs(":=", fp2);
                fputs("\t\t", fp2);
                fputs("assignment", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case comma:
                fputs(",", fp2);
                fputs("\t\t", fp2);
                fputs("comma", fp2);
                fputs("\t\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case semicolon:
                fputs(";", fp2);
                fputs("\t\t", fp2);
                fputs("semicolon", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case leftbrackets:
                fputs("(", fp2);
                fputs("\t\t", fp2);
                fputs("leftbrackets", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case rightbrackets:
                fputs(")", fp2);
                fputs("\t\t", fp2);
                fputs("rightbrackets", fp2);
                fputs("\t\t", fp2);
                itoa(tab.lineN, line, 10);
                fputs(line, fp2);
                fputs("\t\t\n", fp2);
                break;
            case undefine:
                break;
        }
    }
    fclose(fp1);
    fclose(fp2);
}
