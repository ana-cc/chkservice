## cmake, catch c++ boilerplate

[![Donate](https://img.shields.io/badge/donate-3$-green.svg?style=flat-square)](https://www.linuxenko.pro/donate.html#?amount=3)
[![Build Status](https://img.shields.io/travis/linuxenko/cccpp-boilerplate.svg?style=flat-square)](https://travis-ci.org/linuxenko/cccpp-boilerplate)

Most simple and robust c++ boilerplate for cmake with catch tests.

### Why ?

There is alot of broken, misconfigured boilerplates. The worst part is dependencies they are trying to handle in a very wrong way.
It is because i'd create another one for my needs and share it with you.

### Installation

1. Clone repository with:

```
git clone https://github.com/linuxenko/cccpp-boilerplate.git
```

2. Dependencies ! Haha, i'm sure you guess that you should install `cmake` and `catch` on your system :)

```
sudo apt install g++5 cmake catch
```

### Make the project

Lets build boilerplate project

```
> mkdir build
> cd build
> cmake ../
> make
```

### Make tests

```
> make Test
```

### Automatically rebuild project and run tests

There is many different options to watch filesystem changes, i prefer tools from repository such as `rerun`.
To watch, build and run tests for every file change:

```
> cd build
> rerun -d ../src -p "**/*.cpp" "make ; make Test"
```

### License

Unlicensed, free for everyone :) 
