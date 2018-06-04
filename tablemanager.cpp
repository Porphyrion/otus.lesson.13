#include "tablemanager.h"

TableManager::TableManager(){
    tables.emplace("A", table());
    tables.emplace("B", table());
    vt.push_back(std::thread([this](){
        std::pair<commands_type, std::vector<std::string>> whatafuck;
        while(queue.wait_and_pop( whatafuck )){
            switch(whatafuck.first){
                case insert_com:
                    insert( whatafuck.second[1], std::stoi(whatafuck.second[2]), whatafuck.second[3]);
                    break;
                case truncate_com:
                    truncate( whatafuck.second[1] );
                    break;
                case intersection_com:
                    intersection();
                    break;
                case symmetric_difference_com:
                    symmetric_difference();
                    break;
            }
        }
    }));
};

void TableManager::parsing(std::string& msg){
    std::vector<std::string> tokens;
    boost::split(tokens, msg, boost::is_any_of(std::set<char>{' ', '\n'}));
    auto i = find(commands.begin(), commands.end(), *tokens.begin());
    tokens.erase(tokens.end()-1);
    if(i != commands.end())  processing(tokens);
    else responses_queue.push( er.message(Error::types::incorrect_command));
};

void TableManager::processing(std::vector<std::string> line){
    auto i = line.begin();
    if(*i == commands[0]){
        if(line.size() < 4){
            responses_queue.push(er.message(Error::types::few_arguments));
            cv_res.notify_one();
        }
        else if(tables.find(line[1]) == tables.end()){
             responses_queue.push(er.message(Error::types::table_not_found));
             cv_res.notify_one();
         }
        else if(tables[line[1]].find(std::stoi(line[2])) != tables[line[1]].end()){
            responses_queue.push(er.message(Error::types::id_exist));
            cv_res.notify_one();
        }
        else {
            queue.push(std::make_pair(insert_com, line));
            responses_queue.push(er.message(Error::types::id_exist));
            cv_com.notify_one();
            cv_res.notify_one();
        }
    }else if (*i == commands[1]) {
        if(line.size() < 2 ) {
            responses_queue.push(er.message(Error::types::few_arguments));
            cv_res.notify_one();
        }
        else if(tables.find(line[1]) == tables.end()){
             responses_queue.push(er.message(Error::types::table_not_found));
             cv_res.notify_one();
         } else {
             queue.push(std::make_pair(truncate_com, line));
             cv_com.notify_one();
         }
    }else if (*i == commands[2]){
        if(findTables()){
            queue.push(std::make_pair(intersection_com, line));
            cv_com.notify_one();
        }
        else {
            responses_queue.push(er.message(Error::types::table_not_found));
            cv_res.notify_one();
        }
    } else if (*i == commands[3]) {
        if(findTables()) {
            queue.push(std::make_pair(symmetric_difference_com, line));
            cv_com.notify_one();
        }
        else {
            responses_queue.push(er.message(Error::types::table_not_found));
            cv_res.notify_one();
        }
    }
};

void TableManager::insert(std::string tableName, int id, std::string name){
    std::unique_lock<std::shared_timed_mutex> lock(mut);
    tables[tableName].emplace(id, name);
    responses_queue.push("OK\n");
};

void TableManager::intersection(){
    std::shared_lock<std::shared_timed_mutex> lock(mut);
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
    responses_queue.push(response);
};

void TableManager::symmetric_difference(){
    std::shared_lock<std::shared_timed_mutex> lock(mut);
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
    responses_queue.push(response);
};

void TableManager::truncate(std::string tableName){
        std::unique_lock<std::shared_timed_mutex> lock(mut);
        tables.erase(tableName);
        responses_queue.push("OK\n");
};

bool TableManager::findTables(){
    if(tables.find("A") == tables.end() || tables.find("B") == tables.end()) return false;
    else return true;
};
