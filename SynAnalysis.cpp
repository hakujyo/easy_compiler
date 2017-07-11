#include "SynAnalysis.h"
#include"LexAnalysis.h"
/*******************************文法*********************************/
//1.<程序> → program id ; <程序体>
//2.<程序体> → <变量说明>begin <语句串> end .
//3.<变量说明> → var <变量说明表>;
//4.<变量说明表> → <变量表>：<类型> | <变量表>：<类型>；<变量说明表>
//5.<类型> → integer
//6.<变量表> → <变量> | <变量>,<变量表>
//7.<语句串> → <语句> | <语句>;<语句串>
//8.<语句> → <复合语句>|<赋值语句>|<条件语句>|<循环语句>
//9.<复合语句> → begin <语句串>end
//10.<赋值语句> → id:=<表达式>
//11.<条件语句> → if<布尔表达式>　then　<语句>｜if<布尔表达式>　then　<语句> 　else <语句>
//12.<循环语句> → while　<布尔表达式>　do　<语句>
//13.<表达式> → <项>｜<项>+<表达式>
//14.<项> → <因子>｜<因子>*<因子>
//15.<因子> → (<表达式>) | <标识符> | <常数>
//16.<布尔表达式> → <关系表达式> | ！<关系表达式>
//                  | !(<布尔表达式>)| !（<布尔表达式>）&&<布尔表达式>|!（<布尔表达式>）||<布尔表达式>
//                  |<关系表达式>&&<布尔表达式>|<关系表达式>||<布尔表达式>
//                  |（<布尔表达式>）|（<布尔表达式>）&&<布尔表达式>|（<布尔表达式>）||<布尔表达式>
//17.<关系表达式> → <表达式><关系><表达式>|（<表达式>）
//18.<关系> → <　|　<=　|　>　|　>=　|　=　|　<>

/********************************************************************/
/******************************编码*********************************/
//标识符：(1，标识符表指针)  正整数(2，正整数值)
//program：(3,0)      var：(4,0)              procedure：(5,0)
//begin： (6,0)       end：(7,0)              if：(8,0)
//then：(9,0)         else：(10,0)            while：(11,0)
//do：(12,0)          call：(13,0)            integer：(14,0)
//real：(15,0)        +：(16,0)               －：(17,0)
//*：(18,0)           /：(19,0)               ~：(20,0)
//&：(21,0)           |：(22,0)               <：(23,0)
//<= ：(24,0)         >：(25,0)               >=：(26,0)
//= ：(27,0)          <>：(28,0)              :=：(29,0)
//; ：(30,0)          . ：(31,0)              , ：(32,0)
//( ： (33,0)         ) ：(34,0)              : :(35,0)
// !:(36,0)
/********************************************************************/


LexAnalysis la;

void SynAnalysis::ReadBinaryPattern()
{
    //(a,b)\n
    char c;
    int a,b;
    int i=0;
    while(!in.eof())
    {
        in>>c>>a>>c>>b>>c;
        //cout<<a<<" "<<b<<endl;
        if(a==1) {s.push_back(la.IDentifier[b-1]);/*cout<<la.IDentifier[b-1]<<endl;*/}//标识符
        else if(a==2)
        {

            itoa(b,s1,10);
            s.push_back(s1);
            C.push_back(b);
        }
        else if(3<=a&&a<=15){s.push_back(la.Reserved_word[a-3]);/*cout<<la.Reserved_word[a-3]<<endl;*/}//保留字
        else {s.push_back(la.character[a-16]);/*cout<<la.character[a-16]<<endl;*/}//其他字符
    }
}
//是否是标识符:id,变量
bool SynAnalysis::Is_Identifier(int &a)
{
    a++;if(a>s.size()) return 0;
    for(int i=0; i<la.IDentifierSize; ++i)
    {
        if(la.IDentifier[i]==s[a])
        {
            //标识符入栈
            int find=0;//find=0标记未赋值的标识符
            for(int j=0;j<Identify.size();++j)
            {
                if(Identify[j].name==s[a])
                {
                    find=j;//find!=0标记已赋值的标识符,j标记在Identify表中的指针
                    break;
                }
            }
            if(find) tempnum.push(id(Identify[find].name,Identify[find].key));//已赋值的标识符，直接入栈
            else //未赋值的标识符，先赋值，再入栈
            {
                Identify.push_back(id(s[a]));
                tempnum.push(id(s[a]));
            }
            return 1;
        }
    }
    error=a;
    return 0;
}
//是否是常数
bool SynAnalysis::Is_C(int &a)
{
    a++;if(a>s.size()) return 0;
    for(int i=0; i<C.size(); ++i)
    {
        char w[1000000];
        itoa(C[i],s1,10);
        if(s1==s[a])
        {
            //cout<<s[a]<<endl;
            //常数入栈
            tempnum.push(id(C[i]));
            return 1;
        }
    }
    error=a;
    return 0;
}

//1.<程序> → program id ; <程序体>
bool SynAnalysis::Is_Program(int &a)
{
    TC.push(0);
    FC.push(0);
    chainfrom.push(0);
    chainto.push(0);
    nestification=0;
    if(s[a]!="program") {error=a;return 0;}//program
    //id为标识符
    if(!Is_Identifier(a)) return 0;//id
    a++;if(a>s.size()) return 0;
    if(s[a]!=";") {error=a;return 0;}//;
    if(!Is_ProgramBody(a)) return 0;//<程序体>
    return 1;
}
//2.<程序体> → <变量说明>begin <语句串> end .
bool SynAnalysis::Is_ProgramBody(int &a)
{

    if(!Is_VaribleDiscription(a)) return 0;//<变量说明>
    a++;if(a>s.size()) return 0;
    if(s[a]!="begin") {error=a;return 0;}//begin
    if(!Is_SentenceString(a,0))return 0;//<语句串>
    a++;if(a>s.size()) return 0;
    if(s[a]!="end") {error=a;return 0;}//end
    a++;if(a>s.size()) return 0;
    if(s[a]!=".") {error=a;return 0;}//.
    Quadruple.push_back(quad("ret"," "," ","0"));
    return 1;
}
//3.<变量说明> → var <变量说明表>;
bool SynAnalysis::Is_VaribleDiscription(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="var") {error=a;return 0;}//var
    if(!Is_VaribleDiscriptionTable(a,0)) return 0;//<变量说明表>
    a++;if(a>s.size()) return 0;
    if(s[a]!=";") {error=a;return 0;}//;
    return 1;
}
//4.<变量说明表> → <变量表>：<类型> | <变量表>：<类型>；<变量说明表>
bool SynAnalysis::Is_VaribleDiscriptionTable(int &a,bool flag)
{
    if(flag)//嵌套
    {
        a++;if(a>s.size()) return 0;
        if(s[a]!=";") {error=a;return 0;}
    }

    if(!Is_VaribleTable(a,0)) return 0;//<变量表>
    a++;if(a>s.size()) return 0;
    if(s[a]!=":") {error=a;return 0;}//;
    if(!Is_Type(a)) return 0;//<类型>
    /////////////////////////////////////////////
    int b=a;//用来作回溯标记
    while(Is_VaribleDiscriptionTable(b,1)) a=b;
    return 1;
}
//5.<类型> → integer
bool SynAnalysis::Is_Type(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="integer") {error=a;return 0;}
    return 1;
}
//6.<变量表> → <变量> | <变量>,<变量表>
bool SynAnalysis::Is_VaribleTable(int &a,bool flag)
{
    if(flag)//嵌套
    {
        a++;if(a>s.size()) return 0;
        if(s[a]!=",") {error=a;return 0;}
    }
    if(!Is_Identifier(a)) return 0;
    //cout<<s[a]<<endl;
    //////////////////////////////
    int b=a;
    while(Is_VaribleTable(b,1)) a=b;
    return 1;
}
//7.<语句串> → <语句> | <语句>;<语句串>
bool SynAnalysis::Is_SentenceString(int &a,bool flag)
{
    if(flag)//嵌套
    {
        a++;if(a>s.size()) return 0;
        if(s[a]!=";") {error=a;return 0;}
    }
    if(!Is_Sentence(a)) return 0;
    //////////////////////////
    int b=a;
    while(Is_SentenceString(b,1)) a=b;
    return 1;
}
//8.<语句> → <复合语句>|<赋值语句>|<条件语句>|<循环语句>
bool SynAnalysis::Is_Sentence(int &a)
{
    int b=a;
    if(Is_CompoundStatement(b))
    {
        a=b;
        return 1;
    }
    b=a;
    if(Is_AssignmentStatement(b))
    {
        a=b;
        return 1;
    }
    b=a;
    if(Is_ConditionalStatement(b))
    {
        a=b;
        return 1;
    }
    b=a;
    if(Is_LoopStatement(b))
    {
        a=b;
        return 1;
    }
    return 0;
}
//9.<复合语句> → begin <语句串>end
bool SynAnalysis::Is_CompoundStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="begin") {error=a;return 0;}
    if(!Is_SentenceString(a,0)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="end") {error=a;return 0;}
    return 1;
}
//10.<赋值语句> → id:=<表达式>
bool SynAnalysis::Is_AssignmentStatement(int &a)
{
    if(!Is_Identifier(a))return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!=":=") {error=a;return 0;}
    tempop.push(":=");//":="入栈
    if(!Is_Expression(a)) return 0;
    ////////////////////////////////////////////////////////
    id num2,num1;
    if(!tempnum.empty())
    {
        num2=tempnum.top();//弹出数据栈元素（:=右边元素）
        tempnum.pop();
    }
    if(!tempnum.empty())
    {
        num1=tempnum.top();//弹出数据栈元素（:=左边元素）
        tempnum.pop();
    }
    else num1.key=0;
    int find=0;
    for(int j=0;j<Identify.size();++j)
    {
        if(Identify[j].name==num1.name)
        {
            find=j;//find!=0标记已赋值的标识符,j标记在Identify表中的指针
            break;
        }
    }
    if(!find) Identify.push_back(id(num1.name,num2.key));//未赋值的标识符
    else Identify[find].key=num2.key;//若已赋过值，更新值
    //push进四元式表（:=赋值操作）
    string arg1;
    if(num2.name=="0") {itoa(num2.key,s1,10);arg1=string(s1);}
    else arg1=num2.name;
    Quadruple.push_back(quad(":=",arg1," ",num1.name));
    /////////////////////////////////////////////////////////////////
    return 1;
}
//11.<条件语句> → if<布尔表达式>　then　<语句>｜if<布尔表达式>　then　<语句> 　else <语句>
bool SynAnalysis::Is_ConditionalStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="if") {error=a;return 0;}
    /////////////////////////////////////////////////////
    //遇到"if"，push新的TC,FC,chainfrom
    TC.push(0);
    FC.push(0);
    chainfrom.push(0);
    nestification++;//嵌套+1
    /////////////////////////////////////////////////////
    if(!Is_BoolExpression(a)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="then") {error=a;return 0;}
    //遇到"then"，TC出栈;
    p=Quadruple.size()+1;
    itoa(p,s1,10);
    while(TC.top())
    {
        int tc=TC.top();
        TC.top()=atoi(Quadruple[tc-1].result.c_str());
        Quadruple[tc-1].result=s1;
    }
    /////////////////////////////////////////
    if(!Is_Sentence(a)) return 0;
    /////////////////////////////////////////
    if(s[a+1]=="else")
    {
        a++;if(a>s.size()) return 0;
        ///////////////////////////////////////////////////
        //有else语句，真出口语句结束需无条件跳转
        Quadruple.push_back(quad("j"," "," ","0"));
        if(!chainfrom.top())chainfrom.top()=Quadruple.size();
        else
        {
            itoa(chainfrom.top(),s1,10);
            Quadruple[Quadruple.size()-1].result=s1;
            chainfrom.top()=Quadruple.size();
        }
        ///////////////////////////////////////////////////
        //布尔表达式为0,FC出栈
        p=Quadruple.size()+1;
        itoa(p,s1,10);
        while(FC.top())
        {
            int fc=FC.top();
            FC.top()=atoi(Quadruple[fc-1].result.c_str());
            Quadruple[fc-1].result=s1;
        }
        ///////////////////////////////////////////////////
        if(!Is_Sentence(a)) return 0;
    }
    //条件语句结束前，回填无条件跳转
    itoa(Quadruple.size()+1,s1,10);
    while(chainfrom.top())
    {
        int ch=chainfrom.top();
        chainfrom.top()=atoi(Quadruple[ch-1].result.c_str());
        Quadruple[ch-1].result=s1;
    }
    chainfrom.top()=0;
    ///////////////////////////////////////////////////
    if(nestification==1)//已完成所有嵌套
    {
        //布尔表达式为0,FC出栈
        p=Quadruple.size()+1;
        itoa(p,s1,10);
        while(FC.top())
        {
            int fc=FC.top();
            FC.top()=atoi(Quadruple[fc-1].result.c_str());
            Quadruple[fc-1].result=s1;
        }
        nestification--;
        TC.pop();
        FC.pop();
        chainfrom.pop();
    }
    else//还有嵌套未完成
    {
        //将这级的FC赋给上一级的chainfrom，完成无条件跳转
        if(!chainfrom.top()) chainfrom.pop();//chainfrom出栈
        if(FC.top())//如果这级FC不为0，则赋给上一级的chainfrom
        {
            if(!chainfrom.top())chainfrom.top()=FC.top();
            else
            {
                itoa(chainfrom.top(),s1,10);
                Quadruple[FC.top()-1].result=s1;
                chainfrom.top()=FC.top();
            }
        }
        FC.pop();//FC出栈
        TC.pop();//TC出栈
        nestification--;
    }
    return 1;
}
//12.<循环语句> → while　<布尔表达式>　do　<语句>
bool SynAnalysis::Is_LoopStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="while") {error=a;return 0;}
    /////////////////////////////////////////////////////
    //遇到"while"，push新的TC,FC,chainfrom
    TC.push(0);
    FC.push(0);
    chainto.push(0);
    /////////////////////////////////////////////////////
    //while语句一定需要无条件跳转，跳转到while后第一句
    chainto.top()=Quadruple.size()+1;
    ///////////////////////////////////////////////////
    if(!Is_BoolExpression(a)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="do"){error=a;return 0;}
    //遇到"do"，TC出栈;
    p=Quadruple.size()+1;
    itoa(p,s1,10);
    while(TC.top())
    {
        int tc=TC.top();
        TC.top()=atoi(Quadruple[tc-1].result.c_str());
        Quadruple[tc-1].result=s1;
    }
    /////////////////////////////////////////
    if(!Is_Sentence(a)) return 0;
    ///////////////////////////////////////////////////
    //do语句结束，进行无条件跳转
    Quadruple.push_back(quad("j"," "," ","0"));
    itoa(Quadruple.size()+1,s1,10);
    if(chainto.top())
    {
        itoa(chainto.top(),s1,10);
        Quadruple[Quadruple.size()-1].result=s1;
    }
    chainto.pop();
    ///////////////////////////////////////////////////
    //循环语句结束前,布尔表达式为0,FC出栈
    p=Quadruple.size()+1;
    itoa(p,s1,10);
    while(FC.top())
    {
        int fc=FC.top();
        FC.top()=atoi(Quadruple[fc-1].result.c_str());
        Quadruple[fc-1].result=s1;
    }
    TC.pop();
    FC.pop();
    chainto.pop();
    return 1;
}
//13.<表达式> → <项>｜<项>+<表达式>
bool SynAnalysis::Is_Expression(int &a)
{

    if(!Is_Term(a)) return 0;
    if(s[a+1]=="+") //<项>+<表达式>
    {
        a++;
        ////////////////////////////////////////////
        if(!tempop.empty())
        {
        if(tempop.top()=="+")//当下一个字符为"+"且操作符栈内top操作符为"+"时，前面两个数可以相加
        {
            id num1,num2;
            num2=tempnum.top();//arg2出栈
            tempnum.pop();
            num1=tempnum.top();//arg1出栈
            tempnum.pop();
            tempop.pop();//"+"出栈
            temp[++tempcount]=num1.key+num2.key;
            itoa(tempcount,s1,10);
            string result="T"+string(s1);//保存四元式的result
            tempnum.push(id(result,temp[tempcount]));
            string arg1,arg2;//保存四元式的arg1，arg2
            if(num1.name=="0")//num1为常数
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2为常数
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            arg2=num2.name;
            Quadruple.push_back(quad("+",arg1,arg2,result));
        }
        }
        tempop.push("+");//将"+"入栈
        ////////////////////////////////////////////
        if(!Is_Expression(a)) return 0;
        ///////////////////////////////////////////
        if(tempop.top()=="+")
        {
            id num1,num2;
            num2=tempnum.top();//arg2出栈
            tempnum.pop();
            num1=tempnum.top();//arg1出栈
            tempnum.pop();
            tempop.pop();//"+"出栈
            temp[++tempcount]=num1.key+num2.key;
            itoa(tempcount,s1,10);
            string result="T"+string(s1);//保存四元式的result
            tempnum.push(id(result,temp[tempcount]));
            string arg1,arg2;//保存四元式的arg1，arg2
            if(num1.name=="0")//num1为常数
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2为常数
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            else arg2=num2.name;
            Quadruple.push_back(quad("+",arg1,arg2,result));
        }
        ///////////////////////////////////////////
        return 1;//<项>+<表达式>
    }
    return 1;//<项>
}
//14.<项> → <因子>｜<因子>*<因子>
bool SynAnalysis::Is_Term(int &a)
{
    if(!Is_Factor(a)) return 0;
    ///////////////////////////////////////
    if(s[a+1]=="*")
    {
        a++;if(a>s.size()) return 0;
        tempop.push(s[a]);//"*"入栈
        if(!Is_Factor(a)) return 0;
        //////////////////////////////////////////////////
        //生成中间变量Tn=arg1*arg2
        id num1,num2;
        num2=tempnum.top();//arg2出栈
        tempnum.pop();
        num1=tempnum.top();//arg1出栈
        tempnum.pop();
        tempop.pop();//"*"出栈
        temp[++tempcount]=num1.key*num2.key;
        itoa(tempcount,s1,10);
        string result="T"+string(s1);//保存四元式的result
        tempnum.push(id(result,temp[tempcount]));
        string arg1,arg2;//保存四元式的arg1，arg2
        if(num1.name=="0")//num1为常数
        {
            itoa(num1.key,s1,10);
            arg1=string(s1);
        }
        else arg1=num1.name;
        if(num2.name=="0")//num2为常数
        {
            itoa(num2.key,s1,10);
            arg2=string(s1);
        }
        else arg2=num2.name;
        Quadruple.push_back(quad("*",arg1,arg2,result));
        /////////////////////////////////////////////////////
    }
    return 1;
}
//15.<因子> → (<表达式>) | <标识符> | <常数>
bool SynAnalysis::Is_Factor(int &a)
{
    if(s[a+1]=="(")//(<表达式>)
    {
        tempop.push("(");//"("入栈
        a++;if(a>s.size()) return 0;
        if(!Is_Expression(a)) return 0;
        a++;if(a>s.size()) return 0;
        if(s[a]!=")") {error=a;return 0;}
        tempop.pop();//"("出栈
        return 1;
    }

    int b=a;
    if(Is_Identifier(b)) {a=b;return 1;}//<标识符>

    b=a;
    if(Is_C(b)) {a=b;return 1;}//<常数>
    error=a;
    return 0;
}
//16.<布尔表达式> → <关系表达式> | ！<关系表达式>
//                  | !(<布尔表达式>)| !（<布尔表达式>）&&<布尔表达式>|!（<布尔表达式>）||<布尔表达式>
//                  |<关系表达式>&&<布尔表达式>|<关系表达式>||<布尔表达式>
//                  |（<布尔表达式>）|（<布尔表达式>）&&<布尔表达式>|（<布尔表达式>）||<布尔表达式>
bool SynAnalysis::Is_BoolExpression(int &a)
{
    if(s[a+1]=="!"&&s[a+2]=="(")
    {
        a++;
        a++;
        ////////////////////////////////////////////
        TC.push(0);
        FC.push(0);
        ////////////////////////////////////////////
        int b=a;
        if(!Is_BoolExpression(b))  return 0;
        else
        {
            a=b;
            a++;
            if(s[a]!=")"){error=a;return 0;}
            ///////////////////////////////////////////////
            int temp;
            temp=TC.top();
            TC.top()=FC.top();
            FC.top()=temp;
            ///////////////////////////////////////////////
            if(!TC.top()) TC.pop();
            else
            {
                int temptc=TC.top();
                int tc;
                while(TC.top())
                {
                    tc=TC.top();
                    TC.top()=atoi(Quadruple[tc-1].result.c_str());
                }
                TC.pop();
                itoa(TC.top(),s1,10);
                Quadruple[tc-1].result=s1;
                TC.top()=temptc;
            }
            if(!FC.top()) FC.pop();
            else
            {
                int tempfc=FC.top();
                int fc;
                while(FC.top())
                {
                    fc=FC.top();
                    FC.top()=atoi(Quadruple[fc-1].result.c_str());
                }
                FC.pop();
                itoa(FC.top(),s1,10);
                Quadruple[fc-1].result=s1;
                FC.top()=tempfc;
            }
            if(Quadruple[Quadruple.size()-2].op=="jnz")Quadruple[Quadruple.size()-2].op="jz";
            else if(Quadruple[Quadruple.size()-2].op=="jz")Quadruple[Quadruple.size()-2].op="jnz";
            ////////////////////////////////////////////////
            if((s[a+1]=="&"&&s[a+2]=="&"))// !（<布尔表达式>）&&<布尔表达式>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //TC出栈
                p=Quadruple.size()+1;
                itoa(p,s1,10);
                while(TC.top())
                {
                    int tc=TC.top();
                    TC.top()=atoi(Quadruple[tc-1].result.c_str());
                    Quadruple[tc-1].result=s1;
                }
                TC.top()=0;
                /////////////////////////////////////////////////

                if(Is_BoolExpression(a)) return 1;
                return 0;
            }
            else if((s[a+1]=="|"&&s[a+2]=="|"))// !（<布尔表达式>）||<布尔表达式>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //FC出栈
                p=Quadruple.size()+1;
                itoa(p,s1,10);
                while(FC.top())
                {
                    int fc=FC.top();
                    FC.top()=atoi(Quadruple[fc-1].result.c_str());
                    Quadruple[fc-1].result=s1;
                }
                FC.top()=0;
                /////////////////////////////////////////////////
                if(Is_BoolExpression(a)) return 1;
                return 0;
            }
            return 1;// !(<布尔表达式>)
        }
    }
    if(s[a+1]=="!")//！<关系表达式>
    {
        a++;
        if(!Is_RelationExpression(a)) return 0;
        //TC入栈
        if(TC.top()==0) TC.top()=Quadruple.size()-1;
        else
        {
            itoa(TC.top(),s1,10);
            Quadruple[Quadruple.size()-2].result=s1;
            TC.top()=Quadruple.size()-1;
        }
        //FC入栈
        if(FC.top()==0) {FC.top()=Quadruple.size();}
        else
        {
            itoa(FC.top(),s1,10);
            Quadruple[Quadruple.size()-1].result=s1;
            FC.top()=Quadruple.size();
        }
        ////////////////////////////////////////////////
        if(Quadruple[Quadruple.size()-2].op=="jnz")Quadruple[Quadruple.size()-2].op="jz";
        else if(Quadruple[Quadruple.size()-2].op=="jz")Quadruple[Quadruple.size()-2].op="jnz";
        return 1;
    }
    if(s[a+1]=="(")//（<布尔表达式>）
    {
        a++;
        ////////////////////////////////////////////
        TC.push(0);
        FC.push(0);
        ////////////////////////////////////////////
        if(Is_BoolExpression(a))
        {
            a++;
            if(s[a]!=")") {error=a;return 0;}
            if(!TC.top()) TC.pop();
            else
            {
                int temptc=TC.top();
                int tc;
                while(TC.top())
                {
                    tc=TC.top();
                    TC.top()=atoi(Quadruple[tc-1].result.c_str());
                }
                TC.pop();
                itoa(TC.top(),s1,10);
                Quadruple[tc-1].result=s1;
                TC.top()=temptc;
            }
            if(!FC.top()) FC.pop();
            else
            {
                int tempfc=FC.top();
                int fc;
                while(FC.top())
                {
                    fc=FC.top();
                    FC.top()=atoi(Quadruple[fc-1].result.c_str());
                }
                FC.pop();
                itoa(FC.top(),s1,10);
                Quadruple[fc-1].result=s1;
                FC.top()=tempfc;
            }
            ////////////////////////////////////////////////
            if((s[a+1]=="&"&&s[a+2]=="&"))//（<布尔表达式>）&&<布尔表达式>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //TC出栈
                p=Quadruple.size()+1;
                itoa(p,s1,10);
                while(TC.top())
                {
                    int tc=TC.top();
                    TC.top()=atoi(Quadruple[tc-1].result.c_str());
                    Quadruple[tc-1].result=s1;
                }
                TC.top()=0;
                /////////////////////////////////////////////////

                if(Is_BoolExpression(a)) return 1;
                return 0;
            }
            else if((s[a+1]=="|"&&s[a+2]=="|"))//（<布尔表达式>）||<布尔表达式>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //FC出栈
                p=Quadruple.size()+1;
                itoa(p,s1,10);
                while(FC.top())
                {
                    int fc=FC.top();
                    FC.top()=atoi(Quadruple[fc-1].result.c_str());
                    Quadruple[fc-1].result=s1;
                }
                FC.top()=0;
                /////////////////////////////////////////////////
                if(Is_BoolExpression(a)) return 1;
                return 0;
            }
            else return 1;//（<布尔表达式>）
        }
        ////////////////////////////////////////////
        return 0;
    }
    if(Is_RelationExpression(a))
    {
        //TC入栈
        if(TC.top()==0) TC.top()=Quadruple.size()-1;
        else
        {
            itoa(TC.top(),s1,10);
            Quadruple[Quadruple.size()-2].result=s1;
            TC.top()=Quadruple.size()-1;
        }
        //FC入栈
        if(FC.top()==0) {FC.top()=Quadruple.size();}
        else
        {
            itoa(FC.top(),s1,10);
            Quadruple[Quadruple.size()-1].result=s1;
            FC.top()=Quadruple.size();
        }
        if((s[a+1]=="&"&&s[a+2]=="&"))//<关系表达式> && <布尔表达式>
        {
            a=a+2;
            //////////////////////////////////////////////////
            //TC出栈
            p=Quadruple.size()+1;
            itoa(p,s1,10);
            while(TC.top())
            {
                int tc=TC.top();
                TC.top()=atoi(Quadruple[tc-1].result.c_str());
                Quadruple[tc-1].result=s1;
            }
            TC.top()=0;
            /////////////////////////////////////////////////

            if(Is_BoolExpression(a)) return 1;
            return 0;
        }
        else if((s[a+1]=="|"&&s[a+2]=="|"))//<关系表达式>||<布尔表达式>
        {
            a=a+2;
            //////////////////////////////////////////////////
            //FC出栈
            p=Quadruple.size()+1;
            itoa(p,s1,10);
            while(FC.top())
            {
                int fc=FC.top();
                FC.top()=atoi(Quadruple[fc-1].result.c_str());
                Quadruple[fc-1].result=s1;
            }
            FC.top()=0;
            /////////////////////////////////////////////////
            if(Is_BoolExpression(a)) return 1;
            return 0;
        }
        else return 1;//<关系表达式>
    }
    /////////////////////////////////////////////////
    return 0;
}
//17.< 关系表达式> → <表达式><关系><表达式>|<表达式>|标识符
bool SynAnalysis::Is_RelationExpression(int &a)
{
    int b=a;
    if(!Is_Expression(b))
    {
        if(Is_Identifier(a))//标识符
        {
            Quadruple.push_back(quad("jnz",s[a]," ","0"));//关系表达式为1
            Quadruple.push_back(quad("j"," "," ","0"));//关系表达式为0
            return 1;
        }
    }
    else
    {
        a=b;
        if(!Is_Relation(b))//<表达式>
        {
            id num1;
            num1=tempnum.top();//arg1出栈
            tempnum.pop();
            string arg1;//保存四元式的arg1
            if(num1.name=="0")//num1为常数
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            Quadruple.push_back(quad("jnz",arg1," ","0"));//关系表达式为1
            Quadruple.push_back(quad("j"," "," ","0"));//关系表达式为0
            return 1;
        }
        else//<表达式><关系><表达式>
        {
            a=b;
            if(!Is_Expression(a)) return 0;
            //////////////////////////////////////
            id num1,num2;
            num2=tempnum.top();//arg2出栈
            tempnum.pop();
            num1=tempnum.top();//arg1出栈
            tempnum.pop();
            string op="j"+tempop.top();//关系符出栈
            tempop.pop();
            string arg1,arg2;//保存四元式的arg1，arg2
            if(num1.name=="0")//num1为常数
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2为常数
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            else arg2=num2.name;
            Quadruple.push_back(quad(op,arg1,arg2,"0"));//关系表达式为1
            Quadruple.push_back(quad("j"," "," ","0"));//关系表达式为0
            return 1;//<表达式><关系><表达式>
        }
    }
}
//18. <关系> → <　|　<=　|　>　|　>=　|　=　|　<>
bool SynAnalysis::Is_Relation(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]=="<"||s[a]=="<="||s[a]==">"||s[a]==">="||s[a]=="="||s[a]=="<>")
    {
        tempop.push(s[a]);
        return 1;
    }
    else{error=a;return 0;}
}

