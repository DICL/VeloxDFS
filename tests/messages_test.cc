#include "messages/boost_impl.hh"

#include <iostream>
#include <sstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace std;
using namespace network;
using namespace boost::archive;

stringstream ss;

void send_message (Message* in, stringstream& s) {
  xml_oarchive oa(s, no_header);
  string type = in->get_type();
  oa << BOOST_SERIALIZATION_NVP(in);
}

Message* load_message(stringstream& s) {
  xml_iarchive ia(s, no_header);
  Message* p = nullptr;
  ia >> BOOST_SERIALIZATION_NVP(p);
  return p;
}

SUITE(MESSAGES) {
  TEST(basic) {
    Message* a = new Boundaries ({1,2,3,4,5});
    Message* b = new KeyValue ("K","V");
    a->set_origin ("myself");
    a->set_destination ("you");
    b->set_origin ("myself");
    b->set_destination ("you");
    
    send_message (a, ss);
    send_message (b, ss);

    auto b_ = static_cast<Boundaries*>(load_message (ss));
    auto k_ = static_cast<KeyValue*>(load_message (ss));
    cout << "SD: " << CONTROL << endl;

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

int main () {
  return UnitTest::RunAllTests();
}
