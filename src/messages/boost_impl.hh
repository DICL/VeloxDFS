//
//! @brief This module implements the serialization using BOOST SERIALIZAITON
//! For every new message class that you want to add you must add its 
//! functions here and its cc file.
//
#pragma once

// 1) Add your class here
#include "message.hh"
#include "fileinfo.hh"
#include "filelist.hh"
#include "blockinfo.hh"
#include "reply.hh"
#include "filerequest.hh"
#include "blockrequest.hh"
#include "filedescription.hh"
#include "filedel.hh"
#include "formatrequest.hh"
#include "fileexist.hh"
#include "fileupdate.hh"
#include "metadata.hh"
#include "IOoperation.hh"
#include "../common/blockmetadata.hh"
#include "../common/logical_block_metadata.hh"

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/base_object.hpp>

#ifndef BASE_OBJECT
#define BASE_OBJECT(X,Y)                          \
  boost::serialization::make_nvp(#X,              \
      boost::serialization::base_object<X>(Y));
#endif

// 2) Also here
namespace boost{
namespace serialization{

template <typename Archive> void serialize (Archive&, eclipse::messages::Message&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileUpdate&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileList&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockInfo&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::Reply&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::BlockRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDescription&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileDel&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FormatRequest&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::FileExist&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::MetaData&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::BlockMetadata&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::messages::IOoperation&, unsigned);
template <typename Archive> void serialize (Archive&, eclipse::logical_block_metadata&, unsigned);
}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(eclipse::messages::Message);

// 3) Also here
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileList);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::Reply);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileDel);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::FileExist);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::MetaData);
BOOST_CLASS_EXPORT_KEY(eclipse::BlockMetadata);
BOOST_CLASS_EXPORT_KEY(eclipse::messages::IOoperation);
BOOST_CLASS_EXPORT_KEY(eclipse::logical_block_metadata);

// 4) and here
BOOST_CLASS_TRACKING(eclipse::messages::Message, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileUpdate, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockInfo, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileList, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::Reply, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::BlockRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDescription, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileDel, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FormatRequest, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::FileExist, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::BlockMetadata, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::messages::IOoperation, boost::serialization::track_never);
BOOST_CLASS_TRACKING(eclipse::logical_block_metadata, boost::serialization::track_never);
