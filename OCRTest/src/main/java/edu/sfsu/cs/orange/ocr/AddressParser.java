package edu.sfsu.cs.orange.ocr;

import java.util.Iterator;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Kumail on 4/7/17.
 */

public class AddressParser {

    String text;            //input text
    String[] lines;         //address lines

    Reader addressReader;   //to get hashMaps
    String name;
    String company;
    String streetAddress1;
    String streetAddress2;
    String city;
    String state;
    String zip;
    String phone;


    AddressParser() {
        text = null;
        addressReader = new Reader();
        name = null;
        company = null;
        streetAddress1 = null;
        streetAddress2 = null;
        city = null;
        state = null;
        zip = null;
        phone = null;
    }

    AddressParser(String line, Reader reader) {
        text = line;
        addressReader = reader;
        name = null;
        company = null;
        streetAddress2 = null;
        streetAddress1 = null;
        city = null;
        state = null;
        zip = null;
        phone = null;
        toLines();
    }

    public void textAppend(String input) {
        text += input;
        toLines();
    }

    public void setText(String input) {
        text = input;
        toLines();
    }

    public void stripZip() {
        if (text == null)
            return;
        //Scanner scanner;
        Scanner scanner = new Scanner(lines[lines.length - 1]);
        zip = scanner.findInLine("\\d{3,5}(?:[-\\s]\\d{4})?");
        if (zip == null) {
            scanner = new Scanner(lines[lines.length - 2]);
            zip = scanner.findInLine("\\d{3,5}(?:[-\\s]\\d{4})?");
        }
//            for(int i = 0; i < lines.length; i++){
//            if(zip != null)
//                break;
//            scanner= new Scanner((lines[i]));
//            zip = scanner.findInLine("\\d{3,5}(?:[-\\s]\\d{4})?");
//
//        }
    }

    public void stripCity() {
        if (text == null)
            return;
        Scanner scanner = new Scanner(lines[lines.length - 1]);
        city = scanner.findInLine("^[A-Za-z]+(?:[\\s'-]+[A-Za-z]+)*(?=[,]|\\s+\\b)");
        //city = scanner.findInLine("^([a-zA-Z\\\\u0080-\\\\u024F]+(?:. |-| |'))*[a-zA-Z\\\\u0080-\\\\u024F]");
        if (city == null) {
            scanner = new Scanner((lines[lines.length - 2]));
            city = scanner.findInLine("^[A-Za-z]+(?:[\\s'-]+[A-Za-z]+)*(?=[,]|\\s+\\b)");
            //city = scanner.findInLine("^([a-zA-Z\\\\u0080-\\\\u024F]+(?:. |-| |'))*[a-zA-Z\\\\u0080-\\\\u024F]");

        }
//        Scanner scanner;
//        for(int i = 0; i < lines.length; i++){
//            if(city != null)
//                break;
//            scanner= new Scanner((lines[i]));
//            //city = scanner.findInLine("^((\\d{3}-|\\(\\d{3}\\)\\s?)?\\d{3}-|^\\d{3}(\\.)?\\d{3}\\3)\\d{4}$");
//            city = scanner.findInLine("^([a-zA-Z\\u0080-\\u024F]+(?:. |-| |'))*[a-zA-Z\\u0080-\\u024F]*$");
//
//        }
    }

    public int getLineNo() {
        if (lines == null) {
            return 0;

        } else {
            return lines.length - 1;
        }
    }

    public void toLines() {
        if (text == null) {
            return;
        }

        lines = text.split("\n");

    }

    public void stripState() {
        if (text == null)
            return;

//        Scanner scanner = new Scanner(lines[lines.length - 1]);
//        state = scanner.findInLine("[A-Z]{2}(?:\\s\\d{0})");
//        //state = scanner.findInLine("(");
//        if (state == null) {
//            scanner = new Scanner(lines[lines.length - 2]);
//            state = scanner.findInLine("[A-Z]{2}(?:\\s\\d{0})");
//        }

        String stateRegex = "(\\s)([A-Z]{2})(\\s)(\\d)";
        //String COregex = "(\\sc/o\\s)(.*)";

        Pattern stateP = Pattern.compile(stateRegex);
        //Pattern pattern2 = Pattern.compile(COregex);

        for (int i = 0; i < lines.length; i++) {
            Matcher matcher12 = stateP.matcher(lines[i]);

            if (matcher12.find()) {
                state = matcher12.group(2);
            } else if (matcher12.find()) {
                state = matcher12.group(2);
            }
            if (state == null)
                state = ".";
        }
    }

    public void stripPhone() {
        if (text == null)
            return;
        Scanner scanner;
        for (int i = 0; i < lines.length; i++) {
            if (phone != null)
                break;
            scanner = new Scanner((lines[i]));
            phone = scanner.findInLine("^((\\d{3}-|\\(\\d{3}\\)\\s?)?\\d{3}-|^\\d{3}(\\.)?\\d{3}\\3)\\d{4}$");

        }

    }

