#include "models.hpp"
#include "parser.hpp"
#include "simulation.hpp"

#include <fstream>
#include <utility>

int main(int argc, char* argv[])
{
    if (argc < 2)
        return 0;

    std::ofstream out("result.txt");
    if (!out.is_open())
        return 1;

    try {
        Config config = parseInput(argv[1]);
        Simulation simulation(std::move(config), out);
        simulation.run();
    } catch (const ParseError& e) {
        out << e.what() << '\n';
        return 1;
    } catch (const std::exception&) {
        return 1;
    }

    return 0;
}