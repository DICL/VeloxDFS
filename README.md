[![Build Status](https://travis-ci.org/DICL/EclipseDFS.svg?branch=master)](https://travis-ci.org/DICL/EclipseDFS)
[![Slack room](https://img.shields.io/badge/slack-join-pink.svg)](https://dicl.slack.com/messages/general/)
BRIEFING
========

EclipseDFS is a decentralized distributed file system based on Chord DHT. 

Key features of current EclipseDFS include:
 - There's no central directory service such as HDFS NameNode. 
 - The decentralized file metadata are managed by SQLite in each data node. 
 - It is fault tolerant. (Each block is replicated in successor and decessor by default)
 
Current EclipseDFS does not support update operation as in HDFS, but we are working on it.

USAGE
=====
EclipseDFS default launcher is not included in this repository, if you want to use it you can find it [here][eclipsed].

The reason to not to include the launcher inside the package is to let the user to chose any launcher, options are:
 - systemd/init.d
 - puppet/chef/salt
 
Once the system is running, you can interact with EclipseDFS with the following commands:
```
 $ dfs ls|put|get|rm|format|show
```

COMPILING & INSTALLING
=====================

For single user installation for developers
-------------------------------------------

    $ mkdir -p local_eclipse/{tmp,sandbox}                 # Create a sandbox directories
    $ cd local_eclipse                                     # enter in the directory
    $ git clone git@github.com:DICL/Eclipse.git            # Clone the project from github
    $ sh autogen.sh                                        # Generate configure script 
    $ cd ../tmp                                            # Go to building folder
    $ sh ../Eclipse/configure --prefix=`pwd`/../sandbox    # Generate Makefile

    ### This last command will be needed whenever you want to recompile the source
    $ make [-j#] install                                   # Compile & install add -j flag to speed up

Now edit in your **~/.bashrc** or **~/.profile**:

    export PATH="/home/*..PATH/To/eclipse/..*/sandbox/bin":$PATH
    export LIBRARY_PATH="/home/*..PATH/To/eclipse/..*/sandbox/lib"
    export C_INCLUDE_PATH="/home/*..PATH/To/eclipse/..*/sandbox/include"
    export MANPATH=`manpath`:/home*..PATH/To/eclipse/..*/sandbox/share/man

For the configuration refer to the manpage:

    $ man eclipsefs

AUTHOR
======

 - __AUTHOR:__ [Vicente Adolfo Bolea Sanchez] [vicente]
 - __AUTHOR:__ [MooHyeon Nam] [mh]
 - __AUTHOR:__ [WonBae Kim] [wb]
 - __AUTHOR:__ [KiBeom Jin] [kb]
 - __INSTITUTION:__ [DICL laboratory] [dicl] at [UNIST]

<!-- Links -->
[vicente]:  https://github.com/vicentebolea
[ym]:       https://github.com/youngmoon01
[dicl]:     http://dicl.unist.ac.kr
[mh]:       https://github.com/nammh 
[wb]:       https://github.com/zwigul
[kb]:       https://github.com/kbjin
[eclipsed]: https://github.com/DICL/eclipsed
