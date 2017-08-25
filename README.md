## chkservice

[![Donate](https://img.shields.io/badge/donate-3$-green.svg?style=flat-square)](https://www.linuxenko.pro/donate.html#?amount=3)
[![Build Status](https://img.shields.io/travis/linuxenko/chkservice.svg?style=flat-square)](https://travis-ci.org/linuxenko/chkservice)

[![screenshot](https://raw.githubusercontent.com/linuxenko/linuxenko.github.io/master/media/chkservice/chkservice.png)](https://raw.githubusercontent.com/linuxenko/linuxenko.github.io/master/media/chkservice/chkservice.png)

> chkservice is a tool for managing systemd units in terminal.

### Installation


### Usage


### Build

Build release version.

```
git clone https://github.com/linuxenko/chkservice.git
mkdir build
cd build
cmake ../
make
```

To build debug version, `DEBUG` environment should be set

```
export DEBUG=1
cmake ....
make Test
```

### License
GNU General Public License

chkservice is a tool for managing systemd units.
more infomration at https://github.com/linuxenko/chkservice

Copyright (C) 2017 Svetlana Linuxenko <svetlana(at)linuxenko.pro>
