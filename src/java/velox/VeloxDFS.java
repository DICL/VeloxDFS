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
  public native void loadSettings();
  public native int put(String[] fileNames);
  public native int get(String[] fileNames);
  public native int cat(String[] fileNames);
  public native int ls(String[] fileNames);
  public native int rm(String[] fileNames);
  public native int format();
  public native int show(String[] fileNames);
  public native int pget(String[] fileNames);
  public native int update(String[] fileNames);
  public native int append(String[] fileNames);
  public native boolean fileExistsLocal(String fileName);
  public native boolean exists(String fileName);
  public native boolean touch(String fileName);
  public native int pushBack(String[] fileNames);
  public native String load(String fileName);
  public native int write(String fileName, char[] buf, int off, int len); 
  public native int read(String fileName, char[] buf, int off, int len); 
  public native Metadata getMetadata(String fileName);
}
