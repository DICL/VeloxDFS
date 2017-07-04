# Path for VeloxDFS
VELOXHOME=~/VeloxDFS
cd $VELOXHOME

# Your Install Path
INSTALL_PATH=`pwd`/build

export PATH="$INSTALL_PATH/bin:$PATH"
export LIBRARY_PATH="$INSTALL_PATH/lib:/usr/local/lib:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$INSTALL_PATH/lib:/usr/local/lib:$LD_LIBRARY_PATH"
export CPATH="$INSTALL_PATH/include:$CPATH"
export C_INCLUDE_PATH="$INSTALL_PATH/include:$C_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH="$INSTALL_PATH/include:$CPLUS_INCLUDE_PATH"
export MANPATH=$MANPATH:"$INSTALL_PATH/share/man"
export LDFLAGS="-L/usr/local/lib"
export PYTHONPATH=$INSTALL_PATH/bin
export CLASSPATH=$INSTALL_PATH/java:$VELOXHOME/lib/java/json-simple-1.1.jar:$CLASSPATH

# Come back
cd -
