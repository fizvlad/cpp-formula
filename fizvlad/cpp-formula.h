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
    class Formula {
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


            Action(size_t o = 0, std::string r = "") : operandsAmount(o), regexp(r, std::regex_constants::ECMAScript) {
                regexpCheck_();
            }
            Action(size_t o, std::regex r) : operandsAmount(o), regexp(r) {
                regexpCheck_();
            }
        private:
            bool regexpIsCorrect_() {
                return regexp.mark_count() == operandsAmount;
            }
            void regexpCheck_() {
                if (!regexpIsCorrect_()) {
                    throw std::invalid_argument("Number of marked sub-expressions (" + std::to_string(regexp.mark_count()) + ") within the regular expression doesn't match with stated amount of operands (" + std::to_string(operandsAmount) + ")");
                }
            }
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


        Formula(std::string str, Actions actions, std::string aliasPrefix = "ALIAS_") : str_(str), actions_(actions), aliasPrefix_(aliasPrefix) {}
        Formula() = delete;


        Sequence getSequence() {
            Sequence result;
            std::vector<std::string> aliases; // Array to store aliases
            setUpAliases_(str_, &aliases);
            for (size_t i = 0; i < aliases.size(); i++) {
                std::cout << aliasPrefix_ + std::to_string(i) << " = " << aliases[i] << std::endl;
            }
            return result;
        }


    private:
        std::string str_;
        Actions actions_;
        std::string aliasPrefix_;


        void setUpAliases_(std::string str, std::vector<std::string> *aliases) {
            for (size_t i = 0; i < actions_.size(); i++) {
                // For each action:
                Action action = actions_[i];
                while (true) {
                    // While we can still apply current action
                    std::smatch match;
                    std::regex_search(str, match, action.regexp);
                    size_t matchesAmount = match.size();
                    if (matchesAmount == 0) {
                        // Stop if can not apply this action
                        break;
                    }

                    for (size_t j = 1; j < match.size(); j++) {
                        // Recursively processing each operand
                        std::string sub = match[j].str();
                        setUpAliases_(sub, aliases);
                    }

                    std::string target = match[0]; // Saving target string
                    size_t aliasIndex = std::find(aliases->cbegin(), aliases->cend(), target) - aliases->cbegin();
                    // If this target already been met somewhere index will be correctly set
                    // Index = aliases.size() otherwise
                    str.replace(str.find(target), target.length(), aliasPrefix_ + std::to_string(aliasIndex));
                    if (aliasIndex == aliases->size()) {
                        // Saving new alias
                        aliases->push_back(target);
                    }
                }
            }
        }
    };

}

#endif // CPP_FORMULA_H_INCLUDED
