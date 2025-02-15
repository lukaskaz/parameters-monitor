#include "monitoring/interfaces/rpi5/temperature/cpu/monitoring.hpp"

#include <fstream>
#include <iomanip>
#include <source_location>
#include <sstream>

namespace monitoring::rpi5::temperature::cpu
{

static constexpr auto defaultreadingfile{
    "/sys/class/thermal/thermal_zone0/temp"};

struct Monitoring::Handler
{
  public:
    explicit Handler(const std::string& file) : readingfile{file}
    {}

    std::string reading() const
    {
        auto value = std::atoi(getraw().c_str()) / tempcoef;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value;
        return ss.str();
    }

  private:
    const std::string readingfile;
    const double tempcoef{1000.f};

    std::string
        getraw(const std::string funcname =
                   std::source_location::current().function_name()) const
    {
        if (std::ifstream ifs{readingfile}; ifs.is_open())
            if (std::string raw; std::getline(ifs, raw))
                return raw;
            else
                throw std::runtime_error(funcname +
                                         "-> cannot obtain cpu temperature");
        else
            throw std::runtime_error(
                funcname + "-> cannot open reading file: " + readingfile);
    }
};

Monitoring::Monitoring(const config_t& config)
{
    handler = std::visit(
        [](const auto& config) -> decltype(Monitoring::handler) {
            if constexpr (std::is_same<const std::monostate&,
                                       decltype(config)>())
            {
                return std::make_unique<Monitoring::Handler>(
                    defaultreadingfile);
            }
            else
            {
                return std::make_unique<Monitoring::Handler>(config);
            }
        },
        config);
}
Monitoring::~Monitoring() = default;

std::string Monitoring::reading() const
{
    return handler->reading();
}

} // namespace monitoring::rpi5::temperature::cpu
