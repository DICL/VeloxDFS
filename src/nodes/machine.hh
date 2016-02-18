#pragma once
#include <string>
#include <memory>

namespace eclipse {
template <typename T> using u_ptr = std::unique_ptr<T>;

class Machine {
  protected:
    int id;

  public:
    Machine() = default;
    Machine(int);
    virtual ~Machine() { } /* Polymorphic class */

    virtual std::string get_ip () const = 0;
    int get_id () const;
};

}
