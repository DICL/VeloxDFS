//
// This module implements the serialization using BOOST SERIALIZAITON
// For every new message class that you want to add you must add its 
// functions here and its cc file.
//
#pragma once

//! 1) Add your class here
// common
#include "message.hh"
#include "boundaries.hh"
#include "keyvalue.hh"
#include "control.hh"
#include "keyrequest.hh"
#include "task.hh"
#include "reply.hh"
#include "cacheinfo.hh"
#include "formatrequest.hh"
#include "checkexist.hh"
#include "offsetkv.hh"
#include "metadata.hh"

// file
#include "fileinfo.hh"
#include "filelist.hh"
#include "filerequest.hh"
#include "filedescription.hh"
#include "filedel.hh"
#include "fileupdate.hh"

// block
#include "blockinfo.hh"
#include "blocklist.hh"
#include "blockrequest.hh"
#include "blockdel.hh"
#include "blockupdate.hh"

// slice
#include "sliceinfo.hh"
#include "slicelist.hh"
#include "slicerequest.hh"
#include "slicedel.hh"

// mdlist
#include "mdlistinfo.hh"
#include "mdlistlist.hh"
#include "mdlistrequest.hh"
#include "mdlistdel.hh"
#include "mdlistupdate.hh"

// node
#include "nodeinfo.hh"
#include "nodelist.hh"
#include "noderequest.hh"
#include "nodedel.hh"
#include "nodeupdate.hh"
#include "nodedescription.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif

//! 2) Also here
namespace eclipse {
namespace messages {

// common
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Message& m, uint32_t) {
    ar & BOOST_SERIALIZATION_NVP(m.origin);
    ar & BOOST_SERIALIZATION_NVP(m.destination);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Boundaries& b, uint32_t) {
    ar & BASE_OBJECT(Message, b);
    ar & BOOST_SERIALIZATION_NVP(b.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyValue& k, uint32_t) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::OffsetKeyValue& k, uint32_t) {
    ar & BASE_OBJECT(Message, k);
    ar & BOOST_SERIALIZATION_NVP(k.key);
    ar & BOOST_SERIALIZATION_NVP(k.name);
    ar & BOOST_SERIALIZATION_NVP(k.value);
    ar & BOOST_SERIALIZATION_NVP(k.pos);
    ar & BOOST_SERIALIZATION_NVP(k.len);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Control& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::KeyRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Task& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.id);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.library);
    ar & BOOST_SERIALIZATION_NVP(c.input_path);
    ar & BOOST_SERIALIZATION_NVP(c.func_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::Reply& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.message);
    ar & BOOST_SERIALIZATION_NVP(c.details);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::CacheInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.keys);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FormatRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::CheckExist& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MetaData& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.node);
    ar & BOOST_SERIALIZATION_NVP(c.content);
  }

// file
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileList& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);
    ar & BOOST_SERIALIZATION_NVP(c.net_id);
    ar & BOOST_SERIALIZATION_NVP(c.block_size);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileUpdate& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
  }

// block
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name); 
    ar & BOOST_SERIALIZATION_NVP(c.hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.seq);  
    ar & BOOST_SERIALIZATION_NVP(c.size);     
    ar & BOOST_SERIALIZATION_NVP(c.net_id);     
    ar & BOOST_SERIALIZATION_NVP(c.replica);     
    ar & BOOST_SERIALIZATION_NVP(c.check_commit); 
    ar & BOOST_SERIALIZATION_NVP(c.content);  
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockList& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.net_id);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockDel& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockUpdate& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);  
    ar & BOOST_SERIALIZATION_NVP(c.net_id); 
    ar & BOOST_SERIALIZATION_NVP(c.pos);     
    ar & BOOST_SERIALIZATION_NVP(c.len);   
    ar & BOOST_SERIALIZATION_NVP(c.replica);   
    ar & BOOST_SERIALIZATION_NVP(c.content);  
  }

// slice
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::SliceInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.size);
    ar & BOOST_SERIALIZATION_NVP(c.content);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::SliceList& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::SliceRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.hash_key);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::SliceDel& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

// mdlist
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MdlistInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.slice_name);
    ar & BOOST_SERIALIZATION_NVP(c.file_pos);
    ar & BOOST_SERIALIZATION_NVP(c.slice_pos);
    ar & BOOST_SERIALIZATION_NVP(c.len);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MdlistList& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MdlistRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MdlistDel& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.slice_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::MdlistUpdate& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.slice_name);
    ar & BOOST_SERIALIZATION_NVP(c.len);
  }

// node
template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeInfo& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.id);
    ar & BOOST_SERIALIZATION_NVP(c.net_id);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeList& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.data);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeRequest& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeDel& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.type);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeUpdate& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.name);
    ar & BOOST_SERIALIZATION_NVP(c.type);
    ar & BOOST_SERIALIZATION_NVP(c.net_id);
    ar & BOOST_SERIALIZATION_NVP(c.id);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::NodeDescription& c, uint32_t) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.node_list);
  }
}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(eclipse::messages::Message);

//! 3) Also here
// common
BOOST_CLASS_TRACKING(eclipse::messages::Message, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Boundaries, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Control, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Task, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Reply, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::CacheInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FormatRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::CheckExist, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::OffsetKeyValue, boost::serialization::track_never);

// file
BOOST_CLASS_TRACKING(eclipse::messages::FileInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDescription, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDel, boost::serialization::track_never);

// block
BOOST_CLASS_TRACKING(eclipse::messages::BlockInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockUpdate, boost::serialization::track_never);

// slice
BOOST_CLASS_TRACKING(eclipse::messages::SliceInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::SliceList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::SliceRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::SliceDel, boost::serialization::track_never);

// mdlist
BOOST_CLASS_TRACKING(eclipse::messages::MdlistInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::MdlistList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::MdlistRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::MdlistDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::MdlistUpdate, boost::serialization::track_never);

// node
BOOST_CLASS_TRACKING(eclipse::messages::NodeInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::NodeList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::NodeRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::NodeDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::NodeUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::NodeDescription, boost::serialization::track_never);
