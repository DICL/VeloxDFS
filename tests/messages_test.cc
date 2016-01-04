#include "messages/boost_impl.hh"
#include "messages/factory.hh"

#include <iostream>
#include <sstream>

using namespace std;
using namespace eclipse::messages;

SUITE(MESSAGES) {
  TEST(basic) {
    Message* a = new Boundaries ({1,2,3,4,5});
    Message* b = new KeyValue ("K","V");
    a->set_origin ("myself");
    a->set_destination ("you");
    b->set_origin ("myself");
    b->set_destination ("you");
    
    string a_out = save_message(a);
    string b_out = save_message(b);

    auto b_ = static_cast<Boundaries*>(load_message (a_out));
    auto k_ = static_cast<KeyValue*>(load_message (b_out));

    CHECK(b_->get_origin() == "myself");
    CHECK(b_->data[0] == 1);
    CHECK(b_->data[1] == 2);
    CHECK(b_->data[2] == 3);
    CHECK(b_->data[3] == 4);
    CHECK(b_->data[4] == 5);
    CHECK(k_->get_origin() == "myself");
    CHECK(k_->key == "K");
    CHECK(k_->value == "V");

    delete b_;
    delete k_;
  }
}
