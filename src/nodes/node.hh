#pragma once
#include <string>

namespace Nodes {
  enum NodeTypes { PEER, MASTER, APP, EXECUTOR };
  const int header_length = 8;

  class Node {
    protected:
      int id;

    public:
      Node() = default;
      Node(int);
      virtual ~Node() { } /* Polymorphic class */

      virtual std::string get_ip () const = 0;
      int get_id () const;
  };

}
