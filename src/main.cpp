#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std::string_literals;

std::shared_ptr<graphs::GraphIf> getgraph()
{
    using namespace graphs::dygraph::rangesamples;
    auto graph = graphs::GraphFactory::create<Graph, configall_t>(
        {{"cpu temperature", "time/date", "temperature["s + "\u2103" + "]"},
         {1200, 400},
         {1s, 60, {{"temprecords.csv", "timestamp,temperature", 86400}}}});
    graph->start();
    return graph;
}

std::string getfileline(const std::string& file)
{
    std::ifstream ifs{file};
    if (!ifs.is_open())
        throw std::runtime_error("Cannot open " + file);
    std::string line;
    std::getline(ifs, line);
    return line;
}

std::string getcputemp()
{
    static constexpr auto cputempfile{"/sys/class/thermal/thermal_zone0/temp"s};
    double value = std::atoi(getfileline(cputempfile).c_str()) / 1000.f;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
}

int main(int argc, [[maybe_unused]] char** argv)
{
    if (argc == 1)
    {
        auto graph{getgraph()};
        auto tm{graphs::helpers::TimeMonitor()};
        while (true)
        {
            graph->add(tm.gettimestamp() + "," + getcputemp());
            usleep(1000 * 1000);
        }
    }
    return 0;
}
