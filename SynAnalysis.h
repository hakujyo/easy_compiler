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

struct quad//��Ԫʽ
{
    string op;//����
    string arg1;//������
    string arg2;//������
    string result;//���
    quad(string o,string a1,string a2,string re):op(o),arg1(a1),arg2(a2),result(re){};
};

struct id//��ʶ��
{
    string name;//����
    int key;//ֵ
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
    vector<string>s;//�����Ԫʽ��Ӧ���ַ���
    vector<int>C;//������
    vector<quad>Quadruple;//��Ԫʽ��
    vector<id>Identify;//��ʶ����
    stack<id>tempnum;//����ջ
    stack<string>tempop;//�����ջ
    int p;//��תָ��
    //stack<int>condition;//���������תջ
    int temp[100000];//�����м����Tn
    int tempcount;//�����м����Tn�ĸ���
    int error;//��¼�����ַ���λ��
    char s1[10000000];//���ڱ���intת��string���͵��м�����
    stack<int>TC;//�����
    stack<int>FC;//�ٳ���
    stack<int>chainfrom;//if����������ת����¼������ת
    stack<int>chainto;//while����������ת����¼��ת����
    int nestification;//��¼ifǶ�׸���
    void ReadBinaryPattern();
    bool Is_Identifier(int &a);
    bool Is_C(int &a);
    bool Is_Program(int &a);
    bool Is_ProgramBody(int &a);
    bool Is_VaribleDiscription(int &a);
    bool Is_VaribleDiscriptionTable(int &a,bool flag);//flagǶ�ױ��
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
