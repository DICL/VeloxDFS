//package org.dicl.velox.native;
package com.dicl.velox;

import com.dicl.velox.model.Metadata;

public class VeloxDFS {
  static {
    System.loadLibrary("veloxdfs-jni");
  }

  /* VDFS Object Management */
  private long mVeloxDFS = 0;

  private native long constructVeloxDFS(String mr_job_id, long tid, boolean initializer);
  private native void destructVeloxDFS();

  public VeloxDFS(String mr_job_id, long tid, boolean initializer) {
    mVeloxDFS = this.constructVeloxDFS(mr_job_id, tid, initializer);
  }

  public void destroy() {
    this.destructVeloxDFS();
  }

  public long getVeloxDFS() { return mVeloxDFS; }

  /* Native Functions for Operations */
  public native long open(String name);
  public native boolean close(long fid);
  public native boolean isOpen(long fid);

  public native long write(long fid, long pos, byte[] buf, long off, long len);
  public native long write(long fid, long pos, byte[] buf, long off, long len, long blockSize);
  public native long read(long fid, long pos, byte[] buf, long off, long len);
  //public native long readChunk(String chunk_name, String host, byte[] buf, long boff, long off, long len, long lbm_id);
  public native int readChunk(byte[] buf, int boff);

  public native Metadata getMetadata(long fid, byte type);

  public native boolean remove(String name);

  public native boolean exists(String name);

  public native Metadata[] list(boolean all, String name);

  public native boolean rename(String src, String dst);

  public native void write_file(String file_name, String buf, long len);

}
