#pragma once

#include "model/metadata.hh"

#include <string>
#include <vector>
#include <stdint.h>

namespace velox {

using vec_str = std::vector<std::string>;

class DFS {
  public:
    DFS();

    //! Write the contents into a remote file.
    //!
    //! @param file_name  File to peform the operation 
    //! if the file does not exists it will create it.
    //! @param buf        contents to be copied.
    //! @param off        offset to append the buffer.
    //! @param len        size of the content.
    //! @retval >0        bytes successfully uploaded.
    //! @retval 0         The operation failed.
    uint64_t write(std::string& file_name, const char* buf, uint64_t off, uint64_t len);

    //! Download a remote file's section into a buffer.
    //!
    //! @pre The file must exist.
    //! @param file_name   File to peform the operation.
    //! @param[out] buffer Destination buffer.
    //! @param off         offset from where to read.
    //! @param len         size of segment to read.
    //! @retval >0         bytes successfully uploaded.
    //! @retval 0          The operation failed.
    uint64_t read(std::string& file_name, char* buf, uint64_t off, uint64_t len);

    //! Load all the file into a string.
    //!
    //! @param file_name   File to peform the operation.
    //! @retval ""         The file did not exists.
    std::string read_all(std::string file_name);

    //! Append a string to the end of a remote file.
    //! @attention It is currently not working due to 
    //! changes in the internal API.
    int append(std::string file_name, std::string buf);

    //! Upload a local file to a the veloxdfs intance.
    //!
    //! @param file_name   File to peform the operation.
    //! @param is_binary   Whether to broadcast the file to all the nodes.
    //! @retval 0          SUCCESS
    //! @retval 1          FAILURE 
    int upload(std::string file_name, bool is_binary);

    //! Download a remote file to your current local directory.
    //!
    //! @param file_name   File to peform the operation.
    //! @retval 0          SUCCESS
    //! @retval 1          FAILURE 
    int download(std::string file_name);

    bool exists(std::string);

    bool touch(std::string);

    int remove(std::string);

    int format();

    model::metadata get_metadata(std::string& fname);

    model::metadata get_metadata_optimized(std::string& fname);

    std::vector<model::metadata> get_metadata_all();

    //! @attention char out is dynamically allocated, needs to be freed after 
    //! being used.
    int read_block(model::metadata& md, std::string block_name, char* out);

    void file_metadata_append(std::string, size_t, model::metadata&);

    //! @deprecated
    int pget(vec_str);

    //! @deprecated
    int update(vec_str);

  private:
    uint64_t BLOCK_SIZE;
    uint32_t NUM_NODES;
    int replica;
    std::vector<std::string> nodes;
};

}
