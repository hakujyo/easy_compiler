#ifndef __lex_h
#define __lex_h
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include <fstream>
#include<cstring>
using namespace std;

class LexAnalysis
{
public :
    LexAnalysis()
    {

    }
    static const int x = 14;
    static const int y = 30;
    char token[40];//tokenΪ��ŵĵ��������ַ�����
    char ch;
    int syn;//synΪ�����ֱ��룻
    int m = 0;//ָ���ַ����ĵ�m����ĸ
    int countbl = 0;//count��¼�ڱ����ֱ��еĵڼ���
    int IDentifierSize = 0;//��¼��ʶ���ĸ���
    int countIDentifier = 0;//count��¼�ڱ�ʶ�����еĵڼ���
    bool flat = 1;//����Ƿ���Ҫ�����һ���ַ�
    string IDentifier[100];//�����ʶ��
    int line = 1;//��¼�ڼ���
    ifstream in;
    ofstream out;

    void scanner();
    void GetBinaryPattern();

    static string Reserved_word[x];//������
    static string character[y];//�����ַ�
};







#endif
