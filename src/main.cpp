#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

static volatile bool running = true;

void signal_handler(int) {
    running = false;
}

void write_sysfs(const std::string& path, const std::string& value) {
    std::ofstream fs(path);
    if (!fs) {
        throw std::runtime_error("Failed to open " + path);
    }
    fs << value;
}

int main() {
    constexpr int PWM_CHIP = 0;   // pwmchip0
    constexpr int PWM_CHANNEL = 0; // pwm0
    constexpr int PERIOD_NS = 2'000'000;      // 500 Hz
    constexpr int DUTY_NS   = 100'000;      // 50%

    const std::string chip = "/sys/class/pwm/pwmchip" + std::to_string(PWM_CHIP);
    const std::string pwm  = chip + "/pwm" + std::to_string(PWM_CHANNEL);

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        // Export if not already exported
        if (!std::ifstream(pwm)) {
            write_sysfs(chip + "/export", std::to_string(PWM_CHANNEL));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        write_sysfs(pwm + "/enable", "0");
        write_sysfs(pwm + "/period", std::to_string(PERIOD_NS));
        write_sysfs(pwm + "/duty_cycle", std::to_string(DUTY_NS));
        write_sysfs(pwm + "/enable", "1");

        std::cout << "PWM running: 500 Hz, 50% duty (Ctrl+C to stop)\n";

        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        write_sysfs(pwm + "/enable", "0");
    }
    catch (const std::exception& e) {
        std::cerr << "PWM error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
