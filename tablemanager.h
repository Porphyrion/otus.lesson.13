#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "threadsafe_queue.h"
#include "error.h"

class TableManager{
    using table = std::map<int, std::string>;
public:
    TableManager();

    void parsing(std::string& msg);

    void operator()();

private:
    void processing(std::vector<std::string> line);
    bool findTables();

    void insert(std::string tableName, int id, std::string name);
    void truncate(std::string tableName);
    void intersection();
    void symmetric_difference();


    enum commands_type{
        insert_com,
        truncate_com,
        intersection_com,
        symmetric_difference_com
    };

    std::condition_variable cv_com;
    std::condition_variable cv_res;
    std::vector<std::string> commands{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
    std::map<std::string, table> tables;
    Error er;
    mutable std::shared_timed_mutex mut;
    std::vector<std::thread> vt;

public:
    threadSafeQueue<std::string> responses_queue{cv_res};
    threadSafeQueue<std::pair<commands_type, std::vector<std::string>>> queue{cv_com};
};
