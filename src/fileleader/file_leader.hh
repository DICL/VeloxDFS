#pragma once

#include "../nodes/node.hh"
#include "directory.hh"
#include "../blocknode/local_io.hh"
#include "../messages/fileinfo.hh"
#include "../messages/fileupdate.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/filedel.hh"
#include "../messages/fileexist.hh"
#include "../messages/filedescription.hh"
#include "../messages/metadata.hh"
#include "../common/histogram.hh"

#include <string>

namespace eclipse {

using vec_str = std::vector<std::string>;

class FileLeader: public Node {
  public:
    FileLeader(network::ClientHandler*);
    ~FileLeader();

    //! @brief Insert file metadata and compute block metadata.
    unique_ptr<messages::Message> file_insert(messages::FileInfo*);

    //! Confirm that all the blocks has been uploaded.
    bool file_insert_confirm(messages::FileInfo*);

    //! Return a description of the blocks containing the file.
    std::shared_ptr<messages::Message> file_request(messages::FileRequest*);

    bool file_delete(messages::FileDel*);

    //! Fill the param with the list of files.
    bool list(messages::FileList*);

    bool file_exist(std::string);

    //! Update the file with the new size and blocks.
    bool file_update(messages::FileUpdate*);

    void metadata_save(messages::MetaData*);

    //! Remove everything and create tables.
    bool format();

  protected:
    void replicate_metadata();
    Directory directory;
    Local_io local_io;
    std::unique_ptr<Histogram> boundaries;
    std::map<std::string, std::shared_ptr<messages::FileDescription>> current_file_arrangements;
    int network_size;

    void find_best_arrangement(messages::FileDescription* file_desc);
};

}
