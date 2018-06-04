#pragma once

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <shared_mutex>
#include "threadsafe_queue.h"

class Database{
    using table = std::map<int, std::string>;
    using table_iter = std::map<int, std::string>::iterator;

public:
    Database(threadSafeQueue<std::string>& resQueue_);

    bool findTables();
    bool findTable(std::string tableName);
    bool findElement(std::string tableName, int id);

    void insert(std::string tableName, int id, std::string name);
    void truncate(std::string tableName);
    void intersection();
    void symmetric_difference();

private:

    threadSafeQueue<std::string>& resQueue;
    mutable std::shared_timed_mutex mut;
    std::map<std::string, table> tables;
};
