#include "blockrequest.hh"


using namespace eclipse::messages;
namespace ph = std::placeholders;

std::string BlockRequest::get_type() const { return "BlockRequest"; }

void BlockRequest::on_exec(std::string v, std::string k, message_fun f) {
  Block bi;
  bi.block_name = k;
  bi.content = v;
  f(&bi);
}

void BlockRequest::exec(PeerDFS* p, message_fun f) {
  p->request(hash_key, block_name, std::bind(&BlockRequest::on_exec, this, 
        ph::_1, ph::_2, f));
}
