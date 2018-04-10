#!/bin/bash

if [[ ! "$VELOX_HOME" ]]; then
  VELOX_HOME=`pwd`
fi

JNI_CLASSES=( VeloxDFS )
GENERAL_CLASSES=( Configuration )

JAVA_SOURCE_PATH=./src/java/velox
JAVA_BUILD_PATH=$VELOX_HOME/build/java/

JAVA_PACKAGE=com.dicl.velox

if [ ! -d "$JAVA_BUILD_PATH" ]; then
  mkdir -p $JAVA_BUILD_PATH
fi

LIB_PATH=$VELOX_HOME/build/lib

JNI_PATH=$JAVA_SOURCE_PATH/../jni
JNI_SOURCE_NAMES=( VeloxDFS.cc )
JNI_OUTPUT_NAMES=( libveloxdfs-jni.so )
echo $JAVA_SOURCE_PATH

echo "COMPILING JAVA sources in \`$JAVA_BUILD_PATH\`..";
MODEL_SOURCES=( Metadata.java BlockMetadata.java )
javac -d $JAVA_BUILD_PATH ${MODEL_SOURCES[@]/#/$JAVA_SOURCE_PATH\/model/};

SOURCES=( ${JNI_CLASSES[@]/%/.java} ${GENERAL_CLASSES[@]/%/.java})
echo "javac -cp ${CLASSPATH} -d $JAVA_BUILD_PATH ${SOURCES[@]/#/$JAVA_SOURCE_PATH/};"
javac -d $JAVA_BUILD_PATH ${SOURCES[@]/#/$JAVA_SOURCE_PATH/};

echo "CREATING jni header files in \`$JNI_PATH\`..";
javah -jni -d $JNI_PATH ${JNI_CLASSES[@]/#/${JAVA_PACKAGE}.};

if [[ ! "$JAVA_HOME" ]]; then
  JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.144-0.b01.el7_4.x86_64
fi

JNI_HEADER_PATH=$JAVA_HOME/include
echo $JNI_HEADER_PATH

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

cd $JAVA_BUILD_PATH;

jar cf veloxdfs.jar com;

cd -;
