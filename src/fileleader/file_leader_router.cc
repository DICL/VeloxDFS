#include "file_leader_router.hh"
#include "../common/context_singleton.hh"
#include "../messages/boost_impl.hh"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
namespace ph = std::placeholders;

// Constructor {{{
FileLeaderRouter::FileLeaderRouter(FileLeader* fl, Router* router): RouterDecorator(router) {
  file_leader = fl;

  using namespace std::placeholders;
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto& rt = routing_table;
  rt.insert({"FileInfo",   bind(&FileLeaderRouter::insert_file, this, _1, _2)});
  rt.insert({"FileUpdate",   bind(&FileLeaderRouter::update_file, this, _1, _2)});
  rt.insert({"FileRequest", bind(&FileLeaderRouter::request_file, this, _1, _2)});
  rt.insert({"FileList", bind(&FileLeaderRouter::request_ls, this, _1, _2)});
  rt.insert({"FileDel", bind(&FileLeaderRouter::delete_file, this, _1, _2)});
  rt.insert({"FileExist", bind(&FileLeaderRouter::file_exist, this, _1, _2)});
  rt.insert({"MetaData", bind(&FileLeaderRouter::replicate_metadata, this, _1, _2)});
  rt.insert({"FormatRequest", bind(&FileLeaderRouter::request_format, this, _1, _2)});
}
// }}}
// FileInfo* {{{
void FileLeaderRouter::insert_file (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileInfo*> (m_);
  INFO("FileInfo received");

  if (m->uploading == 1) {
    auto reply = file_leader->file_insert(m);
    tcp_connection->do_write(reply.get());

  } else if (m->uploading == 0) {
    bool ret = file_leader->file_insert_confirm(m);
    Reply reply;

    if (ret) {
      reply.message = "TRUE";

    } else {
      reply.message = "FALSE";
    }
    tcp_connection->do_write(&reply);
  }
}
// }}}
// FileUpdate* {{{
void FileLeaderRouter::update_file (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileUpdate*> (m_);
  INFO ("FileUpdate received");

  bool ret = file_leader->file_update(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "File doesn't exist";
  }

  tcp_connection->do_write(&reply);
}
// }}}
// {{{ FileDel
void FileLeaderRouter::delete_file (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileDel*> (m_);
  INFO ("FileDel received");

  bool ret = file_leader->file_delete(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "File doesn't exist";
  }

  tcp_connection->do_write(&reply);
}
// }}}
// request_file {{{
void FileLeaderRouter::request_file (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileRequest*> (m_);
  DEBUG("FILE REQUEST RECIEVED %s", m->name.c_str());

  shared_ptr<Message> fd = file_leader->file_request(m);

  tcp_connection->do_write(fd.get());
}
// }}}
// request_ls {{{
void FileLeaderRouter::request_ls (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileList*> (m_);
  file_leader->list(m);
  tcp_connection->do_write(m);
}
// }}}
// file_exist {{{
void FileLeaderRouter::file_exist (messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::FileExist*> (m_);
  bool ret = file_leader->file_exist(m->name);
  Reply reply;

  if (ret) {
    reply.message = "TRUE";

  } else {
    reply.message = "FALSE";
  }
  tcp_connection->do_write(&reply);
}
// }}}
// replicate_metadata {{{
void FileLeaderRouter::replicate_metadata(messages::Message* m_, Channel* tcp_connection) {
  auto m = dynamic_cast<messages::MetaData*> (m_);
  file_leader->metadata_save(m);
}
// }}}
// request_format {{{
void FileLeaderRouter::request_format (messages::Message* m_, Channel* tcp_connection) {
  bool ret = file_leader->format();
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
  }

  tcp_connection->do_write(&reply);
}
// }}}
