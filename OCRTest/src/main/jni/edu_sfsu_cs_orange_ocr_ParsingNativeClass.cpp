#include <edu_sfsu_cs_orange_ocr_ParsingNativeClass.h>

std::vector<std::string> regexSplit(const std::string &s, std::string rgx_str) {
    std::vector<std::string> elems;
    std::regex rgx (rgx_str);
    std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    std::sregex_token_iterator end;
    while (iter != end)  {
        if(*iter != ""){
        elems.push_back(*iter);

        }
         ++iter;
    }
    return elems;
}

int findLine(std::string ocrAddress){
    std::vector<std::string>lines = regexSplit(ocrAddress,"\n");
    std::regex rgx("(.*) ([A-Z][A-Z]) ([0-9-]+)", regex_constants::ECMAScript | regex_constants::icase );
    std::smatch pieces_match;
    int matched=0;
    int j=0;
    for (int i=0; i<lines.size(); i++) {
        if (std::regex_search(lines[i], pieces_match, rgx)) {
        return i;
        }
    }
}

bool isNumber(std::string input){
    std::regex rgx("([^\\d]|^)\\d{4,5}([^\\d]|$)");
    std::smatch pieces_match;
    int matched=0;
    int j=0;
        if (std::regex_search(input, pieces_match, rgx)) {
            return 1;
        }
    return 0;
}

 JNIEXPORT jstring JNICALL Java_edu_sfsu_cs_orange_ocr_ParsingNativeClass_ParseAddress
  (JNIEnv *env, jclass, jstring name)
{

const char* ocrresult;
ocrresult = (env)->GetStringUTFChars(name,NULL);
string address(ocrresult);


std::vector<std::string>lines = regexSplit(ocrresult,"\n");
string output="";
for(int i=0;i<lines.size();i++){
output = output+"line="+lines[i]+"\n";

}
   int matched;
    std::string location="";
    //assign name field
    string parsedname=lines[0];
    //find the line containing city, state, zip
    matched=findLine(ocrresult);
    std::ostringstream ss;
          ss << matched;
          string s = ss.str();
    if(matched>6){
        return 0;
    }
    else if(matched>=5)matched-=1;

    //parse the cityStateZip line to assign City, State, Zip code
     std::string arr[4];
     string lala ="";
        int i = 0;
        std::stringstream ssin(lines[matched]);
        while (ssin.good() && i < 4){
            ssin >> arr[i];

            ++i;
        }

        string parsedcity=arr[0];
       string parsedstate=arr[1];
        string parsedzip=arr[2];

   if(!(isNumber(parsedzip))){
        parsedzip=" ";
        }

    //the lines between cityZipState line and Name line carry location
    int loci;
    for (loci=0; loci<matched; loci++){
        location=location+lines[loci]+"\n";
    }
    if(loci<3){
        for(int i=loci;i<=3;i++){
            location=location+"\n";
        }
    }
    //assign location feild
    string parsedlocation=location;
    string parsedcountry="";
    //check if there is a line after cityStateZip line
    if(lines.size()-1>matched && lines[lines.size()-1].length()<=10){
            parsedcountry = lines[lines.size() - 1];
        }
    else{
        parsedcountry="USA";
    }
    //display the struct


 const char * c = (parsedname+"\n"+parsedlocation +"\n"+parsedcity +"\n"+parsedstate+"\n"+parsedzip +"\n"+parsedcountry).c_str();
 jstring jstr = (env)->NewStringUTF(c);
    return jstr;

}