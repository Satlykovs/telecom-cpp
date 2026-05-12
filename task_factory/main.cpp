#include "models.hpp"
#include "parser.hpp"
#include "simulation.hpp"

#include <iostream>
#include <ostream>
int main(int argc, char* argv[])
{
    if (argc < 2)
        return 0;

    try {
        const Config config = parseInput(argv[1]);

        Simulation simulation(config);
        simulation.run();
    } catch (const ParseError& e) {
        std::cout << e.what() << '\n';
        return 1;
    } catch (const std::exception& _) {
        return 1;
    }

    return 0;
}
