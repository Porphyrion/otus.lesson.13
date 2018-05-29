#include "tablemanager.h"

TableManager::TableManager(){
    tables.emplace("A", table());
    tables.emplace("B", table());
};

std::string TableManager::parsing(std::string& msg){
    std::vector<std::string> tokens;
    boost::split(tokens, msg, boost::is_any_of(std::set<char>{' ', '\n'}));
    std::string response = "ERR\n";

    auto i = find(commands.begin(), commands.end(), *tokens.begin());
    if(i != commands.end()){
        if(*i == commands[0]){
            response = insert(tokens);
        }else if (*i == commands[1]) {
            response = truncate(tokens);
        }else if (*i == commands[2]){
            response = intersection();
        } else if (*i == commands[3]) {
            response = symmetric_difference();
        }
    }
    return response;
};

std::string TableManager::insert(std::vector<std::string> line){
    if(line.size() < 4) return "ERR\n";
    if(tables.find(line[1]) == tables.end())  return "ERR\n";

    tables[line[1]].emplace(std::stoi(line[2]), line[3]);
    return "OK\n";
};

std::string TableManager::intersection(){
    if(tables.find("A") == tables.end() || tables.find("B") == tables.end()) return "ERR\n";
    std::string response = "";
    for(auto& i : tables["A"]){
        for(auto& j : tables["B"]){
            if(i.first == j.first){
                response.append(std::to_string(i.first) + "," + i.second +
                            "," + i.second + "\n");
            }
        }
    }
    response.append("OK\n");
    return response;
};

std::string TableManager::symmetric_difference(){
    if(tables.find("A") == tables.end() || tables.find("B") == tables.end()) return "ERR\n";
    std::string response = "";
    std::vector<std::pair<int, std::string>> v_symDifference;
    auto d_first = std::back_inserter(v_symDifference);
    auto first1 = tables["A"].begin();
    auto last1 = tables["A"].end();
    auto first2 = tables["B"].begin();
    auto last2 = tables["B"].end();

    while (first1 != last1) {
        if (first2->first == last2->first)  std::copy(first1, last1, d_first);
        if (first1->first < first2->first) {
            *d_first++ = *first1++;
        } else {
            if (first2->first < first1->first) {
                *d_first++ = *first2;
            } else {
                ++first1;
            }
            ++first2;
        }
    }
    std::copy(first2, last2, d_first);

    for(auto i : v_symDifference){
        response.append(std::to_string(i.first)+","+i.second + ",\n");
    }
    response.append("OK\n");
    return response;
};

std::string TableManager::truncate(std::vector<std::string> tokens){
    std::string response;
    if(tokens.size() != 2) return "ERR\n";
    if(tables.find(tokens[1]) != tables.end()){
        tables.erase(tokens[2]);
        return "OK\n";
    } else return "ERR\n";
};
