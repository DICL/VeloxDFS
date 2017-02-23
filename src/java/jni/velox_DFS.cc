#include "velox_VeloxDFS.h"
#include <eclipsedfs/dfs.hh>
#include <eclipsedfs/model/metadata.hh>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif
/* Local Functions */ 
velox::DFS* get_dfs(JNIEnv *env, jobject obj) {
  //std::cout << "get_dfs in " << __FILE__ << std::endl;

  jclass dfs_c = env->GetObjectClass(obj);
  jmethodID get_dfs = env->GetMethodID(dfs_c, "getDFS", "()J");
  if(get_dfs == nullptr) return nullptr;
  
  jlong dfs_ptr = env->CallLongMethod(obj, get_dfs);
  return (dfs_ptr == 0) ? nullptr : reinterpret_cast<velox::DFS*>(dfs_ptr);
}

void string_array_to_vector(JNIEnv *env, jobject obj, jobjectArray inputs, std::vector<std::string> &vec_str) {
  int count = env->GetArrayLength(inputs);

  for (int i=0; i<count; i++) {
    jstring input = (jstring) (env->GetObjectArrayElement(inputs, i));
    const char *utf_chars = env->GetStringUTFChars(input, 0);
    vec_str.push_back(std::string(utf_chars));
    env->ReleaseStringUTFChars(input, utf_chars);
  }
}

/*
 * Class:     Velox_DFS
 * Method:    constructDFS
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_constructDFS
  (JNIEnv *env, jobject obj) {
  //std::cout << "constructDFS in " << __FILE__ << std::endl;
  velox::DFS *dfs = get_dfs(env, obj);
  return reinterpret_cast<jlong>(((dfs == nullptr) ? new velox::DFS() : dfs));
}

/*
 * Class:     Velox_DFS
 * Method:    destructDFS
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VeloxDFS_destructDFS
  (JNIEnv *env, jobject obj) {
  //std::cout << "destructDFS in " << __FILE__ << std::endl;
  velox::DFS *dfs = get_dfs(env, obj);
  if(dfs != nullptr) delete dfs;
}

/*
 * Class:     Velox_DFS
 * Method:    loadSettings
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VeloxDFS_loadSettings
  (JNIEnv *env, jobject obj) {
  //std::cout << "loadSettings in " << __FILE__ << std::endl;
  velox::DFS *dfs = get_dfs(env, obj);
  dfs->load_settings();
}

/*
 * Class:     Velox_DFS
 * Method:    put
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_put
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->put(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    get
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_get
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->get(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    cat
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_cat
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->cat(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    ls
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_ls
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->ls(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    rm
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_rm
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->rm(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    format
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_format
  (JNIEnv *env, jobject obj) {
  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->format());
}

/*
 * Class:     Velox_DFS
 * Method:    show
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_show
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->show(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    pget
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_pget
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->pget(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    update
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_update
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->update(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    append
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_append
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->append(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    fileExistsLocal
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_fileExistsLocal
  (JNIEnv *env, jobject obj, jstring input) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char *file_name = env->GetStringUTFChars(input, 0);
  jboolean ret = (jboolean)(dfs->file_exists_local(std::string(file_name)));
  env->ReleaseStringUTFChars(input, file_name);
  return ret;
}

/*
 * Class:     Velox_DFS
 * Method:    exists
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_exists
  (JNIEnv *env, jobject obj, jstring input) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char *file_name = env->GetStringUTFChars(input, 0);
  jboolean ret = (jboolean)(dfs->exists(std::string(file_name)));
  env->ReleaseStringUTFChars(input, file_name);
  return ret;
}

/*
 * Class:     Velox_DFS
 * Method:    touch
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_touch
  (JNIEnv *env, jobject obj, jstring input) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char *file_name = env->GetStringUTFChars(input, 0);
  jboolean ret = (jboolean)(dfs->touch(std::string(file_name)));
  env->ReleaseStringUTFChars(input, file_name);
  return ret;
}

/*
 * Class:     Velox_DFS
 * Method:    pushBack
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_pushBack
  (JNIEnv *env, jobject obj, jobjectArray inputs) {
  std::vector<std::string> file_names;
  string_array_to_vector(env, obj, inputs, file_names);

  velox::DFS *dfs = get_dfs(env, obj);
  return (jint)(dfs->push_back(file_names));
}

/*
 * Class:     Velox_DFS
 * Method:    load
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_velox_VeloxDFS_load
  (JNIEnv *env, jobject obj, jstring input) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char *file_name = env->GetStringUTFChars(input, 0);
  jstring ret = (jstring)((dfs->load(file_name)).c_str());
  env->ReleaseStringUTFChars(input, file_name);
  return ret;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    write
 * Signature: (Ljava/lang/String;[CII)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_write
  (JNIEnv *env, jobject obj, jstring input, jcharArray buf, jint off, jint len) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char* file_name = env->GetStringUTFChars(input, 0);
  char c_buf[len];
  env->GetCharArrayRegion(buf, 0, (jsize)len, (jchar*)c_buf);

  std::string file_name_str = std::string(file_name);

  int ret = dfs->write(file_name_str, c_buf, (uint32_t)off, (uint32_t)len);

  env->ReleaseStringUTFChars(input, file_name);

  return ret;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    read
 * Signature: (Ljava/lang/String;[CII)I
 */
JNIEXPORT jint JNICALL Java_velox_VeloxDFS_read
  (JNIEnv *env, jobject obj, jstring input, jcharArray buf, jint off, jint len) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char* file_name = env->GetStringUTFChars(input, 0);
  char c_buf[len];
  std::string file_name_str = std::string(file_name);

  int ret = dfs->read(file_name_str, c_buf, (uint32_t)off, (uint32_t)len);

  env->SetCharArrayRegion(buf, 0, (jsize)len, (jchar*)c_buf);

  env->ReleaseStringUTFChars(input, file_name);

  return ret;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    getMetadata
 * Signature: (Ljava/lang/String;)Lvelox/model/Metadata;
 */
JNIEXPORT jobject JNICALL Java_velox_VeloxDFS_getMetadata
  (JNIEnv* env, jobject obj, jstring input) {
  velox::DFS *dfs = get_dfs(env, obj);
  const char* file_name = env->GetStringUTFChars(input, 0);

  std::string file_name_str(file_name);

  velox::model::metadata md = dfs->get_metadata(file_name_str);

  env->ReleaseStringUTFChars(input, file_name);

  jclass MetadataClass = env->FindClass("Lvelox/model/Metadata;");
  jmethodID init = env->GetMethodID(MetadataClass, "<init>", "(Ljava/lang/String;JJIII)V");

  return env->NewObject(MetadataClass, init,
    env->NewStringUTF(md.name.c_str()), md.hash_key, md.size, md.num_block, md.type, md.replica
  );
}

#ifdef __cplusplus
}
#endif
