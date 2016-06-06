#pragma once

namespace eclipse {

class EclipseDFS: public System {
  public:
    EclipseDFS(DFS*, DBS*);
    DFS* get_dfs();
    DBS* get_dio();

  private:
    DFS* dfs;
    DBS* dio;
};
  
} /* eclipse  */ 