    public void stripCountry() {
        if (text == null)
            return;
        Scanner scanner = new Scanner(lines[lines.length - 1]);
        String end[] = lines[lines.length - 1].split("US");
        if (end == null) {
            end = lines[lines.length - 2].split("US");
        }


    }

    public void stripAttn() {
        /**** Written by Basit Bhai ****/

        if (text == null)
            return;


        String Attnregex = "^(attn\\W|attention\\W|attn\\b|attention\\b)(.*)";
        String COregex = "(\\sc/o\\s)(.*)";

        Pattern pattern1 = Pattern.compile(Attnregex);
        Pattern pattern2 = Pattern.compile(COregex);

        for (int i = 0; i < lines.length; i++) {
            Matcher matcher1 = pattern1.matcher(lines[i].toLowerCase());
            Matcher matcher2 = pattern2.matcher(lines[i].toLowerCase());
            if (matcher1.find()) {
                name = matcher1.group(2);
            } else if (matcher2.find()) {
                name = matcher2.group(2);
            }
            if (name == null)
                name = ".";
        }

        String[] Company1 = new String[2];
        //String company;
        int a = 0;
        String CompanyRegex = "\\bassociation\\b|\\bauto repair\\b|\\bbank\\b|\\bbureau\\b|\\bcli\\b|\\bclinic\\b|\\bclub\\b|\\bco\\b|\\bconsultant\\b|\\bcorp\\b|\\bcorporation\\b|\\bdepartment\\b|\\bdevelopment\\b|\\bdivision\\b|\\bedu\\b|\\belectronics\\b|\\belectronics\\b|\\bestate\\b|\\bfarm\\b|\\bhealth\\b|\\bhospital\\b|\\binc\\b|\\binstitute\\b|\\binsurance\\b|\\bjis\\b|\\blaw center\\b|\\bllc\\b|\\bllp\\b|\\bltd\\b|\\bmanagment\\b|\\bmanufacturing\\b|\\bmgmt\\b|\\bmortgage\\b|\\bnews\\b|\\bparent act\\b|\\bpc\\b|\\bpllc\\b|\\bproperty\\b|\\bresturant\\b|\\bschool\\b|\\bservice\\b|\\bstation\\b|\\bsvc\\b|\\btax\\b|\\btaxes\\b|\\btech\\b|\\b association\\W|\\bauto repair\\W|\\bbank\\W|\\bbureau\\W|\\bcli\\W|\\bclinic\\W|\\bclub\\W|\\bco\\W|\\bconsultant\\W|\\bcorp\\W|\\bcorporation\\W|\\bdepartment\\W|\\bdevelopment\\W|\\ddivision\\W|\\bedu\\W|\\belectronics\\W|\\bestate\\W|\\bfarm\\W|\\bhealth\\W|\\bhospital\\W|\\binc\\W|\\binstitute\\W|\\binsurance\\W|\\bjis\\W|\\blaw center\\W|\\bllc\\W|\\bllp\\W|\\bltd\\W|\\bmanagment\\W|\\bmanufacturing\\W|\\bmgmt\\W|\\bmortgage\\W|\\bnews\\W|\\bparent act\\W|\\bpc\\W|\\bpllc\\W|\\bproperty\\W|\\bresturant\\W|\\bschool\\W|\\bservice\\W|\\bstation\\W|\\bsvc\\W|\\btax\\W|\\btaxes\\W|\\btech\\W|\\bmaterials\\b|\\bmaterials\\W";
        Pattern Companypattern = Pattern.compile(CompanyRegex);

        for (int i = 0; i < lines.length; i++) {
            Matcher Companymatcher = Companypattern.matcher(lines[i].toLowerCase());
            if (Companymatcher.find()) {
                Company1[a] = lines[i];
                a = a + 1;
            }
        }
        int b = a;
        if (a > 1 && name.length() == 0) {
            name = Company1[0];
            company = Company1[1];
        } else if (b == 1 && name.length() == 0) {
            company = Company1[0];
            name = "\t";
        } else {
            company = Company1[0];
        }


        if (name != null)
            name = name.toUpperCase();

    }

