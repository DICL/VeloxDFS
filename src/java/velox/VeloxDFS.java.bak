package velox;

import java.lang.String;

import velox.model.Metadata;

public class VeloxDFS {
  static {
    System.loadLibrary("dfs_jni");
  }

  /* DFS Object Management */
  private long mDFS = 0;

  private native long constructDFS();
  private native void destructDFS();

  public VeloxDFS() {
    mDFS = this.constructDFS();
  }

  public void destroy() {
    this.destructDFS();
  }

  public long getDFS() { return mDFS; }

  /* Native Functions for Operations */
  public native int format();
  public native boolean exists(String fileName);
  public native boolean touch(String fileName);
  public native long write(String fileName, char[] buf, long off, long len); 
  public native long read(String fileName, char[] buf, long off, long len); 
  public native Metadata getMetadata(String fileName);
}
