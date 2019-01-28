#include "boost_impl.hh"

#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/utility.hpp>
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
  void serialize (Archive& ar, eclipse::messages::FileInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
    ar & BOOST_SERIALIZATION_NVP(c.uploading);
    ar & BOOST_SERIALIZATION_NVP(c.blocks_metadata);
    ar & BOOST_SERIALIZATION_NVP(c.n_lblock);
    ar & BOOST_SERIALIZATION_NVP(c.is_input);
    ar & BOOST_SERIALIZATION_NVP(c.intended_block_size);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileUpdate& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.blocks_metadata);
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
  void serialize (Archive& ar, eclipse::messages::FileRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.generate);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name); 
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.off);  
    ar & BOOST_SERIALIZATION_NVP(c.len);  
    ar & BOOST_SERIALIZATION_NVP(c.should_read_partially);  
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, unsigned int) {
    using eclipse::messages::FileInfo;
    ar & BASE_OBJECT(FileInfo, c);
    ar & BOOST_SERIALIZATION_NVP(c.blocks);
    ar & BOOST_SERIALIZATION_NVP(c.hash_keys);
    ar & BOOST_SERIALIZATION_NVP(c.block_size);
    ar & BOOST_SERIALIZATION_NVP(c.block_hosts);
    ar & BOOST_SERIALIZATION_NVP(c.logical_blocks);
    ar & BOOST_SERIALIZATION_NVP(c.num_static_blocks);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
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
template <typename Archive>
  void serialize (Archive& ar, eclipse::BlockMetadata& c, unsigned int) {
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
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::IOoperation& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.operation);
    ar & BOOST_SERIALIZATION_NVP(c.option);
    ar & BOOST_SERIALIZATION_NVP(c.pos);
    ar & BOOST_SERIALIZATION_NVP(c.length);
    ar & BOOST_SERIALIZATION_NVP(c.block);
    ar & BOOST_SERIALIZATION_NVP(c.block_metadata);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::logical_block_metadata& c, unsigned int) {
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.host_name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.physical_blocks);
  }

using namespace eclipse::messages;
using namespace boost::archive;

template void serialize<xml_oarchive>   (boost::archive::xml_oarchive&, Message&, unsigned);
template void serialize<xml_iarchive>   (boost::archive::xml_iarchive&,  Message&, unsigned);
template void serialize<binary_iarchive>(boost::archive::binary_iarchive&, Message&, unsigned);
template void serialize<binary_oarchive>(boost::archive::binary_oarchive&, Message&, unsigned);







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


template void serialize (boost::archive::xml_oarchive&, Reply&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  Reply&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  Reply&, unsigned);


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

template void serialize (boost::archive::xml_oarchive&, eclipse::BlockMetadata&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  eclipse::BlockMetadata&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  eclipse::BlockMetadata&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  eclipse::BlockMetadata&, unsigned);

template void serialize (boost::archive::xml_oarchive&, IOoperation&, unsigned);
template void serialize (boost::archive::xml_iarchive&,  IOoperation&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  IOoperation&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  IOoperation&, unsigned);

template void serialize (boost::archive::xml_oarchive&, eclipse::logical_block_metadata&, unsigned);
template void serialize (boost::archive::xml_iarchive&, eclipse::logical_block_metadata&, unsigned);
template void serialize (boost::archive::binary_iarchive&,  eclipse::logical_block_metadata&, unsigned);
template void serialize (boost::archive::binary_oarchive&,  eclipse::logical_block_metadata&, unsigned);
}
}


// 4) Also here
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::MetaData);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::BlockMetadata);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::messages::IOoperation);
BOOST_CLASS_EXPORT_IMPLEMENT(eclipse::logical_block_metadata);
