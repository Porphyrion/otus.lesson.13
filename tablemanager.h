#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "threadsafe_queue.h"
#include "error.h"
#include "database.h"

class TableManager{

using commands_string =  std::vector<std::string>;

private:
    enum commands_type{
        insert_com,
        truncate_com,
        intersection_com,
        symmetric_difference_com
    };

    void processing(commands_string line);
    bool check();
    bool check(commands_string &cs);

    Error er;
    std::condition_variable cv_com;
    std::condition_variable cv_res;
    commands_string commands{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
    std::vector<std::thread> vt;
    std::mutex m;

public:
    TableManager();

    void parsing(std::string& msg);

    threadSafeQueue<std::string> responses_queue{cv_res};
    threadSafeQueue<std::pair<commands_type, commands_string>> queue{cv_com};
    Database db;
};
