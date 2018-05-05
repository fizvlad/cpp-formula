#ifndef CPP_FORMULA_H_INCLUDED
#define CPP_FORMULA_H_INCLUDED

#include <string>
#include <vector>
#include <regex>

namespace fizvlad {

    ///
    /// \brief Formula parser using regex
    ///
    class Formula {
    public:

        ///
        /// \brief Container for operator characteristics
        ///
        struct Action {
            /// Amount of operands
            size_t operandsAmount;

            /// Regex matching this action if all the actions with heigher priority are already initialized. Amount of matching groups must be equal to operandsAmount
            /// Notice: "ALIAS${id}" will be placed into the initial string please make sure your regexp won't recognise it as operator
            std::basic_regex<char> regexp;


            Action(size_t o = 0, std::string r = "") : operandsAmount(o), regexp(r, std::regex_constants::ECMAScript) {}
            Action(size_t o, std::basic_regex<char> r) : operandsAmount(o), regexp(r) {}
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
            std::vector<size_t> operandIds;

            Step(size_t i, Action a, std::vector<size_t> oi) : id(i), action(a), operandIds(oi) {}
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


        Formula(std::string str, Actions actions) : str_(str), actions_(actions) {}
        Formula() = delete;

        Sequence getSequence() {
            Sequence result;
            // TODO Applying regex with highest priority and if got enough matching operands then recursively creating formula for each one.
            return result;
        }

    private:
        std::string str_;
        Actions actions_;
    };

}

#endif // CPP_FORMULA_H_INCLUDED
