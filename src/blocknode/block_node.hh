#pragma once

#include "../nodes/node.hh"
#include "local_io.hh"
#include "../messages/IOoperation.hh"
#include <string>

namespace eclipse {

using vec_str = std::vector<std::string>;

class BlockNode: public Node {
  public:
    BlockNode (network::ClientHandler*);
    ~BlockNode ();

    //! @brief Save to disk a block and replicate.
    bool block_insert_local(Block&, bool replicate = true);

    //! @brief Delete Local block
    bool block_delete_local(Block&, bool replicate = true);

    //! @brief Update the content of the block.
    bool block_update_local(Block& block, uint32_t pos, uint32_t len, bool replicate = true);

    //! @brief Read block from the local node.
    bool block_read_local(Block& block, uint64_t off = 0, uint64_t len = 0, bool ignore_params = true);

  protected:
    void replicate_message(messages::IOoperation*);

    Local_io local_io;
    int network_size;
};

}
