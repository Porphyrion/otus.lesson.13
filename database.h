#include <map>
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

class Database{
    using table = std::map<int, std::string>;
public:
    Database(std::shared_ptr<threadsafe_queue> resQueue_);

    bool findTables();

    void insert(std::string tableName, int id, std::string name);
    void truncate(std::string tableName);
    void intersection();
    void symmetric_difference();

private:
    std::shared_ptr<threadsafe_queue> resQueue;
    mutable std::shared_timed_mutex mut;
    std::map<std::string, table> tables;
}
