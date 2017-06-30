package velox.model;

import java.lang.String;

public class Metadata {
  public Metadata(String _name, long _hashKey, long _size, int _numBlock, int _type, int _replica, BlockMetadata[] _blocks) {
    name = _name;
    hashKey = _hashKey;
    size = _size;
    numBlock = _numBlock;
    type = _type;
    replica = _replica;

    blocks = new BlockMetadata[_blocks.length];
    for(int i=0; i<_blocks.length; i++)
      blocks[i] = new BlockMetadata(_blocks[i]);
  };

  public String name;
  public long hashKey;
  public long size;
  public int numBlock;
  public int type;
  public int replica;
  public BlockMetadata[] blocks;
}
