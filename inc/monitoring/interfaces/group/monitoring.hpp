#pragma once

#include "monitoring/factory.hpp"

#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace monitoring::group
{

using configall_t =
    std::tuple<std::vector<std::shared_ptr<MonitoringIf>>, std::string>;
using config_t = std::variant<std::monostate, configall_t>;

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

} // namespace monitoring::group