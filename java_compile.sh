#!/bin/bash

JNI_CLASSES=( VeloxDFS VDFS )

JAVA_SOURCE_PATH=./src/java/velox
JAVA_BUILD_PATH=./build/java

if [ ! -d "$JAVA_BUILD_PATH" ]; then
  mkdir $JAVA_BUILD_PATH
fi

LIB_PATH=./build/lib

JNI_PATH=$JAVA_SOURCE_PATH/../jni
JNI_SOURCE_NAMES=( velox_DFS.cc velox_VDFS.cc )
JNI_OUTPUT_NAMES=( libdfs_jni.so libvdfs_jni.so )

echo "COMPILING JAVA sources in \`$JAVA_BUILD_PATH\`..";
MODEL_SOURCES=( Metadata.java )
javac -d $JAVA_BUILD_PATH ${MODEL_SOURCES[@]/#/$JAVA_SOURCE_PATH\/model/};

SOURCES=( ${JNI_CLASSES[@]/%/.java} )
javac -d $JAVA_BUILD_PATH ${SOURCES[@]/#/$JAVA_SOURCE_PATH/};

echo "CREATING jni header files in \`$JNI_PATH\`..";
javah -jni -d $JNI_PATH ${JNI_CLASSES[@]/#/velox.};

echo "CREATING jni libraries in \`$LIB_PATH\`..";
i=0
for source_file in ${JNI_SOURCE_NAMES[*]}; do
  echo ${JNI_OUTPUT_NAMES[$i]}
  g++ --std=c++14 -shared -o $LIB_PATH/${JNI_OUTPUT_NAMES[$i]} -fPIC -L/usr/local/lib -I/opt/java/jdk1.7.0_51/include/linux -I/opt/java/jdk1.7.0_51/include $JNI_PATH/$source_file -lc -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -lvdfs;
  let i=i+1
done
