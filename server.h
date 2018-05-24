#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <deque>
#include <utility>

using boost::asio::ip::tcp;
using chat_message = std::vector<std::string>;
using command_queue = std::deque<chat_message>;
class session;

class database_room
{
    using session_ptr = std::shared_ptr<session>;

public:

    database_room(){}

    void leave(session_ptr participant){
        participants_.erase(participant);
    }

    void join(session_ptr participant){
        participants_.emplace(participant);
    }

private:
    std::set<session_ptr> participants_;

};

class session : public std::enable_shared_from_this<session>{
public:
    session(tcp::socket socket, database_room& room_)
    : socket_(std::move(socket)), room(room_)
    {
    }

    void run()
    {
        room.join(shared_from_this());
        do_read();
    }

    void do_read()
    {
       auto self(shared_from_this());
       boost::asio::async_read_until(socket_,
           sb, '\n',
           [this,self](boost::system::error_code ec, std::size_t byte)
           {
             sb.commit(byte);
             std::string line;
             std::stringstream ss;

             boost::asio::streambuf::const_buffers_type constBuffer = sb.data();


             std::copy(
                 boost::asio::buffers_begin(constBuffer),
                 boost::asio::buffers_begin(constBuffer) + byte,
                 std::ostream_iterator<char>(ss)
             );
             line = ss.str();
             sb.consume(byte);
             auto bicycle = std::all_of(line.begin(), line.end(), [](char c){return c == '\0';});

             if (!ec && !bicycle)
             {
                if(proverka(line)) do_write("OK\n");
                else do_write("ERR\n");
             }
             else{
                 if(ec.value() == 2 || bicycle) {
                     room.leave(shared_from_this());
                 }
             }
           });
     }

     void do_write(std::string answer_)
     {
         auto self(shared_from_this());
         boost::asio::async_write(socket_,
         boost::asio::buffer(answer_, 4),
         [this, self](boost::system::error_code ec, std::size_t )
         {
             if (!ec)
             {
                do_read();
             }
         });
   }

   bool proverka(std::string& msg){
       boost::tokenizer<boost::char_separator<char>> tokenizer{msg, boost::char_separator<char>{" "}};
       auto i = find(commands.begin(), commands.end(),*tokenizer.begin());
       if(i != commands.end()) return true;
       else return false;
    }


private:
    std::vector<std::string> commands{"INSERT", "TRUNCATE", "INTERSECTION", "SYMMETRIC_DIFFERENCE"};
    tcp::socket socket_;
    boost::asio::streambuf sb;
    database_room& room;
};


class server
{
public:
    server(boost::asio::io_service& io_service,
            const tcp::endpoint& endpoint)
    : acceptor_(io_service, endpoint),
      socket_(io_service),room()
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
            [this](boost::system::error_code ec)
            {
            if (!ec)
            {
                std::make_shared<session>(std::move(socket_), room)->run();
            }
            do_accept();
        });
    }
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    database_room room;
};