    public void stripAddress() {
        if (text == null)
            return;

        String[] Address1 = new String[2];
        String Address;
        int a1 = 0;
        String AddressRegex = "\\brd\\b|\\brm\\b|\\bave\\b|\\bview\\b|\\bste\\b|\\bdr\\b|\\bst\\b|\\bsuite\\b|\\bterrace\\b|\\bln\\b|\\broad\\b|\\bfw\\b|\\bct\\b|\\bhighway\\b|\\bcircle\\b|\\bdrive\\b|\\bpark\\b|\\bavenue\\b|\\bsq\\b|\\bpi\\b|\\bapt\\b|\\bcourt\\b|\\bstreet\\b|\\bunit\\b|\\brd\\W|\\brm\\W|\\bave\\W|\\bdr\\W|\\bst\\W|\\bln\\W|\\bfw\\W|\\bct\\W|\\bsq\\W|\\bpi\\W|\\bapt\\W|\\brd\\W|\\brm\\W|\\bave\\W|\\bview\\W|\\bste\\W|\\bdr\\W|\\bst\\W|\\bsuite\\W|\\bterrace\\W|\\bln\\W|\\broad\\W|\\bfw\\W|\\bct\\W|\\bhighway\\W|\\bcircle\\W|\\bdrive\\W|\\bpark\\W|\\bavenue\\W|\\bsq\\W|\\bpi\\W|\\bapt\\W|\\bcourt\\W|\\bstreet\\W|\\bunit\\W";
        String AddressRegex1 = "\\d+";
        String AddressRegexPOBOX = "\\bpo box\\b";
        Pattern Addresspattern1 = Pattern.compile(AddressRegex1);
        Pattern Addresspattern = Pattern.compile(AddressRegex);
        Pattern AddresspatternPOBOX = Pattern.compile(AddressRegexPOBOX);
        for (int i = 0; i < lines.length - 1; i++) {
            Matcher Addressmatcher = Addresspattern.matcher(lines[i].toLowerCase());
            Matcher AddressmatcherPOBOX = AddresspatternPOBOX.matcher(lines[i].toLowerCase());
            if (Addressmatcher.find()) {
                Matcher Addressmatcher1 = Addresspattern1.matcher(lines[i].toLowerCase());
                if (Addressmatcher1.find()) {
                    Address1[a1] = lines[i];
                    a1 = a1 + 1;
                }
            } else if (AddressmatcherPOBOX.find()) {
                Address1[0] = lines[i];
            }
        }
        streetAddress1 = Address1[0];
        streetAddress2 = Address1[1];
//        for (int i = 0; i < Address1.length; i++) {
//            streetAddress +=  Address1[i];
//        }
    }

    private void checkNull() {
        if (name == null)
            name = ".";
        if (company == null)
            company = ".";
        if (streetAddress1 == null)
            streetAddress1 = ".";
        if (streetAddress2 == null)
            streetAddress2 = ".";
        if (city == null)
            city = ".";
        if (state == null)
            state = ".";
        if (zip == null)
            zip = ".";
        if (phone == null)
            phone = ".";

    }

    public String parseIt() {
        if (text == null)
            return null;
        stripAttn();
        stripPhone();

        stripCity();
        stripState();
        stripZip();
        stripAddress();
        checkNull();

        String finalResult = name + "\n"
                + company + "\n"
                + phone + "\n"
                + streetAddress1 + "\n"
                + streetAddress2 + "\n"
                + city + "," + "\t" + state + "\t" + zip;

        return finalResult;
    }


    public static void main(String args[]) {
        AddressParser addressParser = new AddressParser();
        //addressParser.setText("ATTN: MAILROOM MANAGER\nNYCTAXGUYS, INC.\n419 LAFAYETTE ST\nNEW YORK NY 10003-7033");
        //addressParser.setText("commanding attn officer\nATTN: LTJG JONES,JOHN\nMCTRC FLINT MI INC.\nARMED FORCES RESERVE including TRANING\n CENTER\n3000 dOUGLUS ST\nSA CRA MENTO, TX 48601-4799");
//        addressParser.setText("CLAUDIA A BILOTTI\n" +
//                "251 BRIAR BRAE RD\n" +
//                "STAMFORD, CT 06903-1725");

        //String lines[] = addressParser.text.split("\n");
        //Scanner scanner = new Scanner(lines[3]);
        //String number = scanner.findInLine("\\d{5}|$\\d{5}-\\d{4}$");
        //String number = scanner.findInLine("\\d{5}(?:[-\\s]\\d{4})?$");
//        addressParser.stripAttn();
//        addressParser.stripZip();
//        addressParser.stripCity();
//        addressParser.stripState();
//        addressParser.stripPhone();
//        addressParser.setText("ATTN: MAILROOM MANAGER\n" +
//                "HALLETT MATERIALS\n" +
//                "1 HALLETT DR\n" +
//                "PORTER TX 77365");
//        //System.out.println(

        addressParser.setText("attn:Khurram Javed\n" + "US Tech Solutions\n" +
                "10 Exchange Place, Suite 1820\n" +
                "Jersey City, NY 07302");
        addressParser.parseIt();

        System.out.println("Name: " + addressParser.name);
        System.out.println("Company: " + addressParser.company);
        System.out.println("Phone: " + addressParser.phone);
        System.out.println("Address1: " + addressParser.streetAddress1);
        System.out.println("Address2: " + addressParser.streetAddress2);
        System.out.println("City: " + addressParser.city);
        System.out.println("State: " + addressParser.state);
        System.out.println("Zip: " + addressParser.zip);

        //System.out.println(addressParser.parseIt());


        //Scanner scanner = new Scanner("NY,");
        //System.out.println(scanner.findInLine("([a-zA-Z]+|[a-zA-Z]+\\\\s[a-zA-Z]+)"));
        //String[] lines = {"Attn: Kumail Raza", "SEECS CORP", "14 CARROLL TERRACE", "LAKE ELMO, MN 55042-1000"};


        //System.out.println(Address1[0]);
        return;


    }
}


