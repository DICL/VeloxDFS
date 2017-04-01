package velox;

import velox.model.Metadata;

public class VDFS {
  static {
    System.loadLibrary("vdfs_jni");
  }

  /* VDFS Object Management */
  private long mVDFS = 0;

  private native long constructVDFS();
  private native void destructVDFS();

  public VDFS() {
    mVDFS = this.constructVDFS();
  }

  public void destroy() {
    this.destructVDFS();
  }

  public long getVDFS() { return mVDFS; }

  /* Native Functions for Operations */
  public native long open(String name);
  public native boolean close(long fid);
  public native boolean isOpen(long fid);

  public native long write(long fid, byte[] buf, long off, long len);
  public native long read(long fid, byte[] buf, long off, long len);

  public native Metadata getMetadata(long fid);
}
