#pragma once

namespace Files {
  class FileText {
    private:
      std::vector<Chunk> file;
      std::string filename;

    public:
      FileText(std::string);
      read_chunk();
  };

  struct Chunk {
    std::array<char, BLOCKSIZE> data;
    Chunk();
  };
}
