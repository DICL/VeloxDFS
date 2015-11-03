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
namespace network {
  template <typename Archive>
    void serialize (Archive& ar, network::Message& m, unsigned int) {
      ar & BOOST_SERIALIZATION_NVP(m.origin);
      ar & BOOST_SERIALIZATION_NVP(m.destination);
    }

  template <typename Archive>
    void serialize (Archive& ar, network::Boundaries& b, unsigned int) {
      ar & BASE_OBJECT(Message, b);
      ar & BOOST_SERIALIZATION_NVP(b.data);
    }

  template <typename Archive>
    void serialize (Archive& ar, network::KeyValue& k, unsigned int) {
      ar & BASE_OBJECT(Message, k);
      ar & BOOST_SERIALIZATION_NVP(k.key);
      ar & BOOST_SERIALIZATION_NVP(k.value);
    }

  template <typename Archive>
    void serialize (Archive& ar, network::Control& c, unsigned int) {
      ar & BASE_OBJECT(Message, c);
      ar & BOOST_SERIALIZATION_NVP(c.type);
    }
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(network::Message);

//! 3) Also here
BOOST_CLASS_TRACKING(network::Message,    boost::serialization::track_never);
BOOST_CLASS_TRACKING(network::Boundaries, boost::serialization::track_never);
BOOST_CLASS_TRACKING(network::KeyValue,   boost::serialization::track_never);
BOOST_CLASS_TRACKING(network::Control,    boost::serialization::track_never);
