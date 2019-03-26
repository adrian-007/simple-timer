#include <iostream>
#include <iomanip>

#include <simple_timer.hpp>

int main()
{
    std::cout << "Starting periodic timer with 1000 ms duration." << std::endl;

    SimpleTimer timer;
    timer.start(std::chrono::milliseconds(1000u), []()
    {
        auto t = std::chrono::system_clock::now().time_since_epoch();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t);

        std::cout << "\t[" << std::setfill('0') << std::setw(8) << ms.count() << "]: callback called" << std::endl;
    });

    std::cout << "Press enter to end the program...\n";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    timer.stop();
    std::cout << "Timer stopped" << std::endl;

    return 0;
}
