package velox;

import velox.model.Metadata;

public class VeloxDFS {
  static {
    System.loadLibrary("veloxdfs-jni");
  }

  /* VDFS Object Management */
  private long mVeloxDFS = 0;

  private native long constructVeloxDFS();
  private native void destructVeloxDFS();

  public VeloxDFS() {
    mVeloxDFS = this.constructVeloxDFS();
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
  public native long read(long fid, long pos, byte[] buf, long off, long len);

  public native Metadata getMetadata(long fid);
}
