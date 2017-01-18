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
#include "fileexist.hh"
#include "offsetkv.hh"
#include "blockupdate.hh"
#include "fileupdate.hh"
#include "metadata.hh"
#include "blockstatus.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/base_object.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif

//! 2) Also here
namespace boost{
namespace serialization{

template <typename Archive> void serialize (Archive&, eclipse::messages::Message&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Boundaries&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyValue&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::OffsetKeyValue&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Control&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::KeyRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Task&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileUpdate&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileList&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockUpdate&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Reply&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::CacheInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDescription&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDel&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockDel&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FormatRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileExist&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::MetaData&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockStatus&, unsigned);
}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(eclipse::messages::Message);

//! 3) Also here
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Boundaries);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyValue);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::OffsetKeyValue);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Control);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::KeyRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Task);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockUpdate);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::CacheInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockDel);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::MetaData);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockStatus);

//! 4) and here
BOOST_CLASS_TRACKING(eclipse::messages::Message, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Boundaries, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::OffsetKeyValue, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Control, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::KeyRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockUpdate, boost::serialization::track_never);
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
BOOST_CLASS_TRACKING(eclipse::messages::FileExist, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockStatus, boost::serialization::track_never);
