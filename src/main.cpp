#include "graphs/interfaces/dygraph/rangesamples/graph.hpp"
#include "shellcommand.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std::string_literals;

std::shared_ptr<graphs::GraphIf> getgraph()
{
    using namespace graphs::dygraph::rangesamples;
    auto graph = graphs::GraphFactory::create<Graph, configall_t>(
        {{"cpu/gpu temperature", "time/date", "temperature["s + "\u2103" + "]"},
         {1200, 400},
         {1s, 60, {{"temprecords.csv", "timestamp,cputemp,gputemp", 86400}}}});
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
    static const auto cputempfile{"/sys/class/thermal/thermal_zone0/temp"s};
    auto reading = getfileline(cputempfile);
    if (!reading.empty())
    {
        double value = std::atoi(reading.c_str()) / 1000.f;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << value;
        return ss.str();
    }
    throw std::runtime_error("Cannot get CPU temp");
}

std::string getgputemp()
{
    static const auto gputempcmd{
        "vcgencmd measure_temp|sed -r \"s/temp=(.*)'C/\\1/g\""s};
    std::vector<std::string> resultvect;
    shell::BashCommand().run(gputempcmd, resultvect);
    if (!resultvect.empty())
        return resultvect.at(0);
    throw std::runtime_error("Cannot get GPU temp");
}

int main(int argc, [[maybe_unused]] char** argv)
{
    if (argc == 1)
    {
        try
        {
            auto graph{getgraph()};
            auto tm{graphs::helpers::TimeMonitor()};
            while (true)
            {
                graph->add(tm.gettimestamp() + "," + getcputemp() + "," +
                           getgputemp());
                usleep(1000 * 1000);
            }
        }
        catch (const std::exception& err)
        {
            std::cerr << "[ERROR] " << err.what() << '\n';
        }
    }
    return 0;
}
