#ifndef CPP_FORMULA_H_INCLUDED
#define CPP_FORMULA_H_INCLUDED

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <exception>

namespace fizvlad {

    ///
    /// \brief Formula parser using regex
    ///
    struct Formula {
    public:

        ///
        /// \brief Container for operator characteristics
        ///
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


        enum OperandType {String, Alias};

        ///
        /// \brief Container with operand. Operand can be a str which is not matching any regexp or alias with aliasIndex
        ///
        struct Operand {
            /// Type of operand
            OperandType type = String;

            std::string str = "0";
            size_t aliasIndex = 0;
        };


        ///
        /// \brief Container for information on each sequence step
        ///
        struct Step {
            /// Step id. Unique for single Sequence
            size_t id;

            /// Matching action
            Action action;

            /// Ids of operands of this Sequence
            std::vector<Operand> operands;

            Step(size_t i, Action a, std::vector<Operand> oi) : id(i), action(a), operands(oi) {}
        };


        ///
        /// \brief Sequence of steps in proper order
        ///
        /// First step must be done first
        typedef std::vector<Step> Sequence;


        ///
        /// \brief Array of operations in order of priority decrease
        ///
        /// First action will be applied first
        typedef std::vector<Action> Actions;


        Formula(std::string s, Actions a, std::string p = "ALIAS_") : str(s), actions(a), prefix(p) {}
        Formula() = delete;


        ///
        /// \brief Get sequence of steps for current string
        ///
        Sequence getSequence();


        ///
        /// \brief Returns index of given action in actions
        ///
        /// Notice throws error on failure
        size_t getActionIndex(Action action) throw(std::invalid_argument);


        ///
        /// \brief Generate solution using given vector of functions
        ///
        /// Notice: Functions must match actions
        ///
        /// \param resultType Type of result of handlerType functions
        /// \param convertType Type of function converting string into type of arguments of handlerType
        /// \param handlerType Type of function which take array of resulType and handles action
        template <typename resultType, typename convertType, typename handlerType> resultType calculate(convertType converter, std::vector<handlerType> handlers) {
            Sequence sequence = getSequence();
            if (sequence.size() == 0) {
                throw std::runtime_error("Empty sequence");
            }
            std::vector<resultType> stepResults(sequence.size());
            for (size_t i = 0; i < sequence.size(); i++) {
                Step s = sequence[i];
                std::vector<resultType> operands(s.operands.size());
                for (size_t j = 0; j < s.operands.size(); j++) {
                    Operand operand = s.operands[j];
                    operands[j] = operand.type == String ? converter(operand.str) : stepResults[operand.aliasIndex];
                }
                resultType stepResult = (handlers[getActionIndex(s.action)])(operands);
                stepResults[i] = stepResult;
            }
            return stepResults[stepResults.size() - 1];
        }


        std::string str;
        Actions actions;
        std::string prefix;


    private:

        /// Creating array of aliases for given str and returns alias equal to str
        std::string setUpAliases_(std::string str, std::vector<std::string> *aliases);
    };

    bool operator==(const Formula::Action &l, const Formula::Action &r);

    std::ostream &operator<<(std::ostream &out, const Formula::Step &step);
}

#endif // CPP_FORMULA_H_INCLUDED
