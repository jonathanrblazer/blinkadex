#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <limits>

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

bool read_user_input(double& frequency_hz, double& duty_percent) {
    std::cout << "\nEnter PWM frequency (Hz) [Ctrl+C to exit]: ";
    if (!(std::cin >> frequency_hz)) {
        return false;
    }

    std::cout << "Enter duty cycle (%) [0–100]: ";
    if (!(std::cin >> duty_percent)) {
        return false;
    }

    if (frequency_hz <= 0.0 || duty_percent < 0.0 || duty_percent > 100.0) {
        std::cerr << "Invalid input. Frequency must be > 0 and duty 0–100.\n";
        return false;
    }

    return true;
}

int main() {
    constexpr int PWM_CHIP    = 0; // pwmchip0
    constexpr int PWM_CHANNEL = 0; // pwm0

    const std::string chip = "/sys/class/pwm/pwmchip" + std::to_string(PWM_CHIP);
    const std::string pwm  = chip + "/pwm" + std::to_string(PWM_CHANNEL);

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        // Export PWM if needed
        if (!std::ifstream(pwm)) {
            write_sysfs(chip + "/export", std::to_string(PWM_CHANNEL));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        while (running) {
            double frequency_hz = 0.0;
            double duty_percent = 0.0;

            if (!read_user_input(frequency_hz, duty_percent)) {
                if (!running) break;

                // Clear bad input
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            const long long period_ns =
                static_cast<long long>(1e9 / frequency_hz);
            const long long duty_ns =
                static_cast<long long>(period_ns * (duty_percent / 100.0));

            // Safely reconfigure PWM
            write_sysfs(pwm + "/enable", "0");
            write_sysfs(pwm + "/period", std::to_string(period_ns));
            write_sysfs(pwm + "/duty_cycle", std::to_string(duty_ns));
            write_sysfs(pwm + "/enable", "1");

            std::cout << "PWM updated: "
                      << frequency_hz << " Hz, "
                      << duty_percent << "% duty\n";
        }

        write_sysfs(pwm + "/enable", "0");
        std::cout << "\nPWM stopped.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "PWM error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
