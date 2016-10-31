#include "boost_impl.hh"

#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using eclipse::messages::Message;

namespace boost {
namespace serialization {

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Message& m, unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(m.origin);
    ar & BOOST_SERIALIZATION_NVP(m.destination);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Boundaries& b, unsigned int) {
    ar & BASE_OBJECT(Message, b);
    ar & BOOST_SERIALIZATION_NVP(b.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyValue& k, unsigned int) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::OffsetKeyValue& k, unsigned int) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
    ar & BOOST_SERIALIZATION_NVP(k.pos);
    ar & BOOST_SERIALIZATION_NVP(k.len);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Control& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileUpdate& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name); 
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.seq);  
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.size);     
    ar & BOOST_SERIALIZATION_NVP(c.type);   
    ar & BOOST_SERIALIZATION_NVP(c.replica);   
    ar & BOOST_SERIALIZATION_NVP(c.node);        
    ar & BOOST_SERIALIZATION_NVP(c.l_node);      
    ar & BOOST_SERIALIZATION_NVP(c.r_node);      
    ar & BOOST_SERIALIZATION_NVP(c.is_committed);  
    ar & BOOST_SERIALIZATION_NVP(c.content);  
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockUpdate& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);  
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.seq);  
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.size);     
    ar & BOOST_SERIALIZATION_NVP(c.replica);   
    ar & BOOST_SERIALIZATION_NVP(c.content);  
    ar & BOOST_SERIALIZATION_NVP(c.pos);     
    ar & BOOST_SERIALIZATION_NVP(c.len);   
    ar & BOOST_SERIALIZATION_NVP(c.is_header);   
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Task& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.id);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.input_path);
    ar & BOOST_SERIALIZATION_NVP(c.func_name);
  }
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileList& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Reply& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.message);
    ar & BOOST_SERIALIZATION_NVP(c.details);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::CacheInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.keys);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.hash_keys);
    ar & BOOST_SERIALIZATION_NVP(c.block_size);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.seq);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FormatRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileExist& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MetaData& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.node);
    ar & BOOST_SERIALIZATION_NVP(c.content);
  }

using namespace eclipse::messages;
using namespace boost::archive;

template void serialize<xml_oarchive>   (boost::archive::xml_oarchive&, Message&, unsigned);
template void serialize<xml_iarchive>   (boost::archive::xml_iarchive&,  Message&, unsigned);
template void serialize<binary_iarchive>(boost::archive::binary_iarchive&, Message&, unsigned);
template void serialize<binary_oarchive>(boost::archive::binary_oarchive&, Message&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Boundaries&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Boundaries&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Boundaries&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Boundaries&, unsigned);

template void serialize (boost::archive::xml_oarchive&, KeyValue&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  KeyValue&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  KeyValue&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  KeyValue&, unsigned);

template void serialize (boost::archive::xml_oarchive&, OffsetKeyValue&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  OffsetKeyValue&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  OffsetKeyValue&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  OffsetKeyValue&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Control&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Control&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Control&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Control&, unsigned);

template void serialize (boost::archive::xml_oarchive&, KeyRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  KeyRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  KeyRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  KeyRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Task&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Task&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Task&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileUpdate&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileUpdate&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileUpdate&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileUpdate&, unsigned);

template void serialize<xml_oarchive>   (boost::archive::xml_oarchive&, FileList&, unsigned);
template void serialize<xml_iarchive>   (boost::archive::xml_iarchive&,  FileList&, unsigned);
template void serialize<binary_iarchive>(boost::archive::binary_iarchive&,  FileList&, unsigned);
template void serialize<binary_oarchive>(boost::archive::binary_oarchive&,  FileList&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockUpdate&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockUpdate&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockUpdate&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockUpdate&, unsigned);

template void serialize (boost::archive::xml_oarchive&, Reply&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Reply&, unsigned);

template void serialize (boost::archive::xml_oarchive&, CacheInfo&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  CacheInfo&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  CacheInfo&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  CacheInfo&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileDescription&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileDescription&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileDescription&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileDescription&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileDel&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileDel&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileDel&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileDel&, unsigned);

template void serialize (boost::archive::xml_oarchive&, BlockDel&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  BlockDel&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  BlockDel&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  BlockDel&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FormatRequest&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FormatRequest&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FormatRequest&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FormatRequest&, unsigned);

template void serialize (boost::archive::xml_oarchive&, FileExist&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  FileExist&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  FileExist&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  FileExist&, unsigned);

template void serialize (boost::archive::xml_oarchive&, MetaData&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  MetaData&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  MetaData&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  MetaData&, unsigned);

}
}


//! 4) Also here
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Boundaries);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::KeyValue);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::OffsetKeyValue);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Control);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::KeyRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Task);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockUpdate);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::CacheInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockDel);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::MetaData);
