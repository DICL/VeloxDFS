#include "messages/boost_impl.hh"
#include "messages/factory.hh"

#include <iostream>
#include <sstream>

using namespace std;
using namespace eclipse::messages;

SUITE(MESSAGES) {
  TEST(basic) {
    //Message* a = new Message();
    //Message* b = new Message();
    //a->set_origin (1);
    //a->set_destination (2);
    //b->set_origin (1);
    //b->set_destination (2);
    //
    //string* a_out = save_message(a);
    ////string* b_out = save_message(b);
    //boost::asio::streambuf a_buf, b_buf;
    //ostream os (&a_buf);
    //os << std::noskipws << *a_out;
    //os.seekp(0, ios::beg);
    //a_buf.commit(a_out.size());
    //cout << a_out.size() << endl;
    //cout << a_buf.size() << endl;
    //cout << &a_buf << endl;
    //Message* b_ = load_message (a_buf);
    //os.rdbuf(&b_buf);
    //os << b_out;
    //b_buf.commit(a_out.size());
    //auto k_ = static_cast<KeyValue*>(load_message (b_buf));

    //CHECK(b_->get_origin() == 1);
    //CHECK(b_->data[0] == 1);
    //CHECK(b_->data[1] == 2);
    //CHECK(b_->data[2] == 3);
    //CHECK(b_->data[3] == 4);
    //CHECK(b_->data[4] == 5);
    //CHECK(k_->get_origin() == 1);
    //CHECK(k_->key == 1);
    //CHECK(k_->name== "K");
    //CHECK(k_->value == "V");

    //delete b_;
    //delete k_;
  }
}
