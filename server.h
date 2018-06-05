#pragma once

#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <thread>
#include <utility>
#include <deque>
#include "tablemanager.h"


using boost::asio::ip::tcp;
using responses_queue = std::deque<std::string>;
class session;
class database_room
{
    using session_ptr = std::shared_ptr<session>;

public:
    database_room();

    void leave(session_ptr participant){
        participants.erase(participant);
    }

    void join(session_ptr participant){
        participants.emplace(participant);
    }

    void deliver(std::string& msg);

private:
    std::set<session_ptr> participants;
    TableManager tm;
    std::vector<std::thread> vt;
};

class session : public std::enable_shared_from_this<session>{
public:
    session(tcp::socket socket, database_room& room_)
    : socket_(std::move(socket)), room(room_), read_counter(0), write_counter(0)
    {}

    void run(){
        room.join(shared_from_this());
        do_read();
    }

    void deliver(const std::string& msg){
        bool write_in_progress = !responses.empty();
        responses.push_back(msg);
        if (!write_in_progress){
            do_write();
        }
    }

    void do_read(){
       auto self(shared_from_this());
       boost::asio::async_read_until(socket_,
           sb, '\n',
           [this,self](boost::system::error_code ec, std::size_t byte)
           {
             sb.commit(byte);
             std::string line;
             std::stringstream ss;

             boost::asio::streambuf::const_buffers_type constBuffer = sb.data();

             std::copy_if(
                 boost::asio::buffers_begin(constBuffer),
                 boost::asio::buffers_begin(constBuffer) + byte,
                 std::ostream_iterator<char>(ss),
                 [](char i){ return i != '\0';}
             );

             line = ss.str();
             sb.consume(byte);

             auto bicycle = std::all_of(line.begin(), line.end(), [](char c){return c == '\0';});
             if (!ec){
                 ++read_counter;
                if(!bicycle)
                    room.deliver(line);
                do_read();
            }
             else{
                if(!bicycle && line.size())
                    room.deliver(line);
                end_of_session = true;
             }
           });
     }

     void do_write(){
         auto self(shared_from_this());
         boost::asio::async_write(socket_,
         boost::asio::buffer(responses.front(), responses.front().size()),
         [this, self](boost::system::error_code ec, std::size_t ){
             if (!ec)
             {
                ++write_counter;
                responses.pop_front();
                if (!responses.empty()){
                  do_write();
                }
                if(write_counter == read_counter && end_of_session)
                    room.leave(shared_from_this());
             }
         });
     }

private:
    tcp::socket socket_;
    boost::asio::streambuf sb;
    database_room& room;
    responses_queue responses;
    int read_counter;
    int write_counter;
    bool end_of_session{false};
};


void database_room::deliver(std::string& msg){
    tm.parsing(msg);
}

database_room::database_room(){
    vt.push_back(std::thread([this](){
            std::string response;
            while(tm.responses_queue.wait_and_pop(response)){
                for(auto participant: participants)
                    participant->deliver(response);
            }
        }));
}


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
    void do_accept(){
        acceptor_.async_accept(socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec) std::make_shared<session>(std::move(socket_), room)->run();
                do_accept();
            }
        );
    }
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    database_room room;
};
