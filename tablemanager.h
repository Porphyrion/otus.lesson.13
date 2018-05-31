#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include <thread>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "error.h"

class TableManager{
    using table = std::map<int, std::string>;
public:
    TableManager();

    std::string parsing(std::string& msg);

private:
    std::string processing(std::vector<std::string> line);
    bool findTables();

    std::string insert(std::string tableName, int id, std::string name);
    std::string truncate(std::vector<std::string>);
    std::string intersection();
    std::string symmetric_difference();

    std::vector<std::string> commands{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
    std::map<std::string, table> tables;
    Error er;
};
