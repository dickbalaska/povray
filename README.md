Dick Balaska's POV-Ray repo clone
=================================

There are two active branches in this repo, neither of which is master,
so github thinks not much is happening here.  The branches are:

- [network](#network)
- [qtpovray](#qtpovray)

network
--------------------------------------

This branch replaces the POV-Ray frontend with websockets.
povrayws and povrayws.exe then becomes a service to the POV-Ray IDE
[qtpov](http://www.buckosoft.com/qtpov).
Instructions [to build](http://www.buckosoft.com/qtpov/developer/) are on the [BuckoSoft](http://www.buckosoft.com/) web site.
Builds, tested, and runs on Linux and Windows.

qtpovray
--------------------------------------

This branch replaces autoconf with the qmake build system.
It also integrates the [qtpov](http://www.buckosoft.com/qtpov) IDE
and builds a single monolithic application to edit and render your POV-Ray SDL.
Although this should run on Windows, it is currently only built and tested on Linux (Ubuntu 16, Ubuntu 18, Kubuntu 18, Mint 18.3, Slackware 14.2).
Debian derivitives can install [from the PPA](https://launchpad.net/~qtpovray/+archive/ubuntu/qtpovray).

    git clone https://github.com/dickbalaska/qtpovray.git
    cd qtpovray
    git checkout qtpovray
    qmake -r
    make -j4
    qt/gui/qtpovray        # run the executable
