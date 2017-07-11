#include "SynAnalysis.h"
#include"LexAnalysis.h"
/*******************************�ķ�*********************************/
//1.<����> �� program id ; <������>
//2.<������> �� <����˵��>begin <��䴮> end .
//3.<����˵��> �� var <����˵����>;
//4.<����˵����> �� <������>��<����> | <������>��<����>��<����˵����>
//5.<����> �� integer
//6.<������> �� <����> | <����>,<������>
//7.<��䴮> �� <���> | <���>;<��䴮>
//8.<���> �� <�������>|<��ֵ���>|<�������>|<ѭ�����>
//9.<�������> �� begin <��䴮>end
//10.<��ֵ���> �� id:=<���ʽ>
//11.<�������> �� if<�������ʽ>��then��<���>��if<�������ʽ>��then��<���> ��else <���>
//12.<ѭ�����> �� while��<�������ʽ>��do��<���>
//13.<���ʽ> �� <��>��<��>+<���ʽ>
//14.<��> �� <����>��<����>*<����>
//15.<����> �� (<���ʽ>) | <��ʶ��> | <����>
//16.<�������ʽ> �� <��ϵ���ʽ> | ��<��ϵ���ʽ>
//                  | !(<�������ʽ>)| !��<�������ʽ>��&&<�������ʽ>|!��<�������ʽ>��||<�������ʽ>
//                  |<��ϵ���ʽ>&&<�������ʽ>|<��ϵ���ʽ>||<�������ʽ>
//                  |��<�������ʽ>��|��<�������ʽ>��&&<�������ʽ>|��<�������ʽ>��||<�������ʽ>
//17.<��ϵ���ʽ> �� <���ʽ><��ϵ><���ʽ>|��<���ʽ>��
//18.<��ϵ> �� <��|��<=��|��>��|��>=��|��=��|��<>

/********************************************************************/
/******************************����*********************************/
//��ʶ����(1����ʶ����ָ��)  ������(2��������ֵ)
//program��(3,0)      var��(4,0)              procedure��(5,0)
//begin�� (6,0)       end��(7,0)              if��(8,0)
//then��(9,0)         else��(10,0)            while��(11,0)
//do��(12,0)          call��(13,0)            integer��(14,0)
//real��(15,0)        +��(16,0)               ����(17,0)
//*��(18,0)           /��(19,0)               ~��(20,0)
//&��(21,0)           |��(22,0)               <��(23,0)
//<= ��(24,0)         >��(25,0)               >=��(26,0)
//= ��(27,0)          <>��(28,0)              :=��(29,0)
//; ��(30,0)          . ��(31,0)              , ��(32,0)
//( �� (33,0)         ) ��(34,0)              : :(35,0)
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
        if(a==1) {s.push_back(la.IDentifier[b-1]);/*cout<<la.IDentifier[b-1]<<endl;*/}//��ʶ��
        else if(a==2)
        {

            itoa(b,s1,10);
            s.push_back(s1);
            C.push_back(b);
        }
        else if(3<=a&&a<=15){s.push_back(la.Reserved_word[a-3]);/*cout<<la.Reserved_word[a-3]<<endl;*/}//������
        else {s.push_back(la.character[a-16]);/*cout<<la.character[a-16]<<endl;*/}//�����ַ�
    }
}
//�Ƿ��Ǳ�ʶ��:id,����
bool SynAnalysis::Is_Identifier(int &a)
{
    a++;if(a>s.size()) return 0;
    for(int i=0; i<la.IDentifierSize; ++i)
    {
        if(la.IDentifier[i]==s[a])
        {
            //��ʶ����ջ
            int find=0;//find=0���δ��ֵ�ı�ʶ��
            for(int j=0;j<Identify.size();++j)
            {
                if(Identify[j].name==s[a])
                {
                    find=j;//find!=0����Ѹ�ֵ�ı�ʶ��,j�����Identify���е�ָ��
                    break;
                }
            }
            if(find) tempnum.push(id(Identify[find].name,Identify[find].key));//�Ѹ�ֵ�ı�ʶ����ֱ����ջ
            else //δ��ֵ�ı�ʶ�����ȸ�ֵ������ջ
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
//�Ƿ��ǳ���
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
            //������ջ
            tempnum.push(id(C[i]));
            return 1;
        }
    }
    error=a;
    return 0;
}

