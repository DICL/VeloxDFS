#pragma once
#include "context.hh"

extern Context& context;

#define PANIC(X, ...) context.logger->panic(X, ##__VA_ARGS__ )
#define ERROR(X, ...) context.logger->error(X, ##__VA_ARGS__ )
#define WARN(X, ...) context.logger->warn(X, ##__VA_ARGS__)
#define INFO(X, ...) context.logger->info(X, ##__VA_ARGS__)
#define DEBUG(X, ...) context.logger->debug(X, ##__VA_ARGS__)

#define GET_INT(X) context.settings.get<int>(X)
#define GET_STR(X) context.settings.get<std::string>(X)
#define GET_VEC_STR(X) context.settings.get<std::vector<std::string>>(X)
#define GET_BOOL(X) (GET_STR(X) == "true" ? true : false)
#define GET_INDEX(X) context.histogram->get_index(X)
#define GET_INDEX_IN_BOUNDARY(X) context.histogram->random_within_boundaries(X)
