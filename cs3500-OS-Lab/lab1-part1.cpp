#include <bits/stdc++.h> 
using namespace std;


// Checking preference of operators(BODMAS)
bool check(char top, char curr)
{
    map<char,int> mp;
    mp['-']=1;
    mp['+']=1;
    mp['*']=2;
    mp['/']=3;
    return (mp[top]>=mp[curr]);
}



int main()
{

    string s;
    stack <float> s_num;
    stack <char> s_oper;
    while(!cin.eof())
    {
        cin>>s;
        int n=s.size();
        // flag=1 represents the previous one is operator and flag=0 represents the previous one is number.
        bool flag=0;
        // wrong=1 - wrong expression
        bool wrong=0;
        for (int i=0;i<n;i++)
        {
            if (s[i]>='0' && s[i]<='9')
            {
                if (flag==0)
                {
                    s_num.push(float(s[i]-'0'));
                }
                else
                {
                    float curr=s_num.top();
                    curr=curr*10+s[i]-'0';
                    s_num.pop();
                    s_num.push(curr);
                }
                flag=1;
            }
            else
            {
                if (flag==0)
                {
                    // Case for negative numbers.
                    if (s[i]=='-')
                    {
                        s_num.push((-1.0)*(s[i+1]-'0'));
                        i++;
                        flag=1;
                    }
                    else
                    {
                        cout<<"Wrong Expression";
                        break;
                    }
                }
                else
                {
                    if (s_oper.empty())
                    {
                        s_oper.push(s[i]);
                    }
                    else
                    {
                        // Case for operators.
                        while(s_oper.size() && check(s_oper.top(),s[i]))
                        {
                            char op=s_oper.top();
                            s_oper.pop();
                            float n2=s_num.top();
                            s_num.pop();
                            float n1=s_num.top();
                            s_num.pop();
                            switch(op)
                            {
                                case '+' : s_num.push(n1+n2); break;
                                case '-' : s_num.push(n1-n2); break;
                                case '*' : s_num.push(n1*n2); break;
                                case '/' : {if (n2==0) {wrong=1; cout<<"Division by 0 error.";} else {s_num.push(n1/n2);}} break;
                                default :  cout<<"Undefined Operator"; wrong=1; break;
                            }
                            if (wrong==1)
                                break;
                        }
                        s_oper.push(s[i]);
                    }
                    flag=0;
                }
            }
            if (wrong==1)
                break;
        }
        if (wrong==1)
            continue;
        while(s_oper.size())
        {
            char op=s_oper.top();
            s_oper.pop();
            float n2=s_num.top();
            s_num.pop();
            float n1=s_num.top();
            s_num.pop();
            switch(op)
            {
                case '+' : s_num.push(n1+n2); break;
                case '-' : s_num.push(n1-n2); break;
                case '*' : s_num.push(n1*n2); break;
                case '/' : {if (n2==0) {wrong=1; cout<<"Division by 0 error."<<endl;} else {s_num.push(n1/n2);}} break;
                default :  cout<<"Undefined Operator"; wrong=1; break;
            }
            if (wrong==1)
                break;
        }
        if (wrong==1)
            continue;
        cout<<s_num.top()<<endl;
        s_num.pop();
    }
}