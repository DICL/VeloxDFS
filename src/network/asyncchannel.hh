#pragma once
#include "channel.hh"
#include "../messages/message.hh"
#include "netobserver.hh"

#include <string>
#include <queue>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/error.hpp>
#include <memory>
#include <mutex>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
//! @brief Represent an opened channel between two endpoints.
//!
//! @attention It's intended to be used as a base class.
//! @attention Calling the constructor is not enought, 
//! you should also call init function.  
class AsyncChannel: public Channel, public std::enable_shared_from_this<AsyncChannel> {
  public:
    //! @param node_ Observer object
    AsyncChannel(NetObserver* node_);
    virtual ~AsyncChannel() ;

    //! @brief Write asynchronously the message.
    void do_write(messages::Message*) override; 

    //! @brief Write asynchronously the message sharing the payload.
    void do_write(std::shared_ptr<std::string>&); 

    //! @brief Write asynchronously the message sharing the payload.
    void do_write_buffer(); 

    //! @brief This method implements the reading loop.
    void do_read();

    //! @brief Get internal socket.
    tcp::socket& get_socket();

    //! @brief Add a message to the sending queue.
    void commit(std::shared_ptr<std::string>&);

  private:
    void on_write(const boost::system::error_code&, size_t); 
    void do_write_impl(); 

    void read_coroutine(boost::asio::yield_context);

    NetObserver* node = nullptr;
    tcp::socket socket; 
    std::queue<std::shared_ptr<std::string>> messages_queue;
    std::atomic<bool> is_writing;
    boost::asio::io_service& iosvc;

    std::mutex queue_mutex;

    std::string host;
    uint32_t port;
};

}
}
