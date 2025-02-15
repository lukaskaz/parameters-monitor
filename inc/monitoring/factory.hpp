#pragma once

#include "monitoring/interfaces/monitoring.hpp"

#include <memory>

namespace monitoring
{

class Factory
{
  public:
    template <typename T, typename C>
    static std::shared_ptr<MonitoringIf> create(const C& config)
    {
        return std::shared_ptr<T>(new T(config));
    }
};

} // namespace monitoring