#include "monitoring/interfaces/rpi5/temperature/gpu/monitoring.hpp"

#include "shellcommand.hpp"

#include <source_location>

namespace monitoring::rpi5::temperature::gpu
{

static constexpr auto defaultreadingcmd{
    "vcgencmd measure_temp|sed -r \"s/temp=(.*)'C/\\1/g\""};

struct Monitoring::Handler
{
  public:
    explicit Handler(const std::string& command) :
        readingcmd{command}, shellIf{std::make_unique<shell::BashCommand>()}
    {}

    std::string reading() const
    {
        return getraw();
    }

  private:
    const std::string readingcmd;
    std::unique_ptr<shell::ShellCommand> shellIf;

    std::string
        getraw(const std::string funcname =
                   std::source_location::current().function_name()) const
    {
        if (std::vector<std::string> outputvect;
            !shellIf->run(readingcmd, outputvect))
        {
            if (!outputvect.empty())
                return outputvect[0];
            else
                throw std::runtime_error(funcname +
                                         "-> cannot obtain gpu temperature");
        }
        else
        {
            throw std::runtime_error(
                funcname + "-> cannot run reading cmd: " + readingcmd);
        }
    }
};

Monitoring::Monitoring(const config_t& config)
{
    handler = std::visit(
        [](const auto& config) -> decltype(Monitoring::handler) {
            if constexpr (std::is_same<const std::monostate&,
                                       decltype(config)>())
            {
                return std::make_unique<Monitoring::Handler>(defaultreadingcmd);
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

} // namespace monitoring::rpi5::temperature::gpu