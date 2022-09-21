# flan

![License](https://img.shields.io/github/license/delaitre/flan)
![Release](https://img.shields.io/github/v/release/delaitre/flan?sort=semver)
![Build](https://img.shields.io/github/workflow/status/delaitre/flan/build?logo=GitHub&label=build)
![Formatting](https://img.shields.io/github/workflow/status/delaitre/flan/clang-format?logo=GitHub&label=format)

*flan* stands for flexible log analyser, and also is a yummy treat (which you probably need if you use a tool like this).

This is a simple line based textual log analyser allowing you to define a bunch of pattern/regexp and give then a name. Then you can select on the fly which ones to apply on your input and decide if the pattern should be filtering matching lines out, or keeping them in, or if you just want to highlight the matches.

Each pattern related to filtering is applied in order until one matches, so pattern order is important (e.g. if you really need a specific pattern to be kept, it needs to be applied (and marked for keeping) before another more generic pattern (and marked for removal) also matching the same line is applied). Highligthing is applied after filtering.

Patterns are saved in a configuration file so that they are restored and ready for action next time you need to analyse a log.

## Why?

I spend a lot of time analysing line based textual log files but never really found a tool to help me for my use cases. I always fell back using vim but from one day to the other, I always needed to search for different complex patterns, sometimes I wanted to filter stuff out, sometimes to keep stuff in, sometimes to just highlight matches and so on. But I always tend to have a basic set of regexps I want to use, and then additional ones I wanted to enabled or not depending on what I was doing. Going through vim's history to select each pattern I wanted to apply got pretty tedious, and thus I wrote *flan*.

## How to build

*flan* only requires a recent C++ compiler, CMake and Qt.

```
mkdir build-flan
cd build-flan
cmake -DCMAKE_BUILD_TYPE=Release <path to flan source code>
make
```

## License

*flan* is licensed under [the MIT License](LICENSE).

