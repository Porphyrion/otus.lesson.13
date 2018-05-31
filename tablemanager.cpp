#include "tablemanager.h"

TableManager::TableManager(){
    tables.emplace("A", table());
    tables.emplace("B", table());
};

std::string TableManager::parsing(std::string& msg){
    std::vector<std::string> tokens;
    boost::split(tokens, msg, boost::is_any_of(std::set<char>{' ', '\n'}));
    auto i = find(commands.begin(), commands.end(), *tokens.begin());
    tokens.erase(tokens.end()-1);
    if(i != commands.end())  return processing(tokens);
    else return er.message(Error::types::incorrect_command);
};

std::string TableManager::processing(std::vector<std::string> line){
    auto i = line.begin();
    std::string response;
    if(*i == commands[0]){
        if(line.size() < 4) return er.message(Error::types::few_arguments);
        if(tables.find(line[1]) == tables.end())  return er.message(Error::types::table_not_found);
        response = insert(line[1], std::stoi(line[2]), line[3]);
    }else if (*i == commands[1]) {
        if(line.size() < 2 ) return er.message(Error::types::few_arguments);
        if(tables.find(line[1]) == tables.end())  return er.message(Error::types::table_not_found);
        response = truncate(line[1]);
    }else if (*i == commands[2]){
        if(findTables()) response = intersection();
        else response = er.message(Error::types::table_not_found);
    } else if (*i == commands[3]) {
        if(findTables()) response = symmetric_difference();
        else response = er.message(Error::types::table_not_found);
    }
    return response;
};

std::string TableManager::insert(std::string tableName, int id, std::string name){
    tables[tableName].emplace(id, name);
    return "OK\n";
};

std::string TableManager::intersection(){
    std::string response;
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
    std::string response;
    std::vector<std::pair<int, std::string>> v_symDifference;
    auto d_first = std::back_inserter(v_symDifference);
    auto first_A = tables["A"].begin();
    auto last_A = tables["A"].end();
    auto first_B = tables["B"].begin();
    auto last_B = tables["B"].end();

    while (first_A != last_A) {
        if (first_B == last_B)  std::copy(first_A, last_A, d_first);
        if (first_A->first < first_B->first) {
            *d_first++ = *first_A++;
        } else {
            if (first_B->first < first_A->first) {
                *d_first++ = *first_B;
            } else {
                ++first_A;
            }
            ++first_B;
        }
    }
    std::copy(first_B, last_B, d_first);

    for(auto i : v_symDifference){
        response.append(std::to_string(i.first)+","+i.second + ",\n");
    }
    response.append("OK\n");
    return response;
};

std::string TableManager::truncate(std::vector<std::string> tokens){
    if(tokens.size() != 2) return er.message(Error::types::few_arguments);
    if(tables.find(tokens[1]) != tables.end()){
        tables.erase(tokens[2]);
        return "OK\n";
    } else return er.message(Error::types::table_not_found);
};

bool TableManager::findTables(){
    if(tables.find("A") == tables.end() || tables.find("B") == tables.end()) return false;
    else return true;
};
