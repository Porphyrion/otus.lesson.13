#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

class TableManager{
    using table = std::map<int, std::string>;
public:
    TableManager();

    std::string parsing(std::string& msg);

    std::string insert(std::vector<std::string>);
    std::string truncate(std::vector<std::string>);
    std::string intersection();
    std::string symmetric_difference();


private:
    std::vector<std::string> commands{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
    std::map<std::string, table> tables;
};
