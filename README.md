# OPT

opt is a bare-bones command line argument parsing library for C++14.
It supports a non-standard key-value option style similar to the `dd` program
from coreutils. Positional arguments and boolean flags are also supported.
Numeric values can be followed by SI unit prefixes (e.g. 2.4k -> int: 2400,
float: 2.4e+3). Clever usage of positional and keyword arguments can result
in extremely readable command lines.

Supported types for options are `bool`, `std::string`, `std::string_view`,
`std::intmax_t`, `std::uintmax_t`, `float`, `double`. Composite types
`std::complex<T>`, `std::array<T, N>`, `std::vector<T>` are also supported,
where T is one of the supported types. Additional types can be supported
by specializing the `parse` method of the `Option` class.

```c++
template<>
bool opt::Object<T>::parse(opt::StringView arg, std::ostream& err) {
    // arg is trimmed (no whitespace before and after content)

    return true; // when the option is not set (e.g. when arg is empty)

    error(err) << "report errors" << std::endl;
    return false; // on error

    value_ = /* parse value from arg */;

    set(); // option correctly set, value is valid
    return true; // no error
}
```

To parse command line arguments, call the `opt::parse` function.
The function returns `false` on error, `true` on success.

```c++
bool opt::parse({ /* initializer-list of positional options */ },
                { /* initializer-list of keyword-only options */ },
                /* optional argument: reference to a
                   std::vector<opt::StringView> which
                   will contain ignored arguments */,
                argv, argv + argc,
                /* optional argument: reference to std::ostream
                   for error reporting. Default value is std::cerr */);
```

When the `help` flag is present on the command line, the `parse` function
prints a simple usage message and returns false. The message can also
be printed with the `opt::usage` function.

```c++
void opt::usage(opt::StringView program_name,
                { /* initializer-list of positional options */ },
                { /* initializer-list of keyword-only options */ },
                /* optional argument: reference to std::ostream
                   for usage message output. Default value is std::cerr */);
```

## Example

```c++
// run.cpp
#include <opt.hpp>

enum Mode {
    OneShot,
    Repeat,
};

enum Unit {
    Second,
    Minute,
    Hour,
};

int main(int argc, char* argv[]) {
    using opt::Option;
    using opt::EnumOption;
    using opt::Placeholder;
    using opt::Required;

    Option<opt::StringView> cmd("cmd", Placeholder("COMMAND"), Required);
    EnumOption<Mode> mode("mode", Required, {
        { "oneshot", OneShot },
        { "after", OneShot },
        { "repeat", Repeat },
        { "every", Repeat }
    });
    Option<float> timeout("timeout", Placeholder("TIMEOUT"), Required);
    EnumOption<Unit>("unit", Required, {
        { "seconds", Second },
        { "sec", Second },
        { "s", Second },
        { "minutes", Minute },
        { "m", Minute },
        { "hours", Hour },
        { "hr", Hour },
        { "h", Hour }
    });
    Option<bool> quiet("quiet", false);
    Option<bool> stop_on_error("stop_on_error", false);
    Option<float> until("until", Placeholder("TIME"), 0.0f);
    Option<std::uintmax_t> times("times", 0);

    if (!opt::parse({ cmd, mode, timeout, unit },
                    { quiet, stop_on_error, until, times },
                    argv, argv + argc))
        return -1;

    /* ... */

    return 0;
}
```

Compile and run:

```
# g++ -std=c++14 -o run run.cpp opt/opt.cpp -Iopt

# ./run help
Usage: ./run [help] [cmd=]COMMAND [mode=](after|every|oneshot|repeat)
  [timeout=]TIMEOUT [unit=](h|hours|hr|m|minutes|s|sec|seconds) [quiet]
  [stop_on_error] [until=TIME] [times=INT]

# ./run "echo hello" every 500m sec times=10
... if implemented would print 'hello' every 500ms for 10 times ...
```
