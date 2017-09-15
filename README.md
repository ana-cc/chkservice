## chkservice

[![Donate](https://img.shields.io/badge/donate-3$-green.svg?style=flat-square)](https://www.linuxenko.pro/donate.html#?amount=3)

[![screenshot](https://raw.githubusercontent.com/linuxenko/linuxenko.github.io/master/media/chkservice/chkservice.png)](https://raw.githubusercontent.com/linuxenko/linuxenko.github.io/master/media/chkservice/chkservice.png)

> chkservice is a tool for managing systemd units in terminal.

### Installation

```
sudo add-apt-repository ppa:linuxenko/chkservice
sudo apt-get update
sudo apt-get install chkservice
```

Arch

```
git clone https://aur.archlinux.org/chkservice.git
cd chkservice
makepkg -si
```

Fedora
```
dnf copr enable srakitnican/default
dnf install chkservice
```
### Usage

`chkservice` require super user privileges to make changes into unit states or sysv scripts. For user it works read-only.

### Dependencies

Package dependencies:
  * libncurses5
  * libsystemd0 ( >= 222 )
  
Build dependencies:
  * pkg-config
  * libncurses5-dev
  * libsystemd-dev ( >= 222 )

### Build

Build and install debian package.

```
git clone https://github.com/linuxenko/chkservice.git
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ../
cpack

dpkg -i chkservice-x.x.x.deb
```

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

[![Build Status](https://img.shields.io/travis/linuxenko/chkservice.svg?style=flat-square)](https://travis-ci.org/linuxenko/chkservice) until travis run it on `trusty` based containers


### TODO

 - [ ] - `.login1` target support (if someone request it)

### License
GNU General Public License

chkservice is a tool for managing systemd units.
more infomration at https://github.com/linuxenko/chkservice

Copyright (C) 2017 Svetlana Linuxenko <svetlana(at)linuxenko.pro>
