#include <iostream>

#include "cpp-formula.h"

using namespace std;
using namespace fizvlad;

int main()
{
    string str = "group1:subscribers & !(user1:friends | user1:subscribers)";
    cout << "Formula: " << str << endl;

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");
    Formula::Action a_and(2, R"RAW_LITERAL(([^\s]+) \& ([^\s]+))RAW_LITERAL");
    Formula::Action a_or(2, R"RAW_LITERAL(([^\s]+) \| ([^\s]+))RAW_LITERAL");
    Formula::Action a_not(1, R"RAW_LITERAL(\!([^\s]+))RAW_LITERAL");

    Formula::Actions actions = {a_brackets, a_not, a_and, a_or};

    Formula f(str, actions);
    Formula::Sequence sequence = f.getSequence();

    for (Formula::Step s : sequence) {
        cout << s << endl << "    Action id: " << f.getActionIndex(s.action) << endl;
    }
    return 0;
}

