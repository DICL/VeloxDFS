#pragma once
#include "mr_traits.hh"

namespace eclipse {

using boost::system::error_code;

class Executor: public MR_traits {
  public:
    Executor (Settings&);
    ~Executor ();

    void action (boost::asio::ip::tcp::socket*) override;

  protected:
    //template <typename T> void process_message (T);
    boost::asio::streambuf inbound_data;
    char inbound_header [16];
    const int header_size = 16;

    void do_read();
    void on_read_header (const error_code&, size_t); 
    void on_read_body (const error_code&, size_t); 
};

} /* eclipse  */ 
