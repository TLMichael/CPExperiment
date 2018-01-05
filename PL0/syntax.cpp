#include "lexical.h"

int err = 0;		//错误总数
int lev = 1;		//层数
char levname[5];//层名
int tx = 0;		//字符表内的字符数
int dx[10] = { 0 }; 		//相对偏移量
int cx = 0;		//代码表内的代码数

//****************************************************************
void getcode() {								//输出字符表和代码表
    int i;
    char number[5];
    FILE *fp;
    fp = fopen("table.txt", "w");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        exit;
    }
    fputs("\t名称\t类型\t      值/空间\t层\t偏移量\n", fp);
    for (i = 0; i<tx; i++) {
        itoa(i, number, 10);
        fputs(number, fp);
        fputs("\t", fp);
        fputs(ta[i].ID, fp);
        fputs("\t", fp);
        switch (ta[i].type) {
            case constS:
                fputs("const\t", fp);
                fputs("\t", fp);
                itoa(ta[i].num, number, 10);
                fputs(number, fp);
                break;
            case varS:
                fputs("var\t", fp);
                fputs("\t", fp);
                break;
            case procedureS:
                fputs("procedure", fp);
                fputs("\t", fp);
                itoa(ta[i].num, number, 10);
                fputs(number, fp);
                break;
        }
        fputs("\t", fp);
        itoa(ta[i].level, number, 10);
        fputs(number, fp);
        fputs("\t", fp);
        itoa(ta[i].adr, number, 10);
        fputs(number, fp);
        fputs("\n", fp);
    }
    fclose(fp);
    fp = fopen("code.txt", "w");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        exit;
    }
    fputs("\t操作码\t层差\t位移量\n", fp);
    for (i = 0; i<cx; i++) {
        itoa(i, number, 10);
        fputs(number, fp);
        fputs("\t", fp);
        switch (cod[i].func) {
            case LIT:
                fputs("LIT", fp);
                break;
            case OPR:
                fputs("OPR", fp);
                break;
            case LOD:
                fputs("LOD", fp);
                break;
            case STO:
                fputs("STO", fp);
                break;
            case CAL:
                fputs("CAL", fp);
                break;
            case RET:
                fputs("RET", fp);
                break;
            case INT:
                fputs("INT", fp);
                break;
            case JMP:
                fputs("JMP", fp);
                break;
            case JPC:
                fputs("JPC", fp);
                break;
            case RED:
                fputs("RED", fp);
                break;
            case WRT:
                fputs("WRT", fp);
                break;
        }
        fputs("\t", fp);
        itoa(cod[i].L, number, 10);
        fputs(number, fp);
        fputs("\t", fp);
        itoa(cod[i].a, number, 10);
        fputs(number, fp);
        fputs("\n", fp);
    }
    fclose(fp);
}
//****************************************************************
void gen(enum codesym func, int L, int a) {		//生成代码存入代码表
    if (cx >= CODENUM) {
        printf("程序过长，代码溢出!\n");
        exit;
    }
    cod[cx].func = func;
    cod[cx].L = L;
    cod[cx].a = a;
    cx++;
}
//****************************************************************
int position(char *ID) {							//在字符表中查找当前字符
    for (int i = tx - 1; i >= 0; i--) {
        if (!(strcmp(ta[i].ID, ID))) {
            if (!(strcmp(ta[i].levelname, levname)) || ta[i].level<lev) {	//判断层级关系是否正确
                return i;
            }
        }
    }
    return -1;
}
//****************************************************************
void enter(tabl now, enum symbol tp) {			//将字符存入字符表
    for (int i = 0; i<tx; i++) {		//判断表里是否已定义过该变量
        if (!(strcmp(ta[i].ID, now->ID)) && ta[i].level == lev && !(strcmp(ta[i].levelname, levname))) {
            printf(" [错误] 第%d行	变量已定义\n", now->lineN);
            err++;
            return;
        }
    }
    ta[tx].type = tp;
    ta[tx].level = lev;
    strcpy(ta[tx].levelname, levname);
    strcpy(ta[tx].ID, now->ID);
    ta[tx].num = now->NUM;
    ta[tx].adr = dx[lev];
    tx++;
}
//****************************************************************
void factor_syn(tabl &now) {						//factor分析
    void exp_syn(tabl &now);
    int t;
    if (now->SYM != ident&&now->SYM != number&&now->SYM != leftbrackets) {
        printf("[错误]第%d行	缺少因子\n", now->lineN);
        err++;
    }
    else if (now->SYM == leftbrackets) {
        if (now->next == NULL) {
            printf("[错误]缺少内容\n");
            err++;
            return;
        }
        now = now->next;
        exp_syn(now);
        if (now->SYM != rightbrackets) {
            printf("[错误]第%d行	缺少右括号\n", now->lineN);
            err++;
        }
        else {
            if (now->next == NULL) {
                return;
            }
            now = now->next;
        }
    }
    else {
        if (now->SYM == ident) {
            t = position(now->ID);	//判断变量是否在符号表中
            if (t<0) {
                printf("[错误]第%d行	变量未定义\n", now->lineN);
                err++;
            }
            else {
                switch (ta[t].type) {
                    case constS:
                        gen(LIT, 0, ta[t].num);
                        break;
                    case varS:
                        gen(LOD, lev - ta[t].level, ta[t].adr);
                        break;
                    case procedureS:
                        printf("[错误]第%d行	变量引用出错\n", now->lineN);
                        err++;
                        break;
                }
            }
        }
        else if (now->SYM == number) {
            gen(LIT, 0, now->NUM);
        }
        if (now->next == NULL) {
            return;
        }
        now = now->next;
    }
}
//****************************************************************
void term_syn(tabl &now) {						//term分析
    factor_syn(now);
    while (now->SYM == multiply || now->SYM == divide) {
        if (now->next == NULL) {
            printf("[ 错误]缺少内容\n");
            err++;
            return;
        }
        if (now->SYM == multiply) {
            gen(OPR, 0, 4);
        }
        else {
            gen(OPR, 0, 5);
        }
        now = now->next;
        factor_syn(now);
    }
}
//****************************************************************
void exp_syn(tabl &now) {						//exp分析
    if (now->SYM == plus || now->SYM == minus) {
        if (now->next == NULL) {
            printf("[错误 ]缺少内容\n");
            err++;
            return;
        }
        if (now->SYM == minus) {
            gen(OPR, 0, 1);
        }
        now = now->next;
    }
    term_syn(now);
    while (now->SYM == plus || now->SYM == minus) {
        if (now->next == NULL) {
            printf("[错误 ]缺少内容\n");
            err++;
            return;
        }
        if (now->SYM == plus) {
            gen(OPR, 0, 2);
        }
        else {
            gen(OPR, 0, 3);
        }
        now = now->next;
        term_syn(now);
    }
}
//****************************************************************
void lexp_syn(tabl &now) {						//lexp分析
    exp_syn(now);
    if (now->SYM != equal&&now->SYM != notequal&&now->SYM != less&&now->SYM != notmore&&now->SYM != more&&now->SYM != notless) {
        printf("[错误]第%d行	缺少运算符\n", now->lineN);
        err++;
        return;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少内容\n");
            err++;
            return;
        }
        switch (now->SYM) {
            case equal:
                gen(OPR, 0, 10);
                break;
            case notequal:
                gen(OPR, 0, 11);
                break;
            case less:
                gen(OPR, 0, 12);
                break;
            case notmore:
                gen(OPR, 0, 13);
                break;
            case more:
                gen(OPR, 0, 14);
                break;
            case notless:
                gen(OPR, 0, 15);
                break;
        }
        now = now->next;
    }
    if (now->SYM == oddS) {
        if (now->next == NULL) {
            printf("[错误]缺少内容\n");
            err++;
            return;
        }
        gen(OPR, 0, 6);
        now = now->next;
    }
    exp_syn(now);
}
//****************************************************************
void callexp_syn(tabl &now) {					//call中的exp循环
    exp_syn(now);
    if (now->SYM == comma) {
        if (now->next == NULL) {
            printf("[错误]缺少内容\n");
            err++;
            return;
        }
        now = now->next;
        callexp_syn(now);
    }
}
//****************************************************************
void statement_syn(tabl &now) {					//statement分析
    void id_syn(tabl &now);
    void body_syn(tabl &now);
    int j, t;
    int tcall;
    int cx0, cx1, cx2, cx3;
    tabl s, q;
    if (now->next == NULL) {
        printf("[错误]缺少程序体\n");
        err++;
        return;
    }
    for (j = 0, s = now; s->next != NULL; s = s->next) {
        if (s->SYM == ident)
        {
            j = 1;
            t = position(now->ID);	//判断变量是否在符号表中
            if (t<0) {
                printf("[错误]第%d行	变量未定义\n", now->lineN);
                err++;
            }
            else if (ta[t].type != varS) {
                printf("[错误]第%d行	变量引用出错\n", now->lineN);
                err++;
            }
            break;
        }
        else if (s->SYM == ifS) {
            j = 2;
            break;
        }
        else if (s->SYM == whileS) {
            j = 3;
            break;
        }
        else if (s->SYM == callS) {
            j = 4;
            break;
        }
        else if (s->SYM == beginS) {
            j = 5;
            break;
        }
        else if (s->SYM == readS) {
            j = 6;
            break;
        }
        else if (s->SYM == writeS) {
            j = 7;
            break;
        }
    }
    if (now != s) {
        printf("[错误]第%d行	有多余内容\n", now->lineN);
        err++;
        for (; now->next != s; now = now->next) {
            q = now->next;
            printf("[错误]第%d行	有多余内容\n", q->lineN);
            err++;
        }
        now = s;
    }
    if (now->next == NULL) {
        printf("[错误]缺少程序体\n");
        err++;
        return;
    }
    now = now->next;
    switch (j) {
        case 1:	//<id> := <exp>
            if (now->SYM != assignment) {//判断是否有赋值号
                printf("[错误]第%d行	缺少赋值号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    printf("[错误]缺少程序体\n");
                    err++;
                    return;
                }
                if (lev == ta[t].level)
                {
                    ta[t].num = 0;
                }
                now = now->next;
            }
            exp_syn(now);
            gen(STO, lev - ta[t].level, ta[t].adr);
            break;
        case 2:	//if语句
            lexp_syn(now);
            if (now->SYM != thenS) {//判断是否有赋值号
                printf("[错误]第%d行	缺少then\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                cx0 = cx;
                gen(JPC, 0, 0);
                now = now->next;
            }
            statement_syn(now);
            cx1 = cx;
            gen(JMP, 0, 0);
            cod[cx0].a = cx;
            if (now->SYM == elseS) {
                if (now->next == NULL) {
                    printf("[错误]第%d行	else多余\n", now->lineN);
                    err++;
                    return;
                }
                now = now->next;
                statement_syn(now);
            }
            cod[cx1].a = cx;
            break;
        case 3:	//while语句
            cx1 = cx;
            lexp_syn(now);
            if (now->SYM != doS) {//判断是否有赋值号
                printf("[错误]第%d行	缺少do\n", now->lineN);
                err++;
            }
            else {
                cx2 = cx;
                gen(JPC, 0, 0);
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                now = now->next;
            }
            statement_syn(now);
            gen(JMP, 0, cx1);
            cod[cx2].a = cx;
            break;
        case 4:	//call语句
            if (now->SYM != ident) {
                printf("[错误]第%d行	缺少变量名\n", now->lineN);
                err++;
            }
            else {
                t = position(now->ID);		//判断变量是否在符号表中
                if (t<0) {
                    printf("[错误]第%d行	过程未定义\n", now->lineN);
                    err++;
                }
                else if (ta[t].type != procedureS) {
                    printf("[错误]第%d行	变量引用出错\n", now->lineN);
                    err++;
                }
                else {
                    tcall = t;
                    //gen(CAL, lev - ta[t].level, ta[t].adr);
                    //cod[ta[t].num].a=cx;
                }
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                now = now->next;
            }
            if (now->SYM != leftbrackets) {
                printf("[错误]第%d行	缺少左括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                now = now->next;
            }
            if (now->SYM != rightbrackets)
                callexp_syn(now);
            if (now->SYM != rightbrackets) {
                printf("[错误]第%d行	缺少右括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    return;
                }
                now = now->next;
            }
            gen(CAL, lev - ta[tcall].level, ta[tcall].adr);
            break;
        case 5:	//<body>
            now = s;
            body_syn(now);
            break;
        case 6:	//read语句
            if (now->SYM != leftbrackets) {
                printf("[错误]第%d行	缺少左括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                now = now->next;
            }
            while (now->SYM != rightbrackets)
            {
                if (now->SYM != ident) {//判断是否有ident
                    printf("[错误]第%d行	缺少右括号\n", now->lineN);
                    err++;
                }
                else {
                    t = position(now->ID);	//判断变量是否在符号表中
                    if (t<0) {
                        printf("[错误]第%d行	变量未定义\n", now->lineN);
                        err++;
                    }
                    else if (ta[t].type != varS) {
                        printf("[错误]第%d行	变量引用出错\n", now->lineN);
                        err++;
                    }
                    else {
                        ta[t].num = 0;
                        gen(RED, lev - ta[t].level, ta[t].adr);	//生成代码
                    }
                    if (now->next == NULL) {
                        printf("[错误]缺少内容\n");
                        err++;
                        return;
                    }
                    now = now->next;
                }
                if (now->SYM != comma) {
                    break;
                }
                else {
                    if (now->next == NULL) {
                        printf("[错误]缺少内容\n");
                        err++;
                        return;
                    }
                    now = now->next;
                }
            }
            if (now->SYM != rightbrackets) {
                printf("[错误]第%d行	缺少右括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    return;
                }
                now = now->next;
            }
            break;
        case 7:	//write语句
            if (now->SYM != leftbrackets) {
                printf("[错误]第%d行	缺少左括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    printf("[错误]缺少内容\n");
                    err++;
                    return;
                }
                now = now->next;
            }
            while (now->SYM != rightbrackets)
            {
                exp_syn(now);
                gen(WRT, 0, 0);
                if (now->SYM != comma) {
                    break;
                }
                else {
                    if (now->next == NULL) {
                        printf("[错误]缺少内容\n");
                        err++;
                        return;
                    }
                    now = now->next;
                }
            }
            if (now->SYM != rightbrackets) {
                printf("[错误]第%d行	缺少右括号\n", now->lineN);
                err++;
            }
            else {
                if (now->next == NULL) {
                    return;
                }
                now = now->next;
            }
    }
    if (now->SYM == endS) {
        return;
    }
    else if (now->SYM != semicolon) {//判断是否有分号
        if (now->next != NULL&&now->SYM != beginS) {
            printf("[错误]第%d行	缺少分号\n", now->lineN);
            err++;
        }
        else
            return;
    }
    else {
        if (now->next == NULL) {
            return;
        }
        now = now->next;
    }
    statement_syn(now);
}
//****************************************************************
void body_syn(tabl &now) {						//body分析
    if (now->SYM == beginS) {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    else {
        printf("[错误]第%d行	缺少begin\n", now->lineN);
        err++;
    }
    statement_syn(now);
    if (now->SYM != endS) {
        printf("[错误]第%d行	缺少end\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            return;
        }
        now = now->next;
    }
}
//****************************************************************
void id_syn(tabl &now) {						//procedure参数循环
    int Noid, Noco = 0;
    if (now->SYM != ident) {//判断是否有ident
        Noid = 1;
    }
    else {
        dx[lev]++;
        enter(now, varS);
        Noid = 0;
    }
    if (now->next == NULL) {
        printf("[错误]缺少程序体\n");
        err++;
        return;
    }
    now = now->next;
    if (now->SYM == comma) {//判断是否是逗号
        if (Noid) {
            printf("[错误]第%d行	缺少参数\n", now->lineN);
            err++;
        }
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
        id_syn(now);
        return;
    }
    else if (now->SYM == ident) {//判断是否是ident
        printf("[错误]第%d行	缺少逗号\n", now->lineN);
        err++;
        id_syn(now);
        return;
    }
}
//****************************************************************
void proc_syn(tabl &now) {					//procedure分析
    void block_syn(tabl &);
    int tx0;
    char name[5];
    if (now->SYM != ident) {//判断是否有ident
        printf("[错误]第%d行	缺少程序名\n", now->lineN);
        err++;
    }
    else {
        tx0 = tx;
        enter(now, procedureS);
        strcpy(name, levname);
        strcpy(levname, now->ID);
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM != leftbrackets) {//判断是否有左括号
        printf("[错误]第%d行	缺少左括号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    lev++;
    dx[lev] = 0;

    id_syn(now);//进入参数分析
    if (now->SYM != rightbrackets) {//判断是否有右括号
        printf("[错误]第%d行	缺少右括号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM != semicolon) {//判断是否有分号
        printf("[错误]第%d行	缺少分号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    ta[tx0].adr = cx;
    block_syn(now);
    strcpy(levname, name);
    ta[tx0].num = cx;

    gen(RET, 0, 0);
    lev--;
    if (now->SYM != semicolon) {//判断是否有分号
        return;
    }
    else {
        if (now->next == NULL || now->next->next == NULL) {
            printf("[错误]第%d行	有多余内容\n", now->lineN);
            err++;
            return;
        }
        now = now->next;
        if (now->SYM == procedureS) {
            now = now->next;
            proc_syn(now);
        }
        else {
            printf("[错误]第%d行	有多余内容\n", now->lineN);
            err++;
            now = now->next;
        }
    }
}
//****************************************************************
void var_syn(tabl &now) {						//var分析
    void blockbegin_syn(tabl &);
    if (now->SYM != ident) {//判断是否有ident
        printf("[错误]第%d行	缺少变量名\n", now->lineN);
        err++;
    }
    else {
        dx[lev]++;
        enter(now, varS);
        ta[tx - 1].num = 0xffffffff;
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM == comma) {//判断是否有逗号
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
        var_syn(now);
        return;
    }
    else if (now->SYM != semicolon) {//判断是否有分号
        printf("[错误]第%d行	缺少分号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    blockbegin_syn(now);
}
//****************************************************************
void const_syn(tabl &now) {						//const分析
    void blockbegin_syn(tabl &);
    if (now->SYM != ident) {//判断是否有ident
        printf("[错误]第%d行	缺少常量名\n", now->lineN);
        err++;
    }
    else {
        enter(now, constS);
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM != assignment) {//判断是否有赋值号
        printf("[错误]第%d行	缺少赋值号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM != number) {//判断是否有number
        printf("[错误]第%d行	缺少常数\n", now->lineN);
        err++;
    }
    else {
        ta[tx - 1].num = now->NUM;
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM == comma) {//判断是否有逗号
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
        const_syn(now);
        return;
    }
    else if (now->SYM != semicolon) {//判断是否有分号
        printf("[错误]第%d行	缺少分号\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    blockbegin_syn(now);
}
//****************************************************************
void blockbegin_syn(tabl &now) {					//block前部分循环
    int j;
    tabl s, q;
    if (now->next == NULL) {
        printf("[错误]缺少程序体\n");
        err++;
        return;
    }
    for (j = 0, s = now; s->next != NULL; s = s->next) {
        if (s->SYM == constS)
        {
            j = 1;
            break;
        }
        else if (s->SYM == varS) {
            j = 2;
            break;
        }
        else if (s->SYM == procedureS) {
            j = 3;
            break;
        }
        else if (s->SYM == beginS) {
            return;
        }
    }
    if (now != s) {
        q = now->next;
        if (now->SYM == ident) {
            if (q->SYM == assignment) {
                if (q->next->SYM == number) {
                    printf("[错误]第%d行	缺少const\n", now->lineN);
                    err++;
                    j = 1;
                }
                else if (q->next->SYM == ident || q->next->SYM == plus || q->next->SYM == minus) {
                    body_syn(now);
                    return;
                }
            }
            else if (q->SYM == comma || q->next->SYM == number) {
                printf("[错误]第%d行	缺少var\n", now->lineN);
                err++;
                j = 2;
            }
            else if (q->SYM == leftbrackets) {
                printf("[错误]第%d行	缺少procedure\n", now->lineN);
                err++;
                j = 3;
            }
        }
        else if (now->SYM == ifS || now->SYM == whileS || now->SYM == callS || now->SYM == readS || now->SYM == writeS) {
            body_syn(now);
            return;
        }
        else {
            printf("[错误]第%d行	有多余内容\n", now->lineN);
            err++;
            for (; now->next != s; now = now->next) {
                q = now->next;
                printf("[错误]第%d行	有多余内容\n", q->lineN);
                err++;
            }
            if (s->next == NULL) {
                printf("[错误]缺少程序体\n");
                err++;
                return;
            }
            now = s->next;
        }
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    switch (j) {
        case 1:	//const分析
            const_syn(now);
            break;
        case 2:	//var分析
            var_syn(now);
            break;
        case 3:	//procedure分析
            proc_syn(now);
            break;
    }
}
//****************************************************************
void block_syn(tabl &now) {						//block分析
    int cx0;
    cx0 = cx;
    gen(JMP, 0, 0);
    blockbegin_syn(now);
    cod[cx0].a = cx;
    gen(INT, 0, dx[lev] + 3);
    if (now->next != NULL) {
        body_syn(now);	//转入body分析
    }
}
//****************************************************************
void prog_syn(tabl &now) {						//prog语法分析
    tabl s, p = now;
    if (now == NULL) {
        return;
    }
    for (s = now; s->next != NULL; s = s->next) {
        if (s->SYM == programS)
            break;
    }if (s->SYM != programS) {	//判断是否有“program”
        printf("[错误]缺少program\n");
        err++;
    }
    else if (now != s) {
        printf("[错误]第%d行	不在程序体内\n", now->lineN);
        err++;
        for (tabl q; now->next != s; now = now->next) {
            q = now->next;
            if (q->lineN != now->lineN)
                printf("[错误]第%d行	不在程序体内\n", q->lineN);
            err++;
        }
        now = s;
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;

    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        now = now->next;
    }
    if (now->SYM != ident) {	//判断是否有ident
        printf("[错误]第%d行	缺少程序名\n", now->lineN);
        err++;
    }
    else {
        if (now->next == NULL) {
            printf("[错误]缺少程序体\n");
            err++;
            return;
        }
        strcpy(levname, now->ID);
        now = now->next;
    }
    for (s = now; s->next != NULL&&s->SYM != semicolon; s = s->next) {	//判断是否有分号
        if (s->SYM == constS || s->SYM == varS || s->SYM == procedureS || s->SYM == beginS) {
            if (now != s) {
                printf("[错误]第%d行	有多余内容\n", now->lineN);
                err++;
                for (tabl q; now->next != s; now = now->next) {
                    q = now->next;
                    printf("[错误]第%d行	有多余内容\n", q->lineN);
                    err++;
                }
            }
            for (now = p; now->next != s; now = now->next);
            printf("[错误]第%d行	缺少分号\n", now->lineN);
            err++;
            break;
        }
        else {
            printf("[错误]第%d行	有多余内容\n", s->lineN);
            err++;
        }
    }
    if (now->next == NULL) {
        printf("[错误]缺少程序体\n");
        err++;
        return;
    }
    now = now->next;
    block_syn(now);//转入block分析
    if (now->next != NULL || now->SYM != endS) {
        printf("[错误]第%d行有多余内容\n", now->lineN);
        err++;
        for (tabl s = now->next; s != NULL; s = s->next) {
            printf("[错误]第%d行有多余内容\n", s->lineN);
            err++;
        }
        return;
    }
    for (int k = 0; k < tx; k++)
    {
        if (ta[k].type == varS)
        {
            if (ta[k].num == 0xffffffff)
            {
                printf("[错误]变量%s未赋初值\n", ta[k].ID);
            }
        }
    }
}
//****************************************************************
int main() {
    lexical();		//开始词法分析
    tabl tb, p, s;
    tb = (tabl)malloc(sizeof(tabnode));//初始化链表
    if (!tb)
    {
        printf("空间申请失败\n");
        exit(0);
    }
    tb->next = NULL;
    FILE *fp = NULL;
    savelex();
    fp = fopen("lexical.txt", "rb");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return 0;
    }
    p = tb;
    while (!feof(fp)) {			//建立单词链表
        s = (tabl)malloc(sizeof(tabnode));
        if (!s)
        {
            printf("空间申请失败\n");
            exit(0);
        }
        fread(&tab, sizeof(struct word), 1, fp);
        if (feof(fp))
        {
            free(s);
            break;
        }
        s->SYM = tab.SYM;
        strcpy(s->ID, tab.ID);
        s->NUM = tab.NUM;
        s->lineN = tab.lineN;
        s->next = NULL;
        p->next = s;
        p = s;
    }
    fclose(fp);
    p = tb->next;
    prog_syn(p);//开始语法分析
    printf("\n语法分析结束\n");
    printf("\n %d 错误\n\n", err);
    if (!err) {
        getcode();
        run(cx, cod);
    }
    freelink(tb);

    return 0;
}
