#pragma once
#include "context.hh"

extern Context& context;

#define ERROR(X, ...) context.logger->error(X, ##__VA_ARGS__ )
#define WARN(X, ...) context.logger->warn(X, ##__VA_ARGS__)
#define INFO(X, ...) context.logger->info(X, ##__VA_ARGS__)
#define DEBUG(X, ...) context.logger->debug(X, ##__VA_ARGS__)
