#include <iostream>

#include "cpp-formula.h"

using namespace std;
using namespace fizvlad;

int main()
{
    string str = "obj1 + obj2 * obj3 + obj2 * obj3 - 2 * (obj1 + obj2 * (obj4 - (obj5))) + 1";
    cout << "Formula: " << str << endl;

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");
    Formula::Action a_plus(2, R"RAW_LITERAL(([^\s]+) \+ ([^\s]+))RAW_LITERAL");
    Formula::Action a_minus(2, R"RAW_LITERAL(([^\s]+) \- ([^\s]+))RAW_LITERAL");
    Formula::Action a_multiple(2, R"RAW_LITERAL(([^\s]+) \* ([^\s]+))RAW_LITERAL");
    Formula::Action a_divide(2, R"RAW_LITERAL(([^\s]+) \/ ([^\s]+))RAW_LITERAL");

    Formula::Actions actions = {a_brackets, a_multiple, a_divide, a_plus, a_minus};

    Formula f(str, actions);
    Formula::Sequence sequence = f.getSequence();

    for (Formula::Step s : sequence) {
        size_t index = 0;
        for (; index < actions.size(); index++) {
            if (actions[index] == s.action) {
                break;
            }
        }
        cout << s << endl << "    Action id: " << index << endl;
    }
    return 0;
}
