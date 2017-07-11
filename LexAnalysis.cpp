#include"LexAnalysis.h"
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

string LexAnalysis::Reserved_word[x] =
{ "program", "var", "procedure", "begin", "end", "if", "then", "else", "while", "do", "call", "integer", "real" };
string LexAnalysis::character[y] =
{"+","-","*","/","~","&","|","<","<=",">",">=","=","<>",":=",";",".",",","(",")",":","!"};

void LexAnalysis::scanner()
{
    for(int n=0; n<40; n++) token[n]=NULL; //��ʼ���ַ���
    if(!flat) token[0]=ch;
    flat=1;
    //�Թ��ո�
    while(ch==' '||ch=='\n')
    {
        in.get(ch);
        if(ch=='\n') line++;
    }
    //�����Ǳ�ʶ�����߱�����
    if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))
    {
        m=0;//ָ���ַ����ĵ�һ����ĸ
        while(('0'<=ch&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='_')
        {
            token[m++]=ch;
            in.get(ch);
            if(ch=='\n') line++;
        }
        token[m++]='\0';
        syn=1;//��Ĭ���ַ���Ϊ��ʶ��
        flat=0;
        if(m>8) syn=-1;
        else
        {
            bool Is_Reserved_word=0;//��¼�Ƿ�Ϊ������
            for(int n=0; n<x; n++) //�ж��Ƿ�Ϊ������
            {
                if(token==Reserved_word[n])
                {
                    syn=n+3;//Ϊ������
                    Is_Reserved_word=1;
                    break;
                }
            }
            if(!Is_Reserved_word)
            {
                bool flat1=1;
                for(int i=0; i<=IDentifierSize; ++i)
                    if(token==IDentifier[i])
                    {
                        flat1=0;
                        countIDentifier=i+1;
                    }
                if(flat1)
                {
                    IDentifier[IDentifierSize++]=token;//���Ʊ�ʶ������ʶ�����Լ���
                    countIDentifier=IDentifierSize;
                }
            }
        }
    }


    else if((ch>='0'&&ch<='9'))  //����
    {
        m=0;
        while(ch>='0'&&ch<='9')
        {
            token[m++]=ch;
            in.get(ch);
            if(ch=='\n') line++;
        }
        flat=0;
        syn=2;
        if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='_')//���ֿ�ͷ�ı�ʶ��
        {
            syn=-1;
            while((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='_'))
            {
                token[m++]=ch;
                in.get(ch);
                if(ch=='\n') line++;
            }
        }
        token[m]='\0';
    }
    else switch(ch)   //�����ַ�
        {
        case'+':
            syn=16;
            token[0]=ch;
            break;
        case'-':
            syn=17;
            token[0]=ch;
            break;
        case'*':
            syn=18;
            token[0]=ch;
            break;
        case'~':
            syn=20;
            token[0]=ch;
            break;
        case'.':
            syn=31;
            token[0]=ch;
            break;
        case',':
            syn=32;
            token[0]=ch;
            break;
        case'(':
            syn=33;
            token[0]=ch;
            break;
        case')':
            syn=34;
            token[0]=ch;
            break;
        case'|':
            syn=22;
            token[0]=ch;
            break;
        case'!':
            syn=36;
            token[0]=ch;
            break;
        case'/':
            token[0]=ch;
            in.get(ch);
            if(ch=='\n') line++;
            if(ch=='*')//'/*'Ϊע�ͣ����Ժ���Ĵ�ֱ��������*/��
            {
                syn=0;
                token[1]=ch;
                char ch1,ch2;
                in.get(ch1);
                if(ch1=='\n') line++;
                in.get(ch2);
                if(ch2=='\n') line++;
                while(!(ch1=='*'&&ch2=='/'))
                {
                    ch1=ch2;
                    in.get(ch2);
                    if(ch2=='\n') line++;
                }
            }
            else if(ch=='/')//'//'Ϊע�ͣ����Ժ���Ĵ�ֱ��������\n��
            {
                syn=0;
                token[1]=ch;
                do
                {
                    in.get(ch);
                    if(ch=='\n') line++;
                }
                while(ch!='\n');
            }
            else
            {
                syn=19;
                flat=0;
            }
            break;
        case'>':
            token[0]=ch;
            in.get(ch);
            if(ch=='\n') line++;
            if(ch=='=')
            {
                syn=26;
                token[1]=ch;
            }
            else
            {
                syn=25;
                flat=0;
            }
            break;
        case'<':
            token[0]=ch;
            in.get(ch);
            if(ch=='\n') line++;
            if(ch=='=')//<=
            {
                syn=24;
                token[1]=ch;
            }
            else if(ch=='>')//<>
            {
                syn=28;
                token[1]=ch;
            }
            else
            {
                syn=23;
                flat=0;
            }
            break;
        case'=':
            syn=27;
            token[0]=ch;
            break;
        case':':
            token[0]=ch;
            in.get(ch);
            if(ch=='\n') line++;
            if(ch=='=')
            {
                syn=29;
                token[1]=ch;
            }
            else
            {
                syn=35;
                flat=0;
            }
            break;
        case';':
            syn=30;
            token[0]=ch;
            break;
        case'&':
            syn=21;
            token[0]=ch;
            break;
        }
}

void LexAnalysis::GetBinaryPattern()
{
    in.open("��Ԫʽ���ɲ���.txt");
    out.open("��Ԫʽ.txt");
    if(!in.is_open())
    {
        cout<<"�����ļ��򿪴���"<<endl;
        exit(1);
    }
    else
    {
        while(1)
        {
            if(flat)ch=in.get();//����һ���ַ�
            if(ch==EOF)
            {
                in.close();
                out.close();
                break;
            }
            if(ch=='\n') line++;
            scanner();//�ʷ�����
            //�����Ԫʽ
            if(syn==0);//ע��
            else if(syn==1) out<<"("<<syn<<","<<countIDentifier<<")"<<endl;//��ʶ��
            else if(syn==2)out<<"("<<syn<<","<<token<<")"<<endl;//������
            else if(3<=syn&&syn<=15)out<<"("<<syn<<","<<"0"<<")"<<endl;//������
            else if(16<=syn&&syn<=36)out<<"("<<syn<<","<<"0"<<")"<<endl;//�ַ�
            else if(syn==-1) out<<"line"<<line<<":"<<"error(1) error is :"<<token<<endl;
            syn=0;//����һ������ֱ�������ظ����
        }
    }
    in.close();
    out.close();
}

