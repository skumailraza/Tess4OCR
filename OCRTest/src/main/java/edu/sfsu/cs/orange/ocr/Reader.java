package edu.sfsu.cs.orange.ocr;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Build;
import android.util.Log;

import junit.framework.Assert;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Array;
import java.util.*;

/**
 * Created by Kumail on 3/29/17.
 */

public class Reader {

    Map<String, ArrayList<String>> zipMap = new HashMap<String, ArrayList<String>>();
    Map stateMap = new HashMap();
    String csvFile;     //file address

    public BufferedReader myBr;

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public Reader(){
        String csvFile = "/Users/skrk/Desktop/Ali/ocfnr/addressFile.csv";
        String line = "";
        String cvsSplitBy = ",";

        try (BufferedReader br = new BufferedReader(new FileReader(csvFile))) {
            br.readLine();
            br.readLine();
            this.myBr = br;

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public Reader(String file, String splitby){
        String csvFile = "/Users/skrk/Desktop/Ali/ocfnr/addressFile.csv";
        String line = "";
        String cvsSplitBy = ",";

        try (BufferedReader br = new BufferedReader(new FileReader(csvFile))) {
            br.readLine();
            br.readLine();
            this.myBr = br;

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public void populateMaps() {

        String csvFile = "/Users/skrk/Desktop/Ali/ocfnr/addressFile.csv";
        String line = "";
        String cvsSplitBy = ",";

        ArrayList<String> temp = new ArrayList<String>();

        try (BufferedReader br = new BufferedReader(new FileReader(csvFile))) {
            br.readLine();

            while ((line = br.readLine()) != null) {
                String[] location = line.split(cvsSplitBy);

                if (zipMap.containsKey(location[2])) {
                    temp = zipMap.get(location[2]);
                    if (temp == null)
                        temp = new ArrayList();
                    temp.add(location[1]);
                } else {
                    temp = new ArrayList();
                    temp.add(location[1]);
                }
                zipMap.put(location[2], temp);


                stateMap.put(location[2], location[3]);


            }
        } catch (IOException e1) {
            e1.printStackTrace();
        }

    }
}

