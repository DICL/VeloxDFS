//
// This module implements the serialization using BOOST SERIALIZAITON
// For every new message class that you want to add you must add its 
// functions here and its cc file.
//
#pragma once

//! 1) Add your class here
#include "message.hh"
#include "boundaries.hh"
#include "keyvalue.hh"
#include "control.hh"
#include "keyrequest.hh"
#include "task.hh"
#include "fileinfo.hh"
#include "filelist.hh"
#include "blockinfo.hh"
#include "reply.hh"
#include "cacheinfo.hh"
#include "filerequest.hh"
#include "blockrequest.hh"
#include "filedescription.hh"
#include "blockdel.hh"
#include "filedel.hh"
#include "formatrequest.hh"

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
    ar & BOOST_SERIALIZATION_NVP(k.value);
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
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.file_hash_key);
    ar & BOOST_SERIALIZATION_NVP(c.file_size);
    ar & BOOST_SERIALIZATION_NVP(c.num_block);
    ar & BOOST_SERIALIZATION_NVP(c.replica);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockInfo& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);  
    ar & BOOST_SERIALIZATION_NVP(c.file_name);  
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);  
    ar & BOOST_SERIALIZATION_NVP(c.block_hash_key); 
    ar & BOOST_SERIALIZATION_NVP(c.block_size);     
    ar & BOOST_SERIALIZATION_NVP(c.is_inter);   
    ar & BOOST_SERIALIZATION_NVP(c.node);        
    ar & BOOST_SERIALIZATION_NVP(c.l_node);      
    ar & BOOST_SERIALIZATION_NVP(c.r_node);      
    ar & BOOST_SERIALIZATION_NVP(c.is_commit);  
    ar & BOOST_SERIALIZATION_NVP(c.content);  
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
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDescription& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.nodes);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FileDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::BlockDel& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
    ar & BOOST_SERIALIZATION_NVP(c.file_name);
    ar & BOOST_SERIALIZATION_NVP(c.block_seq);
    ar & BOOST_SERIALIZATION_NVP(c.block_name);
  }

template <typename Archive>
  void serialize (Archive& ar, eclipse::messages::FormatRequest& c, unsigned int) {
    ar & BASE_OBJECT(Message, c);
  }
}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(eclipse::messages::Message);

//! 3) Also here
BOOST_CLASS_TRACKING(eclipse::messages::Message, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Boundaries, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Control, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Task, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Reply, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::CacheInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDescription, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FormatRequest, boost::serialization::track_never);
