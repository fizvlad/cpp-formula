#include <iostream>
#include <functional>
#include <cmath>

#include "cpp-formula.h"

using namespace std;
using namespace fizvlad;

int main()
{
    string str = "";

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");

    Formula::Action a_power(2,    R"RAW_LITERAL(([^\s+*\/\-]+)\s*\^\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_ln(1,       R"RAW_LITERAL(ln\s*([^\s+*\/\-]+))RAW_LITERAL");

    Formula::Action a_multiply(2, R"RAW_LITERAL(([^\s+*\/\-]+)\s*\*\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_divide(2,   R"RAW_LITERAL(([^\s+*\/\-]+)\s*\/\s*([^\s+*\/\-]+))RAW_LITERAL");

    Formula::Action a_plus(2,     R"RAW_LITERAL(([^\s+*\/\-]+)\s*\+\s*([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_minus(2,    R"RAW_LITERAL(([^\s+*\/\-]+)\s*\-\s*([^\s+*\/\-]+))RAW_LITERAL");

    Formula::Actions actions = {a_brackets, a_power, a_ln, a_multiply, a_divide, a_plus, a_minus};

    Formula f(str, actions);

    vector<function<float(vector<float>)>> func;

    func.push_back([](vector<float> input){
        return input[0];
    });
    func.push_back([](vector<float> input){
        return std::pow(input[0], input[1]);
    });
    func.push_back([](vector<float> input){
        return std::log(input[0]);
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

    function<float(string)> converter = [](string str){
        if (str == "E" || str == "e") {
            return (float) M_E;
        } else if (str == "PI" || str == "pi") {
            return (float) M_PI;
        }
        return stof(str);
    };

    std::string input;
    cout << " < ";
    getline(cin, input);
    while (input != "") {
        f.str = input;
        try {
            float solution = f.calculate<float>(converter, func);
            cout << " > " << solution;
        } catch (std::runtime_error e) {
            cout << " ! Runtime error occurred: " << e.what();
        } catch (std::exception e) {
            cout << " ! Error occurred: " << e.what();
        }
        cout << endl << endl;

        cout << " < ";
        getline(cin, input);
    }
    return 0;
}
