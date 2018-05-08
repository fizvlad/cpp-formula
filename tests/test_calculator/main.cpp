#include <iostream>
#include <functional>

#include "cpp-formula.h"

using namespace std;
using namespace fizvlad;

int main()
{
    string str = "";

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");
    Formula::Action a_plus(2,     R"RAW_LITERAL(([^\s+*\/\-]+)\s*\+\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_minus(2,    R"RAW_LITERAL(([^\s+*\/\-]+)\s*\-\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_multiply(2, R"RAW_LITERAL(([^\s+*\/\-]+)\s*\*\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_divide(2,   R"RAW_LITERAL(([^\s+*\/\-]+)\s*\/\s*([^\s+*\/\-]+))RAW_LITERAL");

    Formula::Actions actions = {a_brackets, a_multiply, a_divide, a_plus, a_minus};

    Formula f(str, actions);

    vector<function<float(vector<float>)>> func;

    func.push_back([](vector<float> input){
        return input[0];
    });
    func.push_back([](vector<float> input){
        return input[0] * input[1];
    });
    func.push_back([](vector<float> input){
        return input[0] / input[1];
    });
    func.push_back([](vector<float> input){
        return input[0] + input[1];
    });
    func.push_back([](vector<float> input){
        return input[0] - input[1];
    });

    function<float(string)> converter = [](string str){return stof(str);};

    std::string input;
    cout << " < ";
    getline(cin, input);
    while (input != "") {
        f.str = input;
        try {
            float solution = f.calculate<float>(converter, func);
            cout << " > " << solution;
        } catch (std::exception e) {
            cout << " ! Error occured: " << e.what();
        }
        cout << endl << endl;

        cout << " < ";
        getline(cin, input);
    }
    return 0;
}
