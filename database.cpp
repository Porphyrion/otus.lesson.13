#include "database.h"

Database::Database(threadSafeQueue<std::string>& resQueue_) : resQueue(resQueue_){
    tables.emplace("A", table());
    tables.emplace("B", table());
};


void Database::insert(std::string tableName, int id, std::string name){
    std::unique_lock<std::shared_timed_mutex> lock(mut);
    tables[tableName].emplace(id, name);
};

void Database::intersection(){
    std::string response = "";
    std::shared_lock<std::shared_timed_mutex> lock(mut);
    for(auto& i : tables["A"]){
        for(auto& j : tables["B"]){
            if(i.first == j.first){
                response.append(std::to_string(i.first) + "," + i.second +
                            "," + i.second + "\n");
            }
        }
    }
    response.append("OK\n");
    resQueue.push(response);
};

void Database::symmetric_difference(){
    std::string response = "";
    std::shared_lock<std::shared_timed_mutex> lock(mut);

    auto first_A = tables["A"].begin(); auto last_A = tables["A"].end();
    auto first_B = tables["B"].begin(); auto last_B = tables["B"].end();

    while (first_A != last_A) {
        if (first_B == last_B){
            for(auto i : tables["A"])
                response.append(std::to_string(i.first)+","+i.second+",\n");
            break;
        }
        if (first_A->first < first_B->first) {
            response.append(std::to_string(first_A->first)+","+first_A->second+",\n");
            *first_A++;
        } else {
            if (first_B->first < first_A->first) {
                response.append(std::to_string(first_B->first)+","+first_B->second+",\n");
            } else {
                ++first_A;
            }
            ++first_B;
        }
    }
    while(first_B != last_B){
        response.append(std::to_string(first_B->first)+","+first_B->second+",\n");
        first_B++;
    }

    response.append("OK\n");
    resQueue.push(response);
};

void Database::truncate(std::string tableName){
    std::unique_lock<std::shared_timed_mutex> lock(mut);
    tables.erase(tableName);
    resQueue.push("OK\n");
};

bool Database::findTables(){
    std::shared_lock<std::shared_timed_mutex> lock(mut);
    return (tables.find("A") != tables.end() || tables.find("B") != tables.end());
};

bool Database::findTable(std::string tableName){
    std::shared_lock<std::shared_timed_mutex> lock(mut);
    return tables.find(tableName) != tables.end();
};

bool Database::findElement(std::string tableName, int id){
    std::shared_lock<std::shared_timed_mutex> lock(mut);
    return tables[tableName].find(id) != tables[tableName].end();
}
