#include "control.hh"

namespace eclipse {
namespace messages {

Control::Control(int t): type(t) { }
std::string Control::get_type() const { return "Control"; };

}
}
