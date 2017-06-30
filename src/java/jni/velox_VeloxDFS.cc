#include "velox_VeloxDFS.h"

#include <eclipsedfs/vdfs.hh>
#include <eclipsedfs/model/metadata.hh>
#include <eclipsedfs/model/block_metadata.hh>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

velox::vdfs* get_vdfs(JNIEnv* env, jobject obj) {
  jclass vdfs_c = env->GetObjectClass(obj);
  jmethodID get_vdfs = env->GetMethodID(vdfs_c, "getVeloxDFS", "()J");
  if(get_vdfs == nullptr) return nullptr;
  
  jlong vdfs_ptr = env->CallLongMethod(obj, get_vdfs);
  return (vdfs_ptr == 0) ? nullptr : reinterpret_cast<velox::vdfs*>(vdfs_ptr);
}

/*
 * Class:     velox_VeloxDFS
 * Method:    constructVeloxDFS
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_constructVeloxDFS
  (JNIEnv* env, jobject obj) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return reinterpret_cast<jlong>(((vdfs == nullptr) ? new velox::vdfs() : vdfs));
}

/*
 * Class:     velox_VeloxDFS
 * Method:    destructVeloxDFS
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_velox_VeloxDFS_destructVeloxDFS
  (JNIEnv* env, jobject obj) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  if(vdfs != nullptr) delete vdfs;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    open
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_open
  (JNIEnv* env, jobject obj, jstring str) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  const char* file_name = env->GetStringUTFChars(str, 0);
  jlong fid = (jlong)vdfs->open_file(std::string(file_name));
  env->ReleaseStringUTFChars(str, file_name);

  return fid;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    close
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_close
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return (jboolean)vdfs->close(fid);
}

/*
 * Class:     velox_VeloxDFS
 * Method:    isOpen
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL Java_velox_VeloxDFS_isOpen
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);
  return (jboolean)vdfs->is_open(fid);
}

/*
 * Class:     velox_VeloxDFS
 * Method:    write
 * Signature: (JJ[BJJ)J
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_write
  (JNIEnv* env, jobject obj, jlong fid, jlong pos, jbyteArray buf, jlong off, jlong len) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  char buffer[len];
  
  env->GetByteArrayRegion(buf, off, (jsize)len, reinterpret_cast<jbyte*>(buffer));

  // TODO: Copy buffer data from given buf, it might be slow.
  // If you don't want to copy data, set the second parameter to JNI_FALSE
  // jboolean* isCopy = JNI_FALSE;
  //jbyte* buffer = env->GetByteArrayElements(buf, NULL);

  jlong ret = vdfs->write((long)fid, buffer, (uint64_t)pos, (uint64_t)len);

  //env->ReleaseByteArrayElements(buf, buffer, JNI_ABORT);

  return ret;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    read
 * Signature: (JJ[BJJ)J
 * fid : File descriptor
 * pos : position to read in a file
 * buf : buffer to store read data
 * off : offset to read in the buffer
 * len : length to read
 */
JNIEXPORT jlong JNICALL Java_velox_VeloxDFS_read
  (JNIEnv* env, jobject obj, jlong fid, jlong pos, jbyteArray buf, jlong off, jlong len) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  char c_buf[len];

  uint32_t ret = vdfs->read((long)fid, c_buf, (uint64_t)pos, (uint64_t)len);

  // TODO: The second parameter is start offset, 
  // but I am not sure it is a position for buf.
  env->SetByteArrayRegion(buf, off, (jsize)len, (jbyte*)c_buf);

  return ret;
}

/*
 * Class:     velox_VeloxDFS
 * Method:    getMetadata
 * Signature: (J)Lvelox/model/Metadata;
 */
JNIEXPORT jobject JNICALL Java_velox_VeloxDFS_getMetadata
  (JNIEnv* env, jobject obj, jlong fid) {
  velox::vdfs* vdfs = get_vdfs(env, obj);

  velox::model::metadata md = vdfs->get_metadata((long)fid);

  jclass velox_model_BlockMetadata = env->FindClass("Lvelox/model/BlockMetadata;");
  jobjectArray block_data = (jobjectArray)env->NewObjectArray(md.num_block, velox_model_BlockMetadata, NULL);

  jmethodID velox_model_BlockMetadata_init = env->GetMethodID(velox_model_BlockMetadata, "<init>", "()V");

  // file_name
  jfieldID fn_field_id = env->GetFieldID(velox_model_BlockMetadata, "fileName", "Ljava/lang/String;");
  jstring file_name = env->NewStringUTF(md.name.c_str());

  for(int i=0; i<md.num_block; i++) {
    jobject data = env->NewObject(velox_model_BlockMetadata, velox_model_BlockMetadata_init);

    // name
    jfieldID name_field_id = env->GetFieldID(velox_model_BlockMetadata, "name", "Ljava/lang/String;");
    jstring name = env->NewStringUTF(md.block_data[i].name.c_str());
    env->SetObjectField(data, name_field_id, name);
    env->DeleteLocalRef(name);

    // host
    jfieldID host_field_id = env->GetFieldID(velox_model_BlockMetadata, "host", "Ljava/lang/String;");
    jstring host = env->NewStringUTF(md.block_data[i].host.c_str());
    env->SetObjectField(data, host_field_id, host);
    env->DeleteLocalRef(host);

    // file_name
    env->SetObjectField(data, fn_field_id, file_name);

    // index
    jfieldID index_field_id = env->GetFieldID(velox_model_BlockMetadata, "index", "I");
    env->SetIntField(data, index_field_id, (jint)i);

    // size
    jfieldID size_field_id = env->GetFieldID(velox_model_BlockMetadata, "size", "J");
    env->SetLongField(data, size_field_id, (jlong)md.block_size[i]);

    env->SetObjectArrayElement(block_data, (jsize)i, data);
  }

  jclass MetadataClass = env->FindClass("Lvelox/model/Metadata;");
  jmethodID init = env->GetMethodID(MetadataClass, "<init>", "(Ljava/lang/String;JJIII[Lvelox/model/BlockMetadata;)V");
  jobject ret = env->NewObject(MetadataClass, init,
    file_name, md.hash_key, md.size, md.num_block, md.type, md.replica, block_data
  );
  
  env->DeleteLocalRef(file_name);
  env->DeleteLocalRef(block_data);

  return ret;
}

#ifdef __cplusplus
}
#endif
