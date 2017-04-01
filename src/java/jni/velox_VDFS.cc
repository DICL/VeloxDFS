#include "velox_VDFS.h"

#include <eclipsedfs/vdfs.hh>
#include <eclipsedfs/model/metadata.hh>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

velox::vdfs* get_vdfs(JNIEnv* env, jobject obj) {
  jclass vdfs_c = env->GetObjectClass(obj);
  jmethodID get_vdfs = env->GetMethodID(vdfs_c, "getVDFS", "()J");
  if(get_vdfs == nullptr) return nullptr;
  
  jlong vdfs_ptr = env->CallLongMethod(obj, get_vdfs);
  return (vdfs_ptr == 0) ? nullptr : reinterpret_cast<velox::vdfs*>(vdfs_ptr);
}

/*
 * Class:     velox_VDFS
 * Method:    constructVDFS
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_velox_VDFS_constructVDFS
  (JNIEnv* env, jobject obj) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return reinterpret_cast<jlong>(((vdfs == nullptr) ? new velox::vdfs() : vdfs));
}

/*
 * Class:     velox_VDFS
 * Method:    destructVDFS
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VDFS_destructVDFS
  (JNIEnv* env, jobject obj) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  if(vdfs != nullptr) delete vdfs;
}

/*
 * Class:     velox_VDFS
 * Method:    open
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_velox_VDFS_open
  (JNIEnv* env, jobject obj, jstring str) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  const char* file_name = env->GetStringUTFChars(str, 0);
  jlong fid = (jlong)vdfs->open_file(std::string(file_name));
  env->ReleaseStringUTFChars(str, file_name);

  return fid;
}

/*
 * Class:     velox_VDFS
 * Method:    close
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VDFS_close
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return (jboolean)vdfs->close(fid);
}

/*
 * Class:     velox_VDFS
 * Method:    isOpen
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VDFS_isOpen
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return (jboolean)vdfs->is_open(fid);
}

/*
 * Class:     velox_VDFS
 * Method:    write
 * Signature: (J[BII)J
 */
JNIEXPORT jlong JNICALL Java_velox_VDFS_write
  (JNIEnv* env, jobject obj, jlong fid, jbyteArray buf, jlong off, jlong len) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  char buffer[len];
  
  env->GetByteArrayRegion(buf, 0, (jsize)len, (jbyte*)buffer);

  return vdfs->write((long)fid, buffer, (uint32_t)off, (uint32_t)len);
}

/*
 * Class:     velox_VDFS
 * Method:    read
 * Signature: (J[BII)J
 */
JNIEXPORT jlong JNICALL Java_velox_VDFS_read
  (JNIEnv* env, jobject obj, jlong fid, jbyteArray buf, jlong off, jlong len) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  char c_buf[len];

  uint32_t ret = vdfs->read((long)fid, c_buf, (uint32_t)off, (uint32_t)len);

  env->SetByteArrayRegion(buf, 0, (jsize)len, (jbyte*)c_buf);

  return ret;
}

/*
 * Class:     velox_VDFS
 * Method:    getMetadata
 * Signature: (J)Lvelox/model/Metadata;
 */
JNIEXPORT jobject JNICALL Java_velox_VDFS_getMetadata
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  velox::model::metadata md = vdfs->get_metadata((long)fid);

  jclass MetadataClass = env->FindClass("Lvelox/model/Metadata;");
  jmethodID init = env->GetMethodID(MetadataClass, "<init>", "(Ljava/lang/String;JJIII)V");

  return env->NewObject(MetadataClass, init,
    env->NewStringUTF(md.name.c_str()), md.hash_key, md.size, md.num_block, md.type, md.replica
  );
}

#ifdef __cplusplus
}
#endif
