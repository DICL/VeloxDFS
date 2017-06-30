#!/bin/bash

JNI_CLASSES=( VeloxDFS )
GENERAL_CLASSES=( Configuration )

JAVA_SOURCE_PATH=./src/java/velox
JAVA_BUILD_PATH=./build/java

if [ ! -d "$JAVA_BUILD_PATH" ]; then
  mkdir $JAVA_BUILD_PATH
fi

LIB_PATH=./build/lib

JNI_PATH=$JAVA_SOURCE_PATH/../jni
JNI_SOURCE_NAMES=( velox_VeloxDFS.cc )
JNI_OUTPUT_NAMES=( libveloxdfs-jni.so )

echo "COMPILING JAVA sources in \`$JAVA_BUILD_PATH\`..";
MODEL_SOURCES=( Metadata.java BlockMetadata.java )
javac -d $JAVA_BUILD_PATH ${MODEL_SOURCES[@]/#/$JAVA_SOURCE_PATH\/model/};

SOURCES=( ${JNI_CLASSES[@]/%/.java} ${GENERAL_CLASSES[@]/%/.java})
echo "javac -cp ${CLASSPATH} -d $JAVA_BUILD_PATH ${SOURCES[@]/#/$JAVA_SOURCE_PATH/};"
javac -d $JAVA_BUILD_PATH ${SOURCES[@]/#/$JAVA_SOURCE_PATH/};

echo "CREATING jni header files in \`$JNI_PATH\`..";
javah -jni -d $JNI_PATH ${JNI_CLASSES[@]/#/velox.};

JNI_HEADER_PATH=/usr/lib/jvm/java-1.7.0-openjdk/include

echo "CREATING jni libraries in \`$LIB_PATH\`..";
i=0
for source_file in ${JNI_SOURCE_NAMES[*]}; do
  echo ${JNI_OUTPUT_NAMES[$i]}
  g++ --std=c++14 -shared \
  -o $LIB_PATH/${JNI_OUTPUT_NAMES[$i]} -fPIC \
  -L/usr/local/lib -lc -lboost_system -lboost_serialization -lboost_coroutine -lboost_thread -lboost_context -lvdfs \
  -I${JNI_HEADER_PATH} -I${JNI_HEADER_PATH}/linux \
  $JNI_PATH/$source_file;
  let i=i+1
done
