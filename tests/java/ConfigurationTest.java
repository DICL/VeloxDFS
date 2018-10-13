import com.dicl.velox.Configuration;

public class ConfigurationTest {
  public static void main(String[] args) {
    Configuration conf = new Configuration("/home/deukyeon/.eclipse.json");
   
    System.out.println(conf.blockSize());
  }
}
