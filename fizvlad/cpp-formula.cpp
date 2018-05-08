#include "cpp-formula.h"

namespace fizvlad {

    Formula::Sequence Formula::getSequence() {
        Formula::Sequence result;
        std::vector<std::string> aliases; // Array to store aliases
        std::string s = setUpAliases_(str, &aliases); // Parsing formula into array of aliases. s is an alias of whole formula

        for (size_t i = 0; i < aliases.size(); i++) {
            //std::cout << prefix + std::to_string(i) << " = " << aliases[i] << std::endl; // LOG
            Formula::Action action;
            Dependencies operands;
            for (Formula::Action a : actions) {
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
                        if (m_s.find(prefix) != std::string::npos) {
                            // This is an alias
                            o.type = Alias;
                            o.aliasIndex = std::stoi(m_s.substr(prefix.size()));
                        } else {
                            o.type = String;
                            o.str = m_s;
                        }
                        operands.push_back(o);
                    }
                    break;
                }
            }
            result.push_back(Formula::Step(i, action, operands));
        }
        return result;
    }


    size_t Formula::getActionIndex(Formula::Action action) throw(std::invalid_argument) {
        for (size_t i = 0; i < actions.size(); i++) {
            if (action == (Formula::Action) actions[i]) {
                return i;
            }
        }
        throw std::invalid_argument("No such action in formula");
    }


    std::string Formula::setUpAliases_(std::string str, std::vector<std::string> *aliases) {
        for (size_t i = 0; i < actions.size(); i++) {
            // For each action:
            Formula::Action action = actions[i];
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
                str.replace(str.find(target), target.size(), prefix + std::to_string(aliasIndex));
                // Replaced found operation with alias
                if (aliasIndex == aliases->size()) {
                    // Saving new alias
                    aliases->push_back(target);
                }
            }
        }
        return str;
    }


    Formula::Dependencies_set Formula::getExternalDependencies() {
        Formula::Dependencies_set result;
        Formula::Sequence sequence = getSequence();
        for (Formula::Step step : sequence) {
            for (Formula::Operand operand : step.operands) {
                if (operand.type == Formula::String && std::find(result.cbegin(), result.cend(),operand) == result.cend()) {
                    result.insert(operand);
                }
            }
        }
        return result;
    }


    bool operator==(const Formula::Action &l, const Formula::Action &r) {
        return l.getStr() == r.getStr();
    }


    bool operator==(const Formula::Operand &l, const Formula::Operand &r) {
        return l.type == r.type && ((l.type == Formula::String && l.str == r.str) || (l.type == Formula::Alias && l.aliasIndex == r.aliasIndex));
    }

    bool operator<(const Formula::Operand &l, const Formula::Operand &r) {
        //TODO finish
        if (l.type == r.type) {
            return (l.type == Formula::String && l.str < r.str) || (l.type == Formula::Alias && l.aliasIndex < r.aliasIndex);
        } else {
            return l.type == Formula::String;
        }
    }


    std::ostream &operator<<(std::ostream &out, const Formula::Step &step) {
        out << "Step #" << step.id << ". Depends on: ";
        for (Formula::Operand o : step.operands) {
            switch (o.type) {
                case Formula::String:
                    out << o.str << " (String)";
                    break;
                case Formula::Alias:
                    out << "step #"<< o.aliasIndex;
                    break;
            }
            out << "; ";
        }
        return out;
    }

}
