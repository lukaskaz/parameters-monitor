#pragma once

#include "monitoring/factory.hpp"

#include <string>
#include <variant>

namespace monitoring::rpi5::temperature::cpu
{

using config_t = std::variant<std::monostate, std::string>;

class Monitoring : public MonitoringIf
{
  public:
    ~Monitoring();
    std::string reading() const override;

  private:
    friend class monitoring::Factory;
    Monitoring(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace monitoring::rpi5::temperature::cpu