void SynAnalysis::SynSolve()
{
    ///////////////////////////////////////////
    /*              词法分析                */
    //////////////////////////////////////////
    la.GetBinaryPattern();
     ///////////////////////////////////////////
    /*              语法分析                */
    //////////////////////////////////////////
    C.clear();//常数表
    s.clear();
    tempcount=0;
    in.open("二元式.txt");
    out.open("Syntax analysis result.txt");
    ReadBinaryPattern();
    in.close();
    int a=0;
    if(!Is_Program(a))
        {out<<"syntax error"<<endl<<"There is an error arround ";
        if(error-1>=0)out<<s[error-1]<<" ";
        out<<s[error];
        if(error+1<s.size())out<<" "<<s[error+1]<<endl;
    }
    else
    {
        ///////////////////////////////////////////
        /*              语义分析                */
        //////////////////////////////////////////
        //最后处理一下无条件跳转,使得(j,,,)->(j,,,)消除
        for(int i=0;i!=Quadruple.size();++i)
        {
            stack<int>jump;
            if(Quadruple[i].op=="j")
            {
                int goton=atoi(Quadruple[i].result.c_str());
                int temp=i+1;//记录上一次跳转到哪

                while(Quadruple[goton-1].op=="j")
                {
                    jump.push(temp);
                    temp=goton;
                    goton=atoi(Quadruple[goton-1].result.c_str());
                }
                goton=atoi(Quadruple[temp-1].result.c_str());
                itoa(goton,s1,10);
                while(!jump.empty())
                {
                    Quadruple[jump.top()-1].result=s1;
                    jump.pop();
                }
            }
        }
        //////////////////////////////////////////
        for(int i=0;i!=Quadruple.size();++i)
        {
            out<<i+1<<".("<<Quadruple[i].op<<","<<Quadruple[i].arg1<<","<<Quadruple[i].arg2<<","<<Quadruple[i].result<<")"<<endl;
        }
    }
    out.close();
}
