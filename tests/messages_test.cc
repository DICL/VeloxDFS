#include "messages/boost_impl.hh"
#include "messages/factory.hh"

#include <iostream>
#include <sstream>

using namespace std;
using namespace eclipse::messages;

SUITE(MESSAGES) {
  TEST(basic) {
    Message* a = new Boundaries ({1,2,3,4,5});
    Message* b = new KeyValue (1, "K","V");
    a->set_origin (1);
    a->set_destination (2);
    b->set_origin (1);
    b->set_destination (2);
    
    string a_out = save_message(a);
    string b_out = save_message(b);

    auto b_ = static_cast<Boundaries*>(load_message (a_out));
    auto k_ = static_cast<KeyValue*>(load_message (b_out));

    CHECK(b_->get_origin() == 1);
    CHECK(b_->data[0] == 1);
    CHECK(b_->data[1] == 2);
    CHECK(b_->data[2] == 3);
    CHECK(b_->data[3] == 4);
    CHECK(b_->data[4] == 5);
    CHECK(k_->get_origin() == 1);
    CHECK(k_->key == 1);
    CHECK(k_->name== "K");
    CHECK(k_->value == "V");

    delete b_;
    delete k_;
  }
}
