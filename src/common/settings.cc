//
// @author     Vicente Adolfo Bolea Sanchez
// @brief      Settings impl using boost library.
//
// Includes {{{
#define BOOST_SPIRIT_THREADSAFE
#include "settings.hh"
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <ifaddrs.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#define FINAL_PATH "/eclipse.json"

static std::map<std::string, std::string> default_ops {
  {"log.type", "LOG_LOCAL6"},
  {"log.name", "ECLIPSE"},
  {"log.mask", "DEBUG"},
  {"cache.numbin", "100"},
  {"cache.size",   "200000"},
  {"cache.concurrency", "1"},
  {"network.serialization", "binary"},
  {"fileystem.block", "137438953"},
  {"fileystem.buffer", "512"},
  {"fileystem.replica", "1"},
  {"addons.zk.enabled", "false"} 
};

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using namespace boost::property_tree;

static vector<string> tokenize(std::string str_separated_by_colons) {
  stringstream ss(str_separated_by_colons);

  vector<string> output;
  string token; 
  while (std::getline(ss, token, ':')) {
    output.push_back(token);
  }
  return move(output);
}

//}}}
// class SettingsImpl {{{
class Settings::SettingsImpl {
  protected:
    ptree pt;
    string config_path, input;
    bool nofile = false;
    bool get_project_path ();

  public:
    SettingsImpl() = default;
    SettingsImpl(string in) : input (in), nofile (true) { }
    bool load ();

    template <typename T> T get (string&) ;
    string getip () const ;
};
//}}}
// get_project_path {{{
bool Settings::SettingsImpl::get_project_path ()
{
  const string home_location   = string(getenv("HOME")) + "/.eclipse.json";
  const string system_location = "/etc/eclipse.json";

  if (access(home_location.c_str(), F_OK) == EXIT_SUCCESS) {     // Then at home
    config_path = home_location;

  } else if (access(system_location.c_str(), F_OK) == EXIT_SUCCESS) {   // Then at /etc
    config_path = system_location;

  } else {

#ifdef ECLIPSE_HOME_CONF_PATH
    config_path = string(ECLIPSE_HOME_CONF_PATH) + "/.eclipse.json";

#elif defined ECLIPSE_CONF_PATH
    config_path = string(ECLIPSE_CONF_PATH) + FINAL_PATH;               // Then configure one
#else
    return false;
#endif
  }

  return true;
}
// }}}
// load {{{
//
bool Settings::SettingsImpl::load ()
{
  for (auto& kv : default_ops) {
    pt.put(kv.first, kv.second);
  }

  if (not nofile) {
    get_project_path();
    json_parser::read_json (config_path, pt);

  } else {
    std::stringstream ss; ss << input;
    json_parser::read_json (ss, pt);
  }

  return true;
}
// }}}
// getip {{{
string Settings::SettingsImpl::getip () const
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  struct ifreq ifr = {{0}}; //GCC bug
#pragma GCC diagnostic pop 

  string interface = pt.get<string> ("network.iface");

  ifr.ifr_addr.sa_family = AF_INET;        // I want to get an IPv4 IP address
  interface.copy (ifr.ifr_name, IFNAMSIZ); // I want IP address attached to "eth0"

  int fd = socket (AF_INET, SOCK_DGRAM, 0);
  ioctl (fd, SIOCGIFADDR, &ifr);
  close (fd);

  auto addr = reinterpret_cast<struct sockaddr_in*> (&ifr.ifr_addr);
  return inet_ntoa (addr->sin_addr);
}
// }}}
// Get specializations {{{
char* get_env (std::string in) {
  char* property;
  auto env_str = in;
  std::replace(env_str.begin(), env_str.end(), '.', '_'); 
  std::transform(env_str.begin(), env_str.end(), env_str.begin(), toupper);
  property = std::getenv(env_str.c_str());
  return property;
}

template<> string Settings::SettingsImpl::get (string& str) {
  auto property = get_env(str);
  if (property != nullptr) {
    return string(property);
  
  } else {
   return pt.get<string> (str.c_str());
  }
}

template<> int    Settings::SettingsImpl::get (string& str) {
  auto property = get_env(str);
  if (property != nullptr) {
    return stoi(string(property));
  
  } else {
   return pt.get<int> (str.c_str());
  }
}

template<> vector<string> Settings::SettingsImpl::get (string& str) {
  vector<string> output;

  auto property = get_env(str);
  if (property != nullptr)
    return tokenize(property);

  auto& subtree = pt.get_child (str.c_str());

  for (auto& v : subtree)
    output.push_back (v.second.data());

  //transform (subtree.begin(), subtree.end(), back_inserter(output), [](ptree::value_type &in) {
  //            return in.second.data(); 
  //          });
  return output;
}
//}}}
// Settings method{{{
//
Settings::Settings() : impl {new SettingsImpl()} { }
Settings::Settings(string in) : impl {new SettingsImpl(in)} { }
Settings::~Settings() {  }

Settings::Settings(Settings&& that) { this->impl = std::move(that.impl); }

void Settings::operator=(Settings&& that) { 
  Settings (std::forward<Settings>(that)); 
}

Settings& Settings::load () & { 
  impl->load (); return *this; 
}

Settings&& Settings::load () && {
  impl->load (); return std::move(*this); 
}

string Settings::getip () const { return impl->getip(); }

template<typename T> T Settings::get (string str) const {
  return impl->get<T>(str);
}

template int            Settings::get (string) const;
template string         Settings::get (string) const;
template vector<string> Settings::get (string) const;
// }}}
