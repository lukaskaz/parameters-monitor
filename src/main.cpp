#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"
#include "monitoring/interfaces/group/monitoring.hpp"
#include "monitoring/interfaces/rpi5/temperature/cpu/monitoring.hpp"
#include "monitoring/interfaces/rpi5/temperature/gpu/monitoring.hpp"

#include <chrono>
#include <iostream>

using namespace std::string_literals;
using namespace std::chrono_literals;

static const auto separator{","s};
static const auto delay = 1000ms;

auto getgraph() -> std::shared_ptr<graphs::GraphIf>
{
    using namespace graphs::dygraph::rangesamples;
    return graphs::Factory::create<Graph, configall_t>(
        {{"cpu/gpu temperature", "time/date", "temperature["s + "\u2103" + "]"},
         {1200, 400},
         {1s, 60, {{"temprecords.csv", "timestamp,cputemp,gputemp", 86400}}}});
}

auto getmonitor() -> std::shared_ptr<monitoring::MonitoringIf>
{
    using namespace monitoring::rpi5::temperature;
    auto cputemp =
        monitoring::Factory::create<cpu::Monitoring, cpu::config_t>({});
    auto gputemp =
        monitoring::Factory::create<gpu::Monitoring, gpu::config_t>({});
    return monitoring::Factory::create<monitoring::group::Monitoring,
                                       monitoring::group::configall_t>(
        {{cputemp, gputemp}, separator});
}

int main(int argc, [[maybe_unused]] char** argv)
{
    if (argc == 1)
    {
        try
        {
            auto graph = getgraph();
            auto monitor = getmonitor();

            graph->start();
            auto tm{graphs::helpers::TimeMonitor()};
            while (true)
            {
                graph->add(tm.gettimestamp() + separator + monitor->reading());
                usleep(delay.count() * 1000);
            }
        }
        catch (const std::exception& err)
        {
            std::cerr << "[ERROR] " << err.what() << '\n';
        }
    }
    return 0;
}
