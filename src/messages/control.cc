#include "control.hh"

namespace messages {

Control::Control(int t): type(t) { }
std::string Control::get_type() const { return "Control"; };

}
