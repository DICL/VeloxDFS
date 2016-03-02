#include "message.hh"

using namespace std;

namespace eclipse {
namespace messages {

Message::Message(int o, int d) : origin(o), destination(d) { }

int  Message::get_origin()      const { return origin; }
int  Message::get_destination() const { return destination; }

Message& Message::set_origin(int o)      { origin = o; return *this; }
Message& Message::set_destination(int d) { destination = d; return *this; }

}
}