//1.<����> �� program id ; <������>
bool SynAnalysis::Is_Program(int &a)
{
    TC.push(0);
    FC.push(0);
    chainfrom.push(0);
    chainto.push(0);
    nestification=0;
    if(s[a]!="program") {error=a;return 0;}//program
    //idΪ��ʶ��
    if(!Is_Identifier(a)) return 0;//id
    a++;if(a>s.size()) return 0;
    if(s[a]!=";") {error=a;return 0;}//;
    if(!Is_ProgramBody(a)) return 0;//<������>
    return 1;
}
//2.<������> �� <����˵��>begin <��䴮> end .
bool SynAnalysis::Is_ProgramBody(int &a)
{

    if(!Is_VaribleDiscription(a)) return 0;//<����˵��>
    a++;if(a>s.size()) return 0;
    if(s[a]!="begin") {error=a;return 0;}//begin
    if(!Is_SentenceString(a,0))return 0;//<��䴮>
    a++;if(a>s.size()) return 0;
    if(s[a]!="end") {error=a;return 0;}//end
    a++;if(a>s.size()) return 0;
    if(s[a]!=".") {error=a;return 0;}//.
    Quadruple.push_back(quad("ret"," "," ","0"));
    return 1;
}
//3.<����˵��> �� var <����˵����>;
bool SynAnalysis::Is_VaribleDiscription(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="var") {error=a;return 0;}//var
    if(!Is_VaribleDiscriptionTable(a,0)) return 0;//<����˵����>
    a++;if(a>s.size()) return 0;
    if(s[a]!=";") {error=a;return 0;}//;
    return 1;
}
//4.<����˵����> �� <������>��<����> | <������>��<����>��<����˵����>
bool SynAnalysis::Is_VaribleDiscriptionTable(int &a,bool flag)
{
    if(flag)//Ƕ��
    {
        a++;if(a>s.size()) return 0;
        if(s[a]!=";") {error=a;return 0;}
    }

    if(!Is_VaribleTable(a,0)) return 0;//<������>
    a++;if(a>s.size()) return 0;
    if(s[a]!=":") {error=a;return 0;}//;
    if(!Is_Type(a)) return 0;//<����>
    /////////////////////////////////////////////
    int b=a;//���������ݱ��
    while(Is_VaribleDiscriptionTable(b,1)) a=b;
    return 1;
}
//5.<����> �� integer
bool SynAnalysis::Is_Type(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="integer") {error=a;return 0;}
    return 1;
}
//6.<������> �� <����> | <����>,<������>
bool SynAnalysis::Is_VaribleTable(int &a,bool flag)
{
    if(flag)//Ƕ��
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
//7.<��䴮> �� <���> | <���>;<��䴮>
bool SynAnalysis::Is_SentenceString(int &a,bool flag)
{
    if(flag)//Ƕ��
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
//8.<���> �� <�������>|<��ֵ���>|<�������>|<ѭ�����>
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
//9.<�������> �� begin <��䴮>end
bool SynAnalysis::Is_CompoundStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="begin") {error=a;return 0;}
    if(!Is_SentenceString(a,0)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="end") {error=a;return 0;}
    return 1;
}
//10.<��ֵ���> �� id:=<���ʽ>
bool SynAnalysis::Is_AssignmentStatement(int &a)
{
    if(!Is_Identifier(a))return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!=":=") {error=a;return 0;}
    tempop.push(":=");//":="��ջ
    if(!Is_Expression(a)) return 0;
    ////////////////////////////////////////////////////////
    id num2,num1;
    if(!tempnum.empty())
    {
        num2=tempnum.top();//��������ջԪ�أ�:=�ұ�Ԫ�أ�
        tempnum.pop();
    }
    if(!tempnum.empty())
    {
        num1=tempnum.top();//��������ջԪ�أ�:=���Ԫ�أ�
        tempnum.pop();
    }
    else num1.key=0;
    int find=0;
    for(int j=0;j<Identify.size();++j)
    {
        if(Identify[j].name==num1.name)
        {
            find=j;//find!=0����Ѹ�ֵ�ı�ʶ��,j�����Identify���е�ָ��
            break;
        }
    }
    if(!find) Identify.push_back(id(num1.name,num2.key));//δ��ֵ�ı�ʶ��
    else Identify[find].key=num2.key;//���Ѹ���ֵ������ֵ
    //push����Ԫʽ��:=��ֵ������
    string arg1;
    if(num2.name=="0") {itoa(num2.key,s1,10);arg1=string(s1);}
    else arg1=num2.name;
    Quadruple.push_back(quad(":=",arg1," ",num1.name));
    /////////////////////////////////////////////////////////////////
    return 1;
}
//11.<�������> �� if<�������ʽ>��then��<���>��if<�������ʽ>��then��<���> ��else <���>
bool SynAnalysis::Is_ConditionalStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="if") {error=a;return 0;}
    /////////////////////////////////////////////////////
    //����"if"��push�µ�TC,FC,chainfrom
    TC.push(0);
    FC.push(0);
    chainfrom.push(0);
    nestification++;//Ƕ��+1
    /////////////////////////////////////////////////////
    if(!Is_BoolExpression(a)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="then") {error=a;return 0;}
    //����"then"��TC��ջ;
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
        //��else��䣬���������������������ת
        Quadruple.push_back(quad("j"," "," ","0"));
        if(!chainfrom.top())chainfrom.top()=Quadruple.size();
        else
        {
            itoa(chainfrom.top(),s1,10);
            Quadruple[Quadruple.size()-1].result=s1;
            chainfrom.top()=Quadruple.size();
        }
        ///////////////////////////////////////////////////
        //�������ʽΪ0,FC��ջ
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
    //����������ǰ��������������ת
    itoa(Quadruple.size()+1,s1,10);
    while(chainfrom.top())
    {
        int ch=chainfrom.top();
        chainfrom.top()=atoi(Quadruple[ch-1].result.c_str());
        Quadruple[ch-1].result=s1;
    }
    chainfrom.top()=0;
    ///////////////////////////////////////////////////
    if(nestification==1)//���������Ƕ��
    {
        //�������ʽΪ0,FC��ջ
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
    else//����Ƕ��δ���
    {
        //���⼶��FC������һ����chainfrom�������������ת
        if(!chainfrom.top()) chainfrom.pop();//chainfrom��ջ
        if(FC.top())//����⼶FC��Ϊ0���򸳸���һ����chainfrom
        {
            if(!chainfrom.top())chainfrom.top()=FC.top();
            else
            {
                itoa(chainfrom.top(),s1,10);
                Quadruple[FC.top()-1].result=s1;
                chainfrom.top()=FC.top();
            }
        }
        FC.pop();//FC��ջ
        TC.pop();//TC��ջ
        nestification--;
    }
    return 1;
}
//12.<ѭ�����> �� while��<�������ʽ>��do��<���>
bool SynAnalysis::Is_LoopStatement(int &a)
{
    a++;if(a>s.size()) return 0;
    if(s[a]!="while") {error=a;return 0;}
    /////////////////////////////////////////////////////
    //����"while"��push�µ�TC,FC,chainfrom
    TC.push(0);
    FC.push(0);
    chainto.push(0);
    /////////////////////////////////////////////////////
    //while���һ����Ҫ��������ת����ת��while���һ��
    chainto.top()=Quadruple.size()+1;
    ///////////////////////////////////////////////////
    if(!Is_BoolExpression(a)) return 0;
    a++;if(a>s.size()) return 0;
    if(s[a]!="do"){error=a;return 0;}
    //����"do"��TC��ջ;
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
    //do��������������������ת
    Quadruple.push_back(quad("j"," "," ","0"));
    itoa(Quadruple.size()+1,s1,10);
    if(chainto.top())
    {
        itoa(chainto.top(),s1,10);
        Quadruple[Quadruple.size()-1].result=s1;
    }
    chainto.pop();
    ///////////////////////////////////////////////////
    //ѭ��������ǰ,�������ʽΪ0,FC��ջ
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
//13.<���ʽ> �� <��>��<��>+<���ʽ>
bool SynAnalysis::Is_Expression(int &a)
{

    if(!Is_Term(a)) return 0;
    if(s[a+1]=="+") //<��>+<���ʽ>
    {
        a++;
        ////////////////////////////////////////////
        if(!tempop.empty())
        {
        if(tempop.top()=="+")//����һ���ַ�Ϊ"+"�Ҳ�����ջ��top������Ϊ"+"ʱ��ǰ���������������
        {
            id num1,num2;
            num2=tempnum.top();//arg2��ջ
            tempnum.pop();
            num1=tempnum.top();//arg1��ջ
            tempnum.pop();
            tempop.pop();//"+"��ջ
            temp[++tempcount]=num1.key+num2.key;
            itoa(tempcount,s1,10);
            string result="T"+string(s1);//������Ԫʽ��result
            tempnum.push(id(result,temp[tempcount]));
            string arg1,arg2;//������Ԫʽ��arg1��arg2
            if(num1.name=="0")//num1Ϊ����
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2Ϊ����
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            arg2=num2.name;
            Quadruple.push_back(quad("+",arg1,arg2,result));
        }
        }
        tempop.push("+");//��"+"��ջ
        ////////////////////////////////////////////
        if(!Is_Expression(a)) return 0;
        ///////////////////////////////////////////
        if(tempop.top()=="+")
        {
            id num1,num2;
            num2=tempnum.top();//arg2��ջ
            tempnum.pop();
            num1=tempnum.top();//arg1��ջ
            tempnum.pop();
            tempop.pop();//"+"��ջ
            temp[++tempcount]=num1.key+num2.key;
            itoa(tempcount,s1,10);
            string result="T"+string(s1);//������Ԫʽ��result
            tempnum.push(id(result,temp[tempcount]));
            string arg1,arg2;//������Ԫʽ��arg1��arg2
            if(num1.name=="0")//num1Ϊ����
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2Ϊ����
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            else arg2=num2.name;
            Quadruple.push_back(quad("+",arg1,arg2,result));
        }
        ///////////////////////////////////////////
        return 1;//<��>+<���ʽ>
    }
    return 1;//<��>
}
//14.<��> �� <����>��<����>*<����>
bool SynAnalysis::Is_Term(int &a)
{
    if(!Is_Factor(a)) return 0;
    ///////////////////////////////////////
    if(s[a+1]=="*")
    {
        a++;if(a>s.size()) return 0;
        tempop.push(s[a]);//"*"��ջ
        if(!Is_Factor(a)) return 0;
        //////////////////////////////////////////////////
        //�����м����Tn=arg1*arg2
        id num1,num2;
        num2=tempnum.top();//arg2��ջ
        tempnum.pop();
        num1=tempnum.top();//arg1��ջ
        tempnum.pop();
        tempop.pop();//"*"��ջ
        temp[++tempcount]=num1.key*num2.key;
        itoa(tempcount,s1,10);
        string result="T"+string(s1);//������Ԫʽ��result
        tempnum.push(id(result,temp[tempcount]));
        string arg1,arg2;//������Ԫʽ��arg1��arg2
        if(num1.name=="0")//num1Ϊ����
        {
            itoa(num1.key,s1,10);
            arg1=string(s1);
        }
        else arg1=num1.name;
        if(num2.name=="0")//num2Ϊ����
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
//15.<����> �� (<���ʽ>) | <��ʶ��> | <����>
bool SynAnalysis::Is_Factor(int &a)
{
    if(s[a+1]=="(")//(<���ʽ>)
    {
        tempop.push("(");//"("��ջ
        a++;if(a>s.size()) return 0;
        if(!Is_Expression(a)) return 0;
        a++;if(a>s.size()) return 0;
        if(s[a]!=")") {error=a;return 0;}
        tempop.pop();//"("��ջ
        return 1;
    }

    int b=a;
    if(Is_Identifier(b)) {a=b;return 1;}//<��ʶ��>

    b=a;
    if(Is_C(b)) {a=b;return 1;}//<����>
    error=a;
    return 0;
}
//16.<�������ʽ> �� <��ϵ���ʽ> | ��<��ϵ���ʽ>
//                  | !(<�������ʽ>)| !��<�������ʽ>��&&<�������ʽ>|!��<�������ʽ>��||<�������ʽ>
//                  |<��ϵ���ʽ>&&<�������ʽ>|<��ϵ���ʽ>||<�������ʽ>
//                  |��<�������ʽ>��|��<�������ʽ>��&&<�������ʽ>|��<�������ʽ>��||<�������ʽ>
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
            if((s[a+1]=="&"&&s[a+2]=="&"))// !��<�������ʽ>��&&<�������ʽ>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //TC��ջ
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
            else if((s[a+1]=="|"&&s[a+2]=="|"))// !��<�������ʽ>��||<�������ʽ>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //FC��ջ
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
            return 1;// !(<�������ʽ>)
        }
    }
    if(s[a+1]=="!")//��<��ϵ���ʽ>
    {
        a++;
        if(!Is_RelationExpression(a)) return 0;
        //TC��ջ
        if(TC.top()==0) TC.top()=Quadruple.size()-1;
        else
        {
            itoa(TC.top(),s1,10);
            Quadruple[Quadruple.size()-2].result=s1;
            TC.top()=Quadruple.size()-1;
        }
        //FC��ջ
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
    if(s[a+1]=="(")//��<�������ʽ>��
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
            if((s[a+1]=="&"&&s[a+2]=="&"))//��<�������ʽ>��&&<�������ʽ>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //TC��ջ
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
            else if((s[a+1]=="|"&&s[a+2]=="|"))//��<�������ʽ>��||<�������ʽ>
            {
                a=a+2;
                //////////////////////////////////////////////////
                //FC��ջ
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
            else return 1;//��<�������ʽ>��
        }
        ////////////////////////////////////////////
        return 0;
    }
    if(Is_RelationExpression(a))
    {
        //TC��ջ
        if(TC.top()==0) TC.top()=Quadruple.size()-1;
        else
        {
            itoa(TC.top(),s1,10);
            Quadruple[Quadruple.size()-2].result=s1;
            TC.top()=Quadruple.size()-1;
        }
        //FC��ջ
        if(FC.top()==0) {FC.top()=Quadruple.size();}
        else
        {
            itoa(FC.top(),s1,10);
            Quadruple[Quadruple.size()-1].result=s1;
            FC.top()=Quadruple.size();
        }
        if((s[a+1]=="&"&&s[a+2]=="&"))//<��ϵ���ʽ> && <�������ʽ>
        {
            a=a+2;
            //////////////////////////////////////////////////
            //TC��ջ
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
        else if((s[a+1]=="|"&&s[a+2]=="|"))//<��ϵ���ʽ>||<�������ʽ>
        {
            a=a+2;
            //////////////////////////////////////////////////
            //FC��ջ
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
        else return 1;//<��ϵ���ʽ>
    }
    /////////////////////////////////////////////////
    return 0;
}
//17.< ��ϵ���ʽ> �� <���ʽ><��ϵ><���ʽ>|<���ʽ>|��ʶ��
bool SynAnalysis::Is_RelationExpression(int &a)
{
    int b=a;
    if(!Is_Expression(b))
    {
        if(Is_Identifier(a))//��ʶ��
        {
            Quadruple.push_back(quad("jnz",s[a]," ","0"));//��ϵ���ʽΪ1
            Quadruple.push_back(quad("j"," "," ","0"));//��ϵ���ʽΪ0
            return 1;
        }
    }
    else
    {
        a=b;
        if(!Is_Relation(b))//<���ʽ>
        {
            id num1;
            num1=tempnum.top();//arg1��ջ
            tempnum.pop();
            string arg1;//������Ԫʽ��arg1
            if(num1.name=="0")//num1Ϊ����
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            Quadruple.push_back(quad("jnz",arg1," ","0"));//��ϵ���ʽΪ1
            Quadruple.push_back(quad("j"," "," ","0"));//��ϵ���ʽΪ0
            return 1;
        }
        else//<���ʽ><��ϵ><���ʽ>
        {
            a=b;
            if(!Is_Expression(a)) return 0;
            //////////////////////////////////////
            id num1,num2;
            num2=tempnum.top();//arg2��ջ
            tempnum.pop();
            num1=tempnum.top();//arg1��ջ
            tempnum.pop();
            string op="j"+tempop.top();//��ϵ����ջ
            tempop.pop();
            string arg1,arg2;//������Ԫʽ��arg1��arg2
            if(num1.name=="0")//num1Ϊ����
            {
                itoa(num1.key,s1,10);
                arg1=string(s1);
            }
            else arg1=num1.name;
            if(num2.name=="0")//num2Ϊ����
            {
                itoa(num2.key,s1,10);
                arg2=string(s1);
            }
            else arg2=num2.name;
            Quadruple.push_back(quad(op,arg1,arg2,"0"));//��ϵ���ʽΪ1
            Quadruple.push_back(quad("j"," "," ","0"));//��ϵ���ʽΪ0
            return 1;//<���ʽ><��ϵ><���ʽ>
        }
    }
}
//18. <��ϵ> �� <��|��<=��|��>��|��>=��|��=��|��<>
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
    /*              �ʷ�����                */
    //////////////////////////////////////////
    la.GetBinaryPattern();
     ///////////////////////////////////////////
    /*              �﷨����                */
    //////////////////////////////////////////
    C.clear();//������
    s.clear();
    tempcount=0;
    in.open("��Ԫʽ.txt");
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
        /*              �������                */
        //////////////////////////////////////////
        //�����һ����������ת,ʹ��(j,,,)->(j,,,)����
        for(int i=0;i!=Quadruple.size();++i)
        {
            stack<int>jump;
            if(Quadruple[i].op=="j")
            {
                int goton=atoi(Quadruple[i].result.c_str());
                int temp=i+1;//��¼��һ����ת����

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
