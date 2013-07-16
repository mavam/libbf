#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "util/configuration.h"

class configuration : public util::configuration
{
public:
  configuration();
protected:
  virtual void verify() override;
};

#endif
