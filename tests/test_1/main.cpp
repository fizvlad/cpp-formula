#include <iostream>

#include "cpp-formula.h"

using namespace std;

int main()
{
    string str = "obj1 + obj2 * obj3 - !(obj1 && obj2) + 1";
    cout << "Formula: " << str << endl;
    return 0;
}
