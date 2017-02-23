#include "router_decorator.hh"

using namespace eclipse;

RouterDecorator::RouterDecorator(Router* _router) : router(_router) { }

void RouterDecorator::on_read(messages::Message* m, Channel* c) {
  auto type = m->get_type();
  if (routing_table.find(type) != routing_table.end()) {
    routing_table[type](m, c);
  } else {
    router->on_read(m, c);
  }
}
