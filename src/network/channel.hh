/* 
 * @brief This class abstract the connection between two node,
 *        it could be either a server or a client.
 *
 *        The strange thing in here is the handler type, it is 
 *        basically a generic function pointer. 
 *        It is used in the on_read/write methods as the last parameter
 *        (call-back function).
 *        Commented lines point out for future refactor.
 *
 */
#pragma once

#include <boost/asio.hpp>
#include <string>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;

//template <typename T>
//using handler<T> = void(T::*)(const boost::system::error_code&, size_t);
  
class Channel {
  public:
    Channel (boost::asio::io_service&, std::string s, int port);
    virtual ~Channel () { };

    virtual bool establish () = 0;
    virtual void close () = 0;

//    template <typename class_name = Channel>
//    virtual void on_read (std::string&, size_t, handler<class_name>) = 0;
//
//    template <typename class_name = Channel>
//    virtual void on_write (std::string&, size_t, handler<class_name>) = 0;

    tcp::socket* get_socket();

  protected:
    std::string endpoint;
    int port;
    boost::asio::io_service& io_service;
    std::unique_ptr<tcp::socket> socket;
};

} /* network */ 
}
