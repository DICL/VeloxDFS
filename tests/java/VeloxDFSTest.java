import java.lang.String;
import java.lang.Thread;
import java.nio.charset.Charset;
import java.util.Arrays;

import com.dicl.velox.VeloxDFS;
import com.dicl.velox.model.Metadata;
import com.dicl.velox.model.BlockMetadata;

class VeloxDFSTest {
  public static void main(String[] args) throws InterruptedException {
    System.out.println("Starting VeloxDFS function test..");

    VeloxDFS vdfs = new VeloxDFS();

    String targetName = "hello.txt";
    System.out.println("target file is " + targetName);

    long fd = vdfs.open(targetName);
    if(vdfs.isOpen(fd)) {
      System.out.println("open..       \tOK!");
      System.out.println("isOpen..     \tOK!");
    }  

    if(vdfs.exists(targetName))
      System.out.println("File exists");

    Metadata[] list = vdfs.list(false, "/");
    for(int i=0; i<list.length; i++)
      System.out.println(list[i].name);

    System.out.println("Try getMetadata");
    Metadata metaData = vdfs.getMetadata(fd, (byte)3);

    if(targetName.equals(metaData.name)) {
      System.out.println("getMetadata..\tOK!");
      System.out.println("Printing BlockHosts");
      for(BlockMetadata bdata : metaData.blocks) {
        System.out.println("Block " + bdata.index);
        System.out.println("Name: " + bdata.name);
        System.out.println("Host: " + bdata.host);
        System.out.println("FileName: " + bdata.fileName);
        System.out.println("Size: " + bdata.size);
      }
    }

    String hello = "Hello, world!";
    byte[] blockBuffer = hello.getBytes(Charset.forName("UTF-8"));

    long written = 0;
    for(int i=0; i<6; i++) {
      System.out.println(i + "th writing..");
      //written += vdfs.write(fd, (long)(i * blockBuffer.length) + metaData.size, blockBuffer, 0,  blockBuffer.length);
      written += vdfs.write(fd, (long)(i * 3) + metaData.size, blockBuffer, 7,  3, 2);
      Thread.sleep(1000);
    }


    byte[] buff = new byte[5];
    long read = vdfs.read(fd, 1, buff, 0, 5);
    System.out.println("read: " +  read);
    if(read == 5) {
      String expectedResult = "ello,";
      String result = new String(buff, Charset.forName("UTF-8"));
      if(expectedResult.equals(result))
        System.out.println("read..       \tOK!");
      else
        System.out.println("read..       \tFAIL!:\n\tExpected result: \"" + expectedResult + "\"\n\tActual result: \"" + result + "\"");
    }
    System.out.println(new String(buff, Charset.forName("UTF-8")));
    for(int i=0; i<5; i++) {
      System.out.println(i + ": " + String.valueOf(buff[i]));
    }
    /*
    if(written >= blockBuffer.length * 1001) {
      String expectedResult = hello;
      byte[] readBuff = new byte[15];
      vdfs.read(fd, readBuff, 0, 15);
      String result = new String(readBuff, Charset.forName("UTF-8"));
      if(expectedResult.equals(result))
        System.out.println("write..      \tOK!");
      else
        System.out.println("write..      \tFAIL!:\n\tExpected result: \"" + expectedResult + "\"\n\tActual result: \"" + result + "\"(" + result.length() + ")");
    }
    */

    vdfs.close(fd);
    if(!vdfs.isOpen(fd)) {
      System.out.println("close..       \tOK!");
    }  

    vdfs.rename(targetName, targetName + "_renamed");

    //vdfs.remove(targetName);

    vdfs.destroy();
  }
}
