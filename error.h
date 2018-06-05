#pragma once

#include <string>

class Error{
public:
    enum types{
        incorrect_command,
        few_arguments,
        table_not_found,
        id_exist
    };

    std::string message(types er){
        switch(er){
            case incorrect_command:
                return "ERR Incorrect command, try: INSERT, TRUNCATE, INTERSECTION, SYMMETRIC_DIFFERENCE\n";
            case few_arguments:
                return "ERR A few arguments, try again\n";
            case table_not_found:
                return "ERR Table not found, try again\n";
            case id_exist:
                return "ERR Id already exist\n";
            default:
                return "ERR\n";
        }
    };
};
