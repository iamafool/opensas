#ifndef STEPTIMER_H
#define STEPTIMER_H

#include <chrono>
#include <ctime>
#include <spdlog/spdlog.h>

namespace sass {

    class StepTimer {
    public:
        StepTimer();
        ~StepTimer() = default;

        // Start or restart the timer
        void start();

        // Stop the timer
        void stop();

        // Retrieve the elapsed real (wall-clock) time in seconds
        double getRealTimeSeconds() const;

        // Retrieve the elapsed CPU time in seconds
        double getCpuTimeSeconds() const;

    private:
        bool running;

        // For wall-clock time
        std::chrono::time_point<std::chrono::steady_clock> startReal;
        std::chrono::time_point<std::chrono::steady_clock> endReal;

        // For CPU time
        std::clock_t startCpu;
        std::clock_t endCpu;
    };

    class ScopedStepTimer {
    public:
        ScopedStepTimer(const std::string& stepName, spdlog::logger& logger)
            : name(stepName), log(logger)
        {
            timer.start();
        }
        ~ScopedStepTimer() {
            timer.stop();
            double realTime = timer.getRealTimeSeconds();
            double cpuTime = timer.getCpuTimeSeconds();
            log.info("NOTE: {} used (Total process time):", name);
            log.info("      real time           {:.2f} seconds", realTime);
            log.info("      cpu time            {:.2f} seconds", cpuTime);
        }
    private:
        StepTimer timer;
        std::string name;
        spdlog::logger& log;
    };

} // namespace sass

#endif // STEPTIMER_H
