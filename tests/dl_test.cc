#include <common/dl_loader.hh>
#include <stdexcept>

SUITE(DL) {
  TEST(BASIC) {
    DL_loader* ld = nullptr;
    CHECK_THROW(ld = new DL_loader("myfile", "myfunc"), 
        std::runtime_error);

    //ld->run();
  }
}
