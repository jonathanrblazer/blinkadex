#include <gpiod.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <csignal>

static volatile bool running = true;

void signal_handler(int)
{
    running = false;
}

int main()
{
    constexpr const char* GPIO_CHIP = "/dev/gpiochip2";
    constexpr unsigned int GPIO_LINE = 3;   // GPIO_3
    constexpr int BLINK_HALF_PERIOD_US = 830; // 1 ms → 500 Hz. 500 ms -> 1 Hz

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    gpiod_chip* chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip\n";
        return 1;
    }

    gpiod_line* line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        std::cerr << "Failed to get GPIO line\n";
        gpiod_chip_close(chip);
        // return 1;
    }

    if (gpiod_line_request_output(line, "gpio-blink", 0) < 0) {
        std::cerr << "Failed to request GPIO as output\n";
        gpiod_chip_close(chip);
        // return 1;
    }

    std::cout << "Blinking GPIO_3 at ~500 Hz. Ctrl+C to stop.\n";

    while (running) {
        gpiod_line_set_value(line, 1);
        std::this_thread::sleep_for(std::chrono::microseconds(BLINK_HALF_PERIOD_US));

        gpiod_line_set_value(line, 0);
        std::this_thread::sleep_for(std::chrono::microseconds(BLINK_HALF_PERIOD_US));
    }

    gpiod_line_set_value(line, 0);
    gpiod_chip_close(chip);

    std::cout << "Stopped.\n";
    return 0;
}
