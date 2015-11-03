#ifndef __TASK_HH_
#define __TASK_HH_

#include "ecfs.hh"
#include <vector>
#include <string>
#include <functional>
#include <iostream>

enum class Task_status {WAITING, RUNNING, COMPLETED}; 

/*
 * Needed for make able the hash_multimap to hash the Task_status key
 *
 */
namespace std {
  template<>
    struct hash<Task_status> {
      std::size_t operator() (const Task_status& in) const {
        return std::hash<int>()(static_cast<int>(in));
      }
    };
}

/*
 * Task class, basically a container of the several attributes.
 *  - Note that setters can be chainable, use this syntax:
 *    Task& a = Task().fd(123).id(232);
 *  - Movable, watch out with this
 */
class Task {
  typedef std::vector<string> vec_str;
  typedef std::vector<int> vec_int;

  public:
    Task() { }

    Task (const Task& rhs) {
      *this = rhs;
    }
    Task (Task&& rhs) {
      *this = std::forward<Task>(rhs);
    }
    Task& operator= (const Task& rhs) {
      id_ = rhs.id_;
      fd_ = rhs.fd_;
      return *this;
    }
    Task& operator= (Task&& rhs) {
      peerids_    = std::move(rhs.peerids_);
      inputpaths_ = std::move(rhs.inputpaths_);
      numiblocks_ = std::move(rhs.numiblocks_);
      id_ = rhs.id_;
      fd_ = rhs.fd_;
      status_ = rhs.status_;
      role_ = rhs.role_;
      return *this;
    }

  public:
    // Setters {{{
    Task& id(int i)                    { id_ = i; return *this; }
    Task& fd(int i)                    { fd_ = i; return *this; }
    Task& status(Task_status i)        { status_ = i; return *this; }
    Task& role(mr_role& i)             { role_ = i; return *this; }
    Task& peerids(const vec_int& i)    { peerids_ = i; return *this; }
    Task& numiblocks(const vec_int& i) { numiblocks_ = i; return *this; }
    Task& inputpaths(const vec_str& i) { inputpaths_ = i; return *this; }

    int id()                    { return id_; }
    int fd()                    { return fd_;  }
    Task_status status()        { return status_; }
    mr_role role()              { return role_; }
    const vec_int& peerids()    { return peerids_; }
    const vec_int& numiblocks() { return numiblocks_; }
    const vec_str& inputpaths() { return inputpaths_; }
    // }}}
  private:
    int id_, fd_;
    vec_int peerids_, numiblocks_;
    vec_str inputpaths_;

    Task_status status_;
    mr_role role_;
};

#endif
