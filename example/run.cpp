#include "../opt.hpp"

#include <iomanip>
#include <iostream>

enum Mode {
    OneShot,
    Repeat,
};

template<>
const opt::Option<Mode>::value_map opt::Option<Mode>::values = {
    { "oneshot", OneShot },
    { "after", OneShot },
    { "repeat", Repeat },
    { "every", Repeat },
};

enum Unit {
    Second,
    Minute,
    Hour,
};

template<>
const opt::Option<Unit>::value_map opt::Option<Unit>::values = {
    { "seconds", Second },
    { "sec", Second },
    { "s", Second },
    { "minutes", Minute },
    { "m", Minute },
    { "hours", Hour },
    { "hr", Hour },
    { "h", Hour },
};

std::ostream& operator<<(std::ostream& stream, Mode mode) {
    switch (mode) {
        case OneShot:
            return stream << "OneShot";
        case Repeat:
            return stream << "Repeat";
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, Unit unit) {
    switch (unit) {
        case Second:
            return stream << "Second";
        case Minute:
            return stream << "Minute";
        case Hour:
            return stream << "Hour";
    }

    return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, opt::Option<T> const& opt) {
    return stream << opt.get() << (opt.is_set() ? "" : " (unset)");
}

int main(int argc, char* argv[]) {
    using opt::Option;
    using opt::Placeholder;
    using opt::Required;

    Option<opt::StringView> cmd("cmd", Placeholder("COMMAND"), Required);
    Option<Mode> mode("mode", Required);
    Option<float> timeout("timeout", Placeholder("TIMEOUT"), Required);
    Option<Unit> unit("unit", Required);
    Option<bool> quiet("quiet", false);
    Option<bool> stop_on_error("stop_on_error", false);
    Option<float> until("until", Placeholder("TIME"), 0.0f);
    Option<std::uintmax_t> times("times", 0);

    if (!opt::parse({ cmd, mode, timeout, unit },
                    { quiet, stop_on_error, until, times },
                    argv, argv + argc))
        return -1;

    if (!cmd.is_set() || !mode.is_set() || !timeout.is_set() || !unit.is_set()) {
        std::cerr << "error: required options are not set" << std::endl;
        opt::usage(argv[0],
                   { cmd, mode, timeout, unit },
                   { quiet, stop_on_error, until, times });
        // return -1;
    }

    std::cout << std::boolalpha << '\n'
              << "           cmd: " << cmd           << '\n'
              << "          mode: " << mode          << '\n'
              << "       timeout: " << timeout       << '\n'
              << "          unit: " << unit          << '\n'
              << "         quiet: " << quiet         << '\n'
              << " stop_on_error: " << stop_on_error << '\n'
              << "         until: " << until         << '\n'
              << "         times: " << times         << '\n'
              << std::endl;

    return 0;
}
