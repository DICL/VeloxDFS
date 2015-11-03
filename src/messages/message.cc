#include "message.hh"

using namespace std;

namespace network {
  Message::Message(string o, string d) :
    origin(o), destination(d) { }

  string Message::get_type()        const { return "Message"; }
  string Message::get_origin()      const { return origin; }
  string Message::get_destination() const { return destination; }

  Message& Message::set_origin(string o)      { origin = o; return *this; }
  Message& Message::set_destination(string d) { destination = d; return *this; }
}
