#include <common/dl_loader.hh>
#include <stdexcept>

SUITE(DL) {
  TEST(BASIC) {
    CHECK_THROW(DL_loader* ld = new DL_loader("myfile", "myfunc"), 
        std::runtime_error);

    //ld->run();
  }
}
