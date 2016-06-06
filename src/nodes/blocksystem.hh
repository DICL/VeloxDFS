#pragma once

#include "block.hh"


struct BlockSystem {
  void insert (Blob*) = 0;
  void open (Blob*) = 0;
  void exists (Blob*) = 0;
  void rename (Blob*) = 0;
  void remove (Blob*) = 0;
  void format (Blob*) = 0;
  void list (Blob*) = 0;
};
