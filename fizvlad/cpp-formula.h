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


        enum OperandType {String, Alias};

        ///
        /// \brief Container with operand. Operand can be a str which is not matching any regexp or alias with aliasId
        ///
        struct Operand {
            /// Type of operand
            OperandType type = String;

            std::string str = "0";
            size_t aliasId = 0;
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


        Formula(std::string str, Actions actions, std::string aliasPrefix = "ALIAS_") : str_(str), actions_(actions), aliasPrefix_(aliasPrefix) {}
        Formula() = delete;


        Sequence getSequence() {
            Sequence result;
            std::vector<std::string> aliases; // Array to store aliases
            std::string s = setUpAliases_(str_, &aliases); // Parsing formula into array of aliases. s is an alias of whole formula

            for (size_t i = 0; i < aliases.size(); i++) {
                //std::cout << aliasPrefix_ + std::to_string(i) << " = " << aliases[i] << std::endl; // LOG
                Action action;
                std::vector<Operand> operands;
                for (Action a : actions_) {
                    // Checking alias for each action
                    std::smatch m;
                    std::regex_search(aliases[i], m, a.regexp);
                    if (m.size() != 0) {
                        // Found action for given alias
                        // TODO Probably it is possible to get all this information from setUpAliases_()
                        action = a;
                        for (size_t i = 1; i < m.size(); i++) {
                            Operand o;
                            std::string m_s = m[i];
                            if (m_s.find(aliasPrefix_) != std::string::npos) {
                                // This is an alias
                                o.type = Alias;
                                o.aliasId = std::stoi(m_s.substr(aliasPrefix_.size()));
                            } else {
                                o.type = String;
                                o.str = m_s;
                            }
                            operands.push_back(o);
                        }
                        break;
                    }
                }
                result.push_back(Step(i, action, operands));
            }
            return result;
        }


    private:
        std::string str_;
        Actions actions_;
        std::string aliasPrefix_;


        /// Creating array of aliases for given str and returns alias equal to str
        std::string setUpAliases_(std::string str, std::vector<std::string> *aliases) {
            for (size_t i = 0; i < actions_.size(); i++) {
                // For each action:
                Action action = actions_[i];
                while (true) {
                    // While we can still apply current action
                    std::smatch match;
                    std::regex_search(str, match, action.regexp);
                    if (match.size() == 0) {
                        // Stop if can not apply this action
                        break;
                    }

                    std::string str_copy = str; // Copying initial string to place new aliases there
                    for (size_t j = 1; j < match.size(); j++) {
                        // Recursively processing each operand
                        std::string target = match[j].str();
                        std::string sub = setUpAliases_(target, aliases);
                        str_copy.replace(str_copy.find(target), target.size(), sub);
                    }

                    str = str_copy; // Now will be using string where all parsed operand replaced with aliases
                    std::regex_search(str, match, action.regexp);
                    if (match.size() == 0) {
                        throw std::runtime_error("Was not able to find action in str: " + str);
                    }
                    std::string target = match[0].str(); // Saving target string
                    size_t aliasIndex = std::find(aliases->cbegin(), aliases->cend(), target) - aliases->cbegin();
                    // If this target already been met somewhere index will be correctly set
                    // Index = aliases.size() otherwise
                    str.replace(str.find(target), target.size(), aliasPrefix_ + std::to_string(aliasIndex));
                    // Replaced found operation with alias
                    if (aliasIndex == aliases->size()) {
                        // Saving new alias
                        aliases->push_back(target);
                    }
                }
            }
            return str;
        }
    };


    std::ostream &operator<<(std::ostream &out, const Formula::Step &step) {
        out << "Step #" << step.id << ". Depends on: ";
        for (Formula::Operand o : step.operands) {
            switch (o.type) {
                case Formula::String:
                    out << o.str << " (String)";
                    break;
                case Formula::Alias:
                    out << "step #"<< o.aliasId;
                    break;
            }
            out << "; ";
        }
        return out;
    }
}

#endif // CPP_FORMULA_H_INCLUDED
