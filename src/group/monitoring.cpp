#include "monitoring/interfaces/group/monitoring.hpp"

#include <algorithm>
#include <ranges>
#include <source_location>

namespace monitoring::group
{

struct Monitoring::Handler
{
  public:
    Handler(const configall_t& config) :
        group{std::get<std::vector<std::shared_ptr<MonitoringIf>>>(config)},
        separator{std::get<std::string>(config)}
    {}

    std::string reading() const
    {
        std::string readings;
        std::ranges::for_each(group, [this, &readings](const auto& iface) {
            readings.append(iface->reading());
            if (group.back() != iface)
                readings.append(separator);
        });
        return readings;
    }

  private:
    const std::vector<std::shared_ptr<MonitoringIf>> group;
    const std::string separator;
};

Monitoring::Monitoring(const config_t& config)
{
    using namespace std::string_literals;
    handler = std::visit(
        [](const auto& config) -> decltype(Monitoring::handler) {
            if constexpr (!std::is_same<const std::monostate&,
                                        decltype(config)>())
            {
                return std::make_unique<Monitoring::Handler>(config);
            }
            throw std::runtime_error(
                std::source_location::current().function_name() +
                "-> grouping config not supported"s);
        },
        config);
}
Monitoring::~Monitoring() = default;

std::string Monitoring::reading() const
{
    return handler->reading();
}

} // namespace monitoring::group