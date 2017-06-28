#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "util/configuration.h"

class config : public util::configuration<config> {
public:
  config() = default;

  void initialize();
  std::string banner() const;
};

#endif
