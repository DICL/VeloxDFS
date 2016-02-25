#include <common/dl_loader.hh>
#include <stdexcept>

SUITE(DL) {
  TEST(BASIC) {
    DL_loader ld("myfile");
    CHECK_THROW(ld.init_lib(), std::runtime_error);
  }
}
