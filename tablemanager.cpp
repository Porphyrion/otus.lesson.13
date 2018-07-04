#include "tablemanager.h"

TableManager::TableManager() :
    db(responses_queue)
{
    vt.push_back(std::thread([this](){
        std::pair<commands_type, commands_string> whatafuck;
        while(queue.wait_and_pop( whatafuck )){
            std::unique_lock<std::mutex> lock(m);
            switch(whatafuck.first){
                case insert_com:
                    db.insert( whatafuck.second[1], std::stoi(whatafuck.second[2]), whatafuck.second[3]);
                    break;
                case truncate_com:
                    db.truncate( whatafuck.second[1] );
                    break;
                case intersection_com:
                    db.intersection();
                    break;
                case symmetric_difference_com:
                    db.symmetric_difference();
                    break;
            }
        }
    }));
};

void TableManager::parsing(std::string& msg){
    commands_string tokens;
    boost::split(tokens, msg, boost::is_any_of(std::set<char>{' ', '\n'}));
    auto i = find(commands.begin(), commands.end(), *tokens.begin());
    tokens.erase(tokens.end()-1);
    if(i != commands.end())  processing(tokens);
    else responses_queue.push(er.message(Error::types::incorrect_command));
};

void TableManager::processing(commands_string line){

    auto i = line.begin();
    if(*i == commands[0]){
        if(line.size() < 4){
            responses_queue.push(er.message(Error::types::few_arguments));
        }
        else if(!db.findTable(line[1])){
             responses_queue.push(er.message(Error::types::table_not_found));
         }
        else if(db.findElement(line[1], std::stoi(line[2]))){
            responses_queue.push(er.message(Error::types::id_exist));
            cv_res.notify_one();
        }
        else {
            queue.push(std::make_pair(insert_com, line));
            responses_queue.push("OK\n");
        }
    }else if (*i == commands[1]) {
        if(line.size() < 2 ) {
            responses_queue.push(er.message(Error::types::few_arguments));
        }
        else if(!db.findTable(line[1])){
             responses_queue.push(er.message(Error::types::table_not_found));
         } else {
             db.forwardTruncate(line[1]);
             responses_queue.push("OK\n");
             queue.push(std::make_pair(truncate_com, line));
         }
    }else if (*i == commands[2]){
        if(db.findTables()){
            queue.push(std::make_pair(intersection_com, line));
        }
        else {
            responses_queue.push(er.message(Error::types::table_not_found));
        }
    } else if (*i == commands[3]) {
        if(db.findTables()) {
            queue.push(std::make_pair(symmetric_difference_com, line));
        }
        else {
            responses_queue.push(er.message(Error::types::table_not_found));
        }
    }
};
