#include <iostream>

#include "cpp-formula.h"

using namespace std;
using namespace fizvlad;

int main()
{
    string str = "obj1 + obj2 * obj3 - -(obj1 + obj2 * (obj4 - (obj5))) + 1";
    cout << "Formula: " << str << endl;

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");
    Formula::Action a_plus(1, R"RAW_LITERAL((.+)\+(.+))RAW_LITERAL");

    Formula f(str, {a_brackets, a_plus});
    Formula::Sequence s = f.getSequence();

    return 0;
}
