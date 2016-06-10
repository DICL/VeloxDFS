#include "context_singleton.hh"

Context& context = *Context::connect();

Context* Context::singleton = nullptr;
