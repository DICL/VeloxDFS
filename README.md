VeloxDFS {#mainpage}
======== 
<!-- @cond Remove those links for Doxygen-->
--- 
[![Build Status](https://travis-ci.org/DICL/VeloxDFS.svg?branch=master)](https://travis-ci.org/DICL/VeloxDFS)
[![Release](https://img.shields.io/github/release/DICL/VeloxDFS.svg)](https://img.shields.io/github/release/DICL/VeloxDFS.svg)
[![Slack room](https://img.shields.io/badge/slack-join-pink.svg)](https://dicl.slack.com/messages/general/)
---
<!-- @endcond -->

_VeloxDFS_ is a decentralized distributed file system based on _ChordDHT_ and _HDFS_.

This distributed file system serves as the foundation and essential component of the _Velox Big Data Framework (VBDF)_, 
however, it can be used independently of the other components such as _VeloxMR_ (MapReduce Framework).

Key features of current VeloxDFS implementation includes:
 - No central directory service such as in _HDFS_ NameNode.
 - Logical block representation where the size adapts to the current workload.
 - HADOOP API to be used instead of HDFS.

VELOX ECOSYSTEM
===============

Related projects

| Project                               | Description                  |URL                          |
| -- | -- | -- |
| VeloxMR           |  experimental MapReduce engine based on VeloxDFS | https://github.com/DICL/VeloxMR |
| eclipsed          | deployment/debugging helper script writen in RUBY| https://github.com/DICL/eclipsed|
| velox-hadoop      | VeloxDFS JAVA library for Hadoop                 | https://github.com/DICL/velox-hadoop |
| velox-deploy-ansible | ansible playbook to automatize velox/hadoop installation | https://github.com/DICL/velox-deploy-ansible |
| hadoop-etc        | Hadoop configuration files to use VeloxDFS       | https://github.com/vicentebolea/hadoop-etc |
| velox-report      | Project to benchmark and log VeloxDFS performance | https://github.com/vicentebolea/velox-report |

USAGE
=====
To deploy the system please refer to `velox` command and its following options:
    
    $ veloxd up|down|restart|status
    
The command line API has the following options:

    $ veloxdfs put|get|cat|ls|rm|format|show

The C++ API can be found at `vdfs.hh` and `DFS.hh` files, as for the JAVA API, it can be found at `src/java` directory.

COMPILING & INSTALLING
======================
<!-- @cond Remove those links for Doxygen-->
Further information can be found it in: [Installation](https://github.com/DICL/VeloxDFS/wiki/Installation)
<!-- @endcond -->

Compiling requirements
----------------------
 - C++14 support, this is: GCC >= 4.9, Clang >= 3.4, or ICC >= 16.0.
 - Boost library >= 1.56.
 - Sqlite3 library.
 - GNU Autotools (Autoconf, Automake, Libtool).
 - Unittest++ [optional].

For single user installation for developers
-------------------------------------------

    $ mkdir -p local_eclipse/{tmp,sandbox}                 # Create a sandbox directories
    $ cd local_eclipse                                     # enter in the directory
    $ git clone git@github.com:DICL/VeloxDFS.git           # Clone the project from github
    $ cd VeloxDFS
    $ sh autogen.sh                                        # Generate configure script 
    $ cd ../tmp                                            # Go to building folder
    $ sh ../VeloxDFS/configure --prefix=`pwd`/../sandbox # Check requirements and generate the Makefile

    # If you get a boost error go the FAQ section of the README

    ### This last command will be needed whenever you want to recompile the source
    $ make [-j#] install                                   # Compile & install add -j flag to speed up

Now edit in your `~/.bashrc` or `~/.profile`:

    export PATH="/home/*..PATH/To/eclipse/..*/sandbox/bin":$PATH
    export LIBRARY_PATH="/home/*..PATH/To/eclipse/..*/sandbox/lib"
    export C_INCLUDE_PATH="/home/*..PATH/To/eclipse/..*/sandbox/include"

Default settings for VELOXDFS 
-----------------------------

    "log" : {
      "type" : "LOG_LOCAL6"
      "name" : "ECLIPSE"
      "mask" : "DEBUG"
    },

    "cache" : {
      "numbin"      : 100,
      "size"        : 200000,
      "concurrency" : 1
    },

    "filesystem" : {
      "block"    : 137438953,
      "buffer"   : 512,
      "replica"  : 1
    }

<!-- @cond Remove those links for Doxygen-->
Further information can be found it in: [Conf reference](https://github.com/DICL/VeloxDFS/wiki/Configuration-file-reference)
<!-- @endcond -->

FAQ
---

- _Question_ : `configure` stops with errors related to boost library.
- _Answer_ : It probably means that you do not have boost library installed in
  the default location, in such case you should specify the boost library location.

        sh ../VeloxDFS/configure --prefix ~/sandbox --with-boost=/usr/local --with-boost-libdir=/usr/local/lib

  In this example we assume that the boost headers are in `/usr/local/include` while the library files
  are inside `/usr/local/lib`.

AUTHORS
=======

 - [Vicente Adolfo Bolea Sanchez](http://vicentebolea.me)
 - [MooHyeon Nam](https://github.com/nammh)
 - [WonBae Kim](https://github.com/zwigul)
 - [KiBeom Jin](https://github.com/kbjin)
 - [Deukyeon Hwang](https://github.com/deukyeon)
 - [Prof. Nam Beomseok](http://dicl.unist.ac.kr)
 - __INSTITUTION:__ [DICL laboratory](http://dicl.unist.ac.kr) at _UNIST_ 

<!--links-->
[eclipsed]: https://github.com/DICL/eclipsed
