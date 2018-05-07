# cpp-formula

C++ tool to form sequence of calculations steps from formula in text form using regular expressions.


## Example

All the functions and classes are placed in namespace `fizvlad`:

    using namespace fizvlad

Defining all used actions with regexp. Algorithm will gradually replace actions with their aliases.  
Every regexp must save groups which will be treated as operands of this action.  
If operand will be a parsable formula itself it will be processed.

**Notice**: Regexp `\(([^\(\)]+)\)` match only constructions with no brackets inside.

    Formula::Action a_brackets(1, R"RAW_LITERAL(\(([^\(\)]+)\))RAW_LITERAL");  
    Formula::Action a_plus(2,     R"RAW_LITERAL(([^\s+*\/\-]+)\s?\+\s?([^\s+*\/\-]+))RAW_LITERAL");  
    Formula::Action a_minus(2,    R"RAW_LITERAL(([^\s+*\/\-]+)\s?\-\s?([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_multiply(2, R"RAW_LITERAL(([^\s+*\/\-]+)\s?\*\s?([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Action a_divide(2,   R"RAW_LITERAL(([^\s+*\/\-]+)\s?\/\s?([^\s+*\/\-]+))RAW_LITERAL");
    Formula::Actions actions = {a_brackets, a_multiply, a_divide, a_plus, a_minus};

Small example of how it should work:  
`(4 - 2 * (1 + 1))` → `(4 - 2 * (ALIAS_0))` → `(4 - 2 * ALIAS_1)` → `(4 - ALIAS_2)` → `(ALIAS_3)` → `ALIAS_4`

Defining `Formula` object:

    Formula f(str, actions);

Receiving sequence of steps from `Formula` object and outputing info on each step:

    Formula::Sequence sequence = f.getSequence();
    // Formula::Sequence = std::vector<Formula::Step>

    for (Formula::Step s : sequence) {
        cout << s << endl << "    Action id: " << f.getActionIndex(s.action) << endl;
    }

Each `Step` stores its id, action and array of dependencies.  
Each dependency may be an unparsable string or id of another step.

### Evaluating

Defining handlers for actions:

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

Defining converter which will transform string to type of handlers input:

    function<float(string)> converter = [](string str){return stof(str);};

Now we can find answer using method of `Formula`:

    float solution = f.calculate<float>(converter, func);
