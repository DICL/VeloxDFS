package velox.model;

import java.lang.String;

public class BlockMetadata {
  public String name;
  public String host;
  public int index;
  public String fileName;
  public long size;

  public BlockMetadata(BlockMetadata that) {
    this.name = that.name;
    this.host = that.host;
    this.index = that.index;
    this.fileName = that.fileName;
    this.size = that.size;
  }
}
