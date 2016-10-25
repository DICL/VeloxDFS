#include "context_singleton.hh"

//Context* Context::singleton = nullptr;
Context& context = *Context::connect();

