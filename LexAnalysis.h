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
    char token[40];//token为存放的单词自身字符串；
    char ch;
    int syn;//syn为单词种别码；
    int m = 0;//指向字符串的第m个字母
    int countbl = 0;//count记录在保留字表中的第几个
    int IDentifierSize = 0;//记录标识符的个数
    int countIDentifier = 0;//count记录在标识符表中的第几个
    bool flat = 1;//标记是否需要往后读一个字符
    string IDentifier[100];//保存标识符
    int line = 1;//记录第几行
    ifstream in;
    ofstream out;

    void scanner();
    void GetBinaryPattern();

    static string Reserved_word[x];//保留字
    static string character[y];//其他字符
};







#endif
