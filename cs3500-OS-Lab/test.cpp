#include <iostream>
#include <sstream>
#include <string>

int main() {
    using namespace std;
    int i;
    string input;
    cin >> input; //or getline(cin, input)
    stringstream ss(input);
    if (ss >> i && ss.eof()) {  //if conversion succeeds and there's no more to get
        cout<< "\n Correct Input \n";
    }
    else {
        cout<< "\n Format Error \n";
    }

  return 0;
}