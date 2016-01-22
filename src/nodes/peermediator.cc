#include "peermediator.hh"

namespace eclipse { 
// cnstructor {{{
PeerMediator::PeerMediator(PeerRemote* r, PeerLocal* l) : remote(*r), local(*l) 
{
  
}
// }}}
// set_boundaries {{{
void PeerMediator::set_boundaries (message::Boundaries* b) {
  *histogram << b;
}
// }}}
}
