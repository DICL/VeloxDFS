package com.dicl.velox;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Iterator;

public class Configuration {
    private JSONObject conf;

    public Configuration(String path) {
      // Parse a JSON file, eclispe.json
      JSONParser parser = new JSONParser();

      try {
        conf = (JSONObject)parser.parse(new FileReader(path));
      } catch (FileNotFoundException e) {
        e.printStackTrace();
      } catch (IOException e) {
        e.printStackTrace();
      } catch (ParseException e) {
        e.printStackTrace();
      }
    }

    public long blockSize() {
      return (Long)((JSONObject)conf.get("filesystem")).get("block");
    }

    public long numOfReplications() {
      return (Long)((JSONObject)conf.get("filesystem")).get("replica");
    }

    public String storagePath() {
      return (String)((JSONObject)conf.get("path")).get("scratch");
    }
}
