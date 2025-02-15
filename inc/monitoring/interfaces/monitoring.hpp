#pragma once

#include <string>

namespace monitoring
{

class MonitoringIf
{
  public:
    virtual ~MonitoringIf() = default;
    virtual std::string reading() const = 0;
};

} // namespace monitoring
