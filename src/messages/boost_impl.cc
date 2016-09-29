#include "boost_impl.hh"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

//! 4) Also here
// common
BOOST_CLASS_EXPORT(eclipse::messages::Boundaries);
BOOST_CLASS_EXPORT(eclipse::messages::KeyValue);
BOOST_CLASS_EXPORT(eclipse::messages::Control);
BOOST_CLASS_EXPORT(eclipse::messages::KeyRequest);
BOOST_CLASS_EXPORT(eclipse::messages::Task);
BOOST_CLASS_EXPORT(eclipse::messages::Reply);
BOOST_CLASS_EXPORT(eclipse::messages::CacheInfo);
BOOST_CLASS_EXPORT(eclipse::messages::FormatRequest);
BOOST_CLASS_EXPORT(eclipse::messages::CheckExist);
BOOST_CLASS_EXPORT(eclipse::messages::OffsetKeyValue);
BOOST_CLASS_EXPORT(eclipse::messages::MetaData);

// file
BOOST_CLASS_EXPORT(eclipse::messages::FileInfo);
BOOST_CLASS_EXPORT(eclipse::messages::FileUpdate);
BOOST_CLASS_EXPORT(eclipse::messages::FileList);
BOOST_CLASS_EXPORT(eclipse::messages::FileRequest);
BOOST_CLASS_EXPORT(eclipse::messages::FileDescription);
BOOST_CLASS_EXPORT(eclipse::messages::FileDel);

// block
BOOST_CLASS_EXPORT(eclipse::messages::BlockInfo);
BOOST_CLASS_EXPORT(eclipse::messages::BlockList);
BOOST_CLASS_EXPORT(eclipse::messages::BlockRequest);
BOOST_CLASS_EXPORT(eclipse::messages::BlockDel);
BOOST_CLASS_EXPORT(eclipse::messages::BlockUpdate);

// slice
BOOST_CLASS_EXPORT(eclipse::messages::SliceInfo);
BOOST_CLASS_EXPORT(eclipse::messages::SliceList);
BOOST_CLASS_EXPORT(eclipse::messages::SliceRequest);
BOOST_CLASS_EXPORT(eclipse::messages::SliceDel);

// mdlist
BOOST_CLASS_EXPORT(eclipse::messages::MdlistInfo);
BOOST_CLASS_EXPORT(eclipse::messages::MdlistList);
BOOST_CLASS_EXPORT(eclipse::messages::MdlistRequest);
BOOST_CLASS_EXPORT(eclipse::messages::MdlistDel);
BOOST_CLASS_EXPORT(eclipse::messages::MdlistUpdate);

// node
BOOST_CLASS_EXPORT(eclipse::messages::NodeInfo);
BOOST_CLASS_EXPORT(eclipse::messages::NodeList);
BOOST_CLASS_EXPORT(eclipse::messages::NodeRequest);
BOOST_CLASS_EXPORT(eclipse::messages::NodeDel);
BOOST_CLASS_EXPORT(eclipse::messages::NodeUpdate);
BOOST_CLASS_EXPORT(eclipse::messages::NodeDescription);
