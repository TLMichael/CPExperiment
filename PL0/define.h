#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLENUM 100
#define CODENUM 200
//**************************************************************************
enum symbol {							//字符类型标签
    ident, programS, varS, procedureS, beginS, endS, ifS, thenS, elseS,
    whileS, doS, callS, readS, writeS, oddS, constS, number, equal, plus, minus,
    multiply, involution, divide, more, notless, less, notmore, notequal,
    assignment, comma, semicolon, leftbrackets, rightbrackets, undefine
};
//*************************************************
struct word {							//单词表
    enum symbol SYM;//单词类型
    char ID[15];	//标识符内容
    int NUM;		//常数内容
    int lineN;		//行号
}tab;
//*************************************************
typedef struct tablelink {				//单词链表结构体
    enum symbol SYM;//单词类型
    char ID[15];	//标识符内容
    int NUM;		//常数内容
    int lineN;		//行号
    struct tablelink *next;//下一单元地址
}tabnode, *tabl;
//*************************************************
struct  table {							//符号表
    enum symbol type;//符号类型
    int level;		//所在层次
    char levelname[15];//层名
    char ID[15];	//名称
    int num;		//常数值
    int adr; 		//相对偏移量
}ta[TABLENUM];
//*************************************************
void freelink(tabl &tb) {						//销毁链表
    tabl q;
    while (tb != NULL) {
        q = tb;
        tb = tb->next;
        free(q);
    }
}
//**************************************************************************
enum codesym {							//代码操作类型
    LIT, OPR, LOD, STO, CAL, RET, INT, JMP, JPC, RED, WRT
};
//*************************************************
struct code {							//代码表
    enum codesym func;	//操作类型
    int L;				//层差
    int a;				//相对地址
}cod[CODENUM];
//**************************************************************************
#define STACK_INIT_SIZE 1000	//默认最大长度为100
#define STACKINCREMENT 10	//空间增量为10
#define SElemType int
struct SqStack								//定义栈结构
{
    SElemType *base;//栈底指针
    SElemType *top;	//栈顶指针
    int Stacksize;	//当前分配的存储容量
}data;
//*************************************************
void InitStack(SqStack &S)					//构造空的栈
{
    S.base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));//申请空间
    S.top = S.base;
    S.Stacksize = STACK_INIT_SIZE;
}
//*************************************************
void DestroyStack(SqStack &S)				//销毁栈
{
    S.top = S.base;
    S.Stacksize = 0;
    free(S.base);
    S.base = NULL;
    S.top = NULL;
}
//*************************************************
void StackPush(SqStack &S, SElemType e)		//插入新的栈顶元素
{
    SElemType *newbase;
    if ((S.top - S.base) == STACK_INIT_SIZE)	//若空间已满则重新申请空间
    {
        newbase = (SElemType *)realloc(S.base, (S.Stacksize + STACKINCREMENT) * sizeof(SElemType));
        if (!newbase)
            exit(0);
        S.base = newbase;
        S.top = S.base + STACK_INIT_SIZE;
        S.Stacksize += STACKINCREMENT;
    }
    *(S.top) = e;
    S.top++;
}
//*************************************************
void GetESem(SqStack S, SElemType &e)		//返回栈顶元素
{
    if (S.top != S.base)
        e = *(S.top - 1);
    else
    {
        printf("栈为空！\n");
        exit(0);
    }
}

//解决linux下itoa函数不存在的问题
void itoa(int value, char *string, int radix)
{
    if(radix != 10)
        exit(0);
    sprintf(string, "%d", value);
}

