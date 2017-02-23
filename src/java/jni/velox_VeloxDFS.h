/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class velox_VeloxDFS */

#ifndef _Included_velox_VeloxDFS
#define _Included_velox_VeloxDFS
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     velox_VeloxDFS
 * Method:    constructDFS
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_constructDFS
  (JNIEnv *, jobject);

/*
 * Class:     velox_VeloxDFS
 * Method:    destructDFS
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VeloxDFS_destructDFS
  (JNIEnv *, jobject);

/*
 * Class:     velox_VeloxDFS
 * Method:    loadSettings
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VeloxDFS_loadSettings
  (JNIEnv *, jobject);

/*
 * Class:     velox_VeloxDFS
 * Method:    put
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_put
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    get
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_get
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    cat
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_cat
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    ls
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_ls
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    rm
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_rm
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    format
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_format
  (JNIEnv *, jobject);

/*
 * Class:     velox_VeloxDFS
 * Method:    show
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_show
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    pget
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_pget
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    update
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_update
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    append
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_append
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    fileExistsLocal
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_fileExistsLocal
  (JNIEnv *, jobject, jstring);

/*
 * Class:     velox_VeloxDFS
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_exists
  (JNIEnv *, jobject, jstring);

/*
 * Class:     velox_VeloxDFS
 * Method:    touch
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_touch
  (JNIEnv *, jobject, jstring);

/*
 * Class:     velox_VeloxDFS
 * Method:    pushBack
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_pushBack
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     velox_VeloxDFS
 * Method:    load
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_velox_VeloxDFS_load
  (JNIEnv *, jobject, jstring);

/*
 * Class:     velox_VeloxDFS
 * Method:    write
 * Signature: (Ljava/lang/String;[CII)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_write
  (JNIEnv *, jobject, jstring, jcharArray, jint, jint);

/*
 * Class:     velox_VeloxDFS
 * Method:    read
 * Signature: (Ljava/lang/String;[CII)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_read
  (JNIEnv *, jobject, jstring, jcharArray, jint, jint);

/*
 * Class:     velox_VeloxDFS
 * Method:    getMetadata
 * Signature: (Ljava/lang/String;)Lvelox/model/Metadata;
 */
JNIEXPORT jobject JNICALL Java_velox_VeloxDFS_getMetadata
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
