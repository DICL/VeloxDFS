#include "block_node_router.hh"
#include "../common/context_singleton.hh"
#include "../messages/boost_impl.hh"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
namespace ph = std::placeholders;

// Constructor {{{
BlockNodeRouter::BlockNodeRouter (BlockNode* b_node, Router* router) : RouterDecorator(router) {
  block_node = b_node;

  using namespace std::placeholders;
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto& rt = routing_table;
  rt.insert({"IOoperation", bind(&BlockNodeRouter::io_operation, this, _1, _2)});
}
// }}}
// io_operation {{{
void BlockNodeRouter::io_operation (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::IOoperation*> (m_);

  if (m->operation == "BLOCK_INSERT") {
    auto ret = block_node->block_insert_local(m->block);
    Reply reply;

    if (ret) {
      reply.message = "TRUE";

    } else {
      reply.message = "FALSE";
    }
    tcp_connection->do_write(&reply);

  } else if (m->operation == "BLOCK_INSERT_REPLICA") {
    block_node->block_insert_local(m->block, false);

  } else if (m->operation == "BLOCK_DELETE") {
    auto ret = block_node->block_delete_local(m->block);
    Reply reply;

    if (ret) {
      reply.message = "TRUE";

    } else {
      reply.message = "FALSE";
    }
    tcp_connection->do_write(&reply);
  
  } else if (m->operation == "BLOCK_DELETE_REPLICA") {
    block_node->block_delete_local(m->block, false);

  } else if (m->operation == "BLOCK_REQUEST") {
    block_node->block_read_local(m->block);
    IOoperation io_ops;
    io_ops.operation = "BLOCK_TRANSFER"; 
    io_ops.block = move(m->block);
    tcp_connection->do_write(&io_ops);

  } else if (m->operation == "BLOCK_UPDATE") {
    auto ret = block_node->block_update_local(m->block, m->pos, m->length);
    Reply reply;

    if (ret) {
      reply.message = "TRUE";

    } else {
      reply.message = "FALSE";
    }
    tcp_connection->do_write(&reply);
  
  } else if (m->operation == "BLOCK_UPDATE_REPLICA") {
    block_node->block_update_local(m->block, m->pos, m->length, false);
  }
}
// }}}