//**************************************************************************
void run(int cx, struct code *cod) {					//执行代码
    InitStack(data);
    int i, e, B = 0, T = 0, P;
    int a, b, c, Bs[100] = { 0 }, j = 0;
    int count, Bsum = 0;
    int jj = 0;
    for (i = 0; i<cx; i = P) {
        P = i + 1;
        switch (cod[i].func) {
            case LIT:
                StackPush(data, cod[i].a);
                T++;
                break;
            case OPR:
                switch (cod[i + 1].func) {	//先执行下一条代码，获取操作数
                    case LIT:
                        StackPush(data, cod[i + 1].a);
                        T++;
                        break;
                    case LOD:
                        a = *(data.base + Bs[cod[i].L ? 0 : j] + 2 + cod[i + 1].a);
                        StackPush(data, a);
                        T++;
                        break;
                }
                GetESem(data, b);
                data.top--;
                T--;
                GetESem(data, a);
                data.top--;
                T--;
                switch (cod[i].a) {		//判断操作类型
                    case 0:
                        break;
                    case 1:			//取负
                        StackPush(data, b);
                        T++;
                        StackPush(data, -a);
                        T++;
                        break;
                    case 2:			//相加
                        StackPush(data, b + a);
                        T++;
                        break;
                    case 3:			//相减
                        StackPush(data, a - b);
                        T++;
                        break;
                    case 4:			//相乘
                        StackPush(data, b*a);
                        T++;
                        break;
                    case 5:			//相除
                        StackPush(data, a / b);
                        T++;
                        break;
                    case 6:			//判断奇偶性
                        StackPush(data, b);
                        T++;
                        StackPush(data, a % 2);
                        T++;
                        break;
                    case 10:		//等于判定
                        c = b - a;
                        if (!c) {
                            c = 1;
                        }
                        else {
                            c = 0;
                        }
                        StackPush(data, c);
                        T++;
                        break;
                    case 11:		//不等于判定
                        StackPush(data, b - a);
                        T++;
                        break;
                    case 12:		//小于判定
                        c = b - a;
                        if (c>0) {
                            c = 1;
                        }
                        else {
                            c = 0;
                        }
                        StackPush(data, c);
                        T++;
                        break;
                    case 13:		//小于等于判定
                        c = b - a;
                        if (c >= 0) {
                            c = 1;
                        }
                        else {
                            c = 0;
                        }
                        StackPush(data, c);
                        T++;
                        break;
                    case 14:		//大于判定
                        c = b - a;
                        if (c<0) {
                            c = 1;
                        }
                        else {
                            c = 0;
                        }
                        StackPush(data, c);
                        T++;
                        break;
                    case 15:		//大于等于判定
                        c = b - a;
                        if (c <= 0) {
                            c = 1;
                        }
                        else {
                            c = 0;
                        }
                        StackPush(data, c);
                        T++;
                        break;
                }
                P = i + 2;	//跳过下一条指令
                break;
            case LOD:
                a = *(data.base + Bs[cod[i].L ? 0 : j] + 2 + cod[i].a);
                StackPush(data, a);
                T++;
                break;
            case STO:
                GetESem(data, e);
                data.top--;
                T--;
                *(data.base + Bs[cod[i].L ? 0 : j] + 2 + cod[i].a) = e;
                break;
            case CAL:
                P = cod[i].a;
                jj += cod[i].L;
                Bsum = 0;
                for (int k = 0; k <= j; k++)
                {
                    Bsum += *(data.base + Bs[k]);
                }
                count = T - Bsum;
                data.top -= count;
                T -= count;
                int temp[10];
                for (int k = 0; k < count; k++)
                {
                    temp[k] = *(data.top + k);
                }
                *(data.top) = Bs[j];
                *(data.top + 1) = B;
                B = T;
                j++;
                Bs[j] = B;

                i++;
                c = 3;

                while (c < 3 + count) {
                    *(data.top + c) = temp[c - 3];

                    c++;
                }
                *(data.top + 2) = i;
                break;
            case RET:
                data.top = data.base + B;
                T = B;
                P = *(data.top + 2);
                B = *(data.top + 1);
                j--;
                break;
            case INT:
                data.top += cod[i].a;

                *(data.base + T) = cod[i].a;
                T += cod[i].a;
                break;
            case JMP:
                P = cod[i].a;
                break;
            case JPC:
                GetESem(data, e);
                data.top--;
                T--;
                if (!e) {		//当不满足条件时跳转
                    P = cod[i].a;
                }
                break;
            case RED:
                printf("请输入数据: ");
                scanf("%d", data.base + B + 2 + cod[i].a);
                break;
            case WRT:
                GetESem(data, e);
                data.top--;
                T--;
                printf("%d\n", e);
                break;
        }
    }
    DestroyStack(data);
}
