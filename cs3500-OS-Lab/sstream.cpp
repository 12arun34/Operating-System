#include<iostream>
#include<sstream>
#include<string>
using namespace std;
int main()
{
    string sentence;
    // we need to find the number of words in sentence
    // cin>>sentence;
    getline(cin,sentence);
    stringstream s(sentence);
    string word;
    int count=0;
    while(s >> word)
    {
     count++;
    }
    //  return count;
    cout<<count<<endl;

    return 0;

}