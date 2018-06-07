Dick Balaska's POV-Ray repo clone
=================================

There are two active branches in this repo, neither of which is master,
so it looks like not much is happening here.  The branches are:

- [network](#network)
- [qmake](#qmake)

network
--------------------------------------

This branch replaces the POV-Ray frontend with websockets.
povrayws and povrayws.exe then become a service to the POV-Ray IDE
[qtpov](http://www.buckosoft.com/qtpov)
Instructions to build are on the [BuckoSoft](http://www.buckosoft.com/qtpov/developer/) web site.
Builds, tested, and runs on Linux and Windows.

qmake
--------------------------------------

This branch replaces autoconf with the qmake build system.
It also integrates the [qtpov](http://www.buckosoft.com/qtpov) IDE
and builds a single application to edit and render your POV-Ray SDL.
Although this should run on Windows, it is currently only built and tested on Linux (Ubuntu 16).

git clone https://github.com/dickbalaska/povray.git
cd povray
git checkout qmake
qmake
make -j4
qt/gui/qtpovray        # run the executable
