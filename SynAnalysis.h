#ifndef __syn_h
#define __syn_h
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include <fstream>
#include<sstream>
#include<cstring>
#include<vector>
#include<stack>
using namespace std;

struct quad//四元式
{
    string op;//操作
    string arg1;//操作数
    string arg2;//操作数
    string result;//结果
    quad(string o,string a1,string a2,string re):op(o),arg1(a1),arg2(a2),result(re){};
};

struct id//标识符
{
    string name;//名称
    int key;//值
    id(){};
    id(string n):name(n){};
    id(string n,int k):name(n),key(k){};
    id(int k):name("0"),key(k){};
};

class SynAnalysis
{
public:
    SynAnalysis()
    {

    }
    ifstream in;
    ofstream out;
    vector<string>s;//保存二元式对应的字符串
    vector<int>C;//常数表
    vector<quad>Quadruple;//四元式表
    vector<id>Identify;//标识符表
    stack<id>tempnum;//数字栈
    stack<string>tempop;//运算符栈
    int p;//跳转指针
    //stack<int>condition;//条件语句跳转栈
    int temp[100000];//保存中间变量Tn
    int tempcount;//保存中间变量Tn的个数
    int error;//记录报错字符串位置
    char s1[10000000];//用于保存int转成string类型的中间类型
    stack<int>TC;//真出口
    stack<int>FC;//假出口
    stack<int>chainfrom;//if的无条件跳转，记录从哪跳转
    stack<int>chainto;//while的无条件跳转，记录跳转到哪
    int nestification;//记录if嵌套个数
    void ReadBinaryPattern();
    bool Is_Identifier(int &a);
    bool Is_C(int &a);
    bool Is_Program(int &a);
    bool Is_ProgramBody(int &a);
    bool Is_VaribleDiscription(int &a);
    bool Is_VaribleDiscriptionTable(int &a,bool flag);//flag嵌套标记
    bool Is_Type(int &a);
    bool Is_VaribleTable(int &a,bool flag);
    bool Is_SentenceString(int &a,bool flag);
    bool Is_Sentence(int &a);
    bool Is_CompoundStatement(int &a);
    bool Is_AssignmentStatement(int &a);
    bool Is_ConditionalStatement(int &a);
    bool Is_LoopStatement(int &a);
    bool Is_Expression(int &a);
    bool Is_Term(int &a);
    bool Is_Factor(int &a);
    bool Is_BoolExpression(int &a);
    bool Is_RelationExpression(int &a);
    bool Is_Relation(int &a);
    void SynSolve();
};

#endif
