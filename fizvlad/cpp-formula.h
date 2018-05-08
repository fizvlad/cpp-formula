#ifndef CPP_FORMULA_H_INCLUDED
#define CPP_FORMULA_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <regex>
#include <iostream>
#include <exception>

namespace fizvlad {

    /// Formula parser using regex.
    struct Formula {
    public:

        /// Container for operator characteristics.
        struct Action {
            /// Amount of operands
            size_t operandsAmount;

            /// Regex matching this action if all the actions with heigher priority are already initialized. Amount of matching groups must be equal to operandsAmount
            /// Notice: "${aliasPrefix}${id}" will be placed into the initial string please make sure your regexp won't recognise it as operator
            std::regex regexp;


            Action(size_t o = 0, std::string r = "") : operandsAmount(o), regexp(r, std::regex_constants::ECMAScript), regexp_str_(r) {
                regexpCheck_();
            }


            std::string getStr() const {
                return regexp_str_;
            }


        private:
            std::string regexp_str_;

            bool regexpIsCorrect_() {
                return regexp.mark_count() == operandsAmount;
            }
            void regexpCheck_() {
                if (!regexpIsCorrect_()) {
                    throw std::invalid_argument("Number of marked sub-expressions (" + std::to_string(regexp.mark_count()) + ") within the regular expression doesn't match with stated amount of operands (" + std::to_string(operandsAmount) + ")");
                }
            }
        };


        /// Returns index of given action in actions.
        /// Notice throws error on failure.
        size_t getActionIndex(Action action) throw(std::invalid_argument);


        /// Array of operations in order of priority decrease.
        /// First action will be applied first.
        typedef std::vector<Action> Actions;


        enum OperandType {String, Alias};

        /// Container with operand. Operand can be a str which is not matching any regexp or alias with aliasIndex.
        struct Operand {
            /// Type of operand
            OperandType type;

            std::string str;
            size_t aliasIndex;

            Operand(std::string s = "") : type(String), str(s), aliasIndex(0) {}
            Operand(size_t i) : type(Alias), str(""), aliasIndex(i) {}
        };


        /// Array of operands
        typedef std::vector<Operand> Dependencies;

        /// Set of unique operands
        typedef std::set<Operand> Dependencies_set;


        /// Get array of strings of dependencies
        Dependencies_set getExternalDependencies();


        /// Container for information on each sequence step.
        struct Step {
            /// Step id. Unique for single Sequence
            size_t id;

            /// Matching action
            Action action;

            /// Ids of operands of this Sequence
            Dependencies operands;

            Step(size_t i, Action a, Dependencies oi) : id(i), action(a), operands(oi) {}
        };


        /// Sequence of steps in proper order.
        /// First step must be done first.
        typedef std::vector<Step> Sequence;


        /// Get sequence of steps for current string.
        Sequence getSequence();


        /// Generate solution using given vector of functions.
        /// Notice: Functions must match actions.
        /// \param resultType Type of result of handlerType functions
        /// \param convertType Type of function converting string into type of arguments of handlerType
        /// \param handlerType Type of function which take array of resulType and handles action
        template <typename resultType, typename convertType, typename handlerType> resultType calculate(convertType converter, std::vector<handlerType> handlers) {
            Sequence sequence = getSequence();
            if (sequence.size() == 0) {
                return converter(str);
            }
            std::vector<resultType> stepResults(sequence.size());
            for (size_t i = 0; i < sequence.size(); i++) {
                Step s = sequence[i];
                std::vector<resultType> operands;
                operands.reserve(s.operands.size());
                for (Operand operand : s.operands) {
                    operands.push_back(operand.type == String ? converter(operand.str) : stepResults[operand.aliasIndex]);
                }
                resultType stepResult = (handlers[getActionIndex(s.action)])(operands);
                stepResults[i] = stepResult;
            }
            return stepResults[stepResults.size() - 1];
        }


        Formula(std::string s, Actions a, std::string p = "ALIAS_") : str(s), actions(a), prefix(p) {}
        Formula() = delete;


        /// Input string
        std::string str;

        /// Array of defined actions
        Actions actions;

        /// Alias prefix
        std::string prefix;


    private:

        /// Creating array of aliases for given str and returns alias equal to str.
        std::string setUpAliases_(std::string str, std::vector<std::string> *aliases);
    };

    bool operator==(const Formula::Action &l, const Formula::Action &r);

    bool operator==(const Formula::Operand &l, const Formula::Operand &r);
    bool operator<(const Formula::Operand &l, const Formula::Operand &r);

    std::ostream &operator<<(std::ostream &out, const Formula::Step &step);
}

#endif // CPP_FORMULA_H_INCLUDED
