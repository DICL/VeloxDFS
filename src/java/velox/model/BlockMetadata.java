package com.dicl.velox.model;

import java.lang.String;

public class BlockMetadata {
  public String name;
  public String host;
  public int index;
  public String fileName;
  public long size;
  public long numChunks; 
  public BlockMetadata[] chunks = null;

  public BlockMetadata() { }

  public BlockMetadata(BlockMetadata that) {
    this.name = that.name;
    this.host = that.host;
    this.index = that.index;
    this.fileName = that.fileName;
    this.size = that.size;
    this.numChunks = that.numChunks;
    if (that.chunks != null) {
        this.chunks = that.chunks.clone();
    }
  }
}
