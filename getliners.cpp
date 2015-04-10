//version 6
#include <iostream>
#include <fstream> //used for ifstream
#include <cstdio> //used for printf
#include <cstring> //used for strtok strcmp
#include <map>
#include <vector>
#include <utility> //used for make_pair
#include <cstdlib> //used atoi
#include <zlib.h>

size_t LENS = 1000000; //buffer length


//comparison operator used when comparing char*
struct cmp_char {
  bool operator()(const char *first,const char* second) const {
    int tmp = std::strcmp(first, second);
    return tmp<0;
  }
};



std::vector<int> getVec(const char *fname){
  fprintf(stderr,"\t-> openfile:%s\n",fname);
  FILE *fp = NULL;
  fp=fopen(fname,"r");
  if(fp==NULL){
    fprintf(stderr,"file:%s doesn't load\n",fname);
    exit(0);
  }
  std::vector<int> ret;
  char *buf = new char[LENS];
  while(fgets(buf,LENS,fp))
    ret.push_back(atoi(strtok(buf,"\r\n\t ")));
  
  fprintf(stderr,"number of lines to extract=%lu\n",ret.size());
  return ret;
}




typedef std::map <char*,int,cmp_char> aMap;


void printMap(const aMap &m,FILE *fp){
  for(aMap::const_iterator it = m.begin(); it != m.end(); ++it){
    char *l = it->first;
    int val = it->second;
    fprintf(fp,"%s\t%d\n",l,val) ;
  }
}





aMap build_map(const char *filename,const char *delims="\r \t\n"){
  std::ifstream pFile(filename,std::ios::in);
  char buffer[LENS];

  
  aMap ret;
  while(pFile.getline(buffer,LENS)){
    char *key = strtok(buffer,delims);
    while(key!=NULL){
      ret.insert(std::make_pair(strdup(key),1));
      key =strtok(NULL,delims);
    }
  }
  fprintf(stderr,"num items to grep for: %lu\n",ret.size());
  return ret;
  
}



int main(int argc, char *argv[]){
  if(argc==1){
    fprintf(stderr,"\tUSE -c column -d delimter -k keysfile -f infile -b bufferSize\n\n");
    fprintf(stderr,"\texample1: to grep for all instances in file: \'keys\', in column 4 of file: \'vals\'\n");
    fprintf(stderr,"\t\t./getliners  -k key -f vals  -c 4\n");
    
    return 0;
  }

  int extractCol=2;

  int argPos = 1;
  const char *delims = " \t\n\r";
  const char* indexfile = NULL;
  const char* linefile = NULL;
  const char* datafile = NULL;
  const char* infokeys = NULL; 
  int doCompl =0;
  while(argPos <argc){
    if(strcmp(argv[argPos],"-c")==0)
      extractCol  = atoi(argv[argPos+1]);
    else if(strcmp(argv[argPos],"-d")==0)
      delims = strdup(argv[argPos+1]);
    else if(strcmp(argv[argPos],"-k")==0)
      indexfile = argv[argPos+1];
    else if(strcmp(argv[argPos],"-l")==0)
      linefile = argv[argPos+1];
    else if(strcmp(argv[argPos],"-f")==0)
      datafile = argv[argPos+1];
    else if(strcmp(argv[argPos],"-i")==0)
      infokeys = argv[argPos+1];
   else if(strcmp(argv[argPos],"-v")==0)
     doCompl = atoi(argv[argPos+1]);
   else if(strcmp(argv[argPos],"-b")==0)
     LENS = atoi(argv[argPos+1]);
  
   else {
      printf("\tUnknown arguments: %s\n",argv[argPos]);
      printf("USE -c column -d delimter -k keysfile -f infile\n");
      return 0;
    }
    argPos+=2;
  }
  aMap asso;//used for keys
  std::vector<int> vec;//used for linenumbers
  if(indexfile!=NULL)
    asso = build_map(indexfile,delims);
  if(linefile)
    vec = getVec(linefile);    
  //printMap(asso);

  gzFile gz = Z_NULL;
  gz = gzopen(datafile,"r");
  if(gz==Z_NULL){
    fprintf(stderr,"Problems opening file:%s\n",datafile);
    return 0;
  }
  char buffer[LENS];
  char *original = new char[LENS];

  if(asso.size()!=0){
    fprintf(stderr,"Using keys file for extracting lines\n");
    while(gzgets(gz,buffer,LENS)){
      //      fprintf(stderr,"here extractCol=%d\n",extractCol);
      strcpy(original,buffer);
      char *cmp = strtok(buffer,delims);
      //      fprintf(stderr,"counter=%d\tcmp:%s\n",0,cmp);
      for(int counter=2;counter<extractCol+1;counter++){
	cmp = strtok(NULL,delims);
	if(cmp==NULL){
	  fprintf(stderr,"Problem col:%d doesnt exist will exit:%s\n",counter,cmp);
	  return 0;
	}
	//	fprintf(stderr,"counter=%d,cmp=%s\n",counter,cmp);
      }
      //cmp now contains the correct column
      //      fprintf(stderr,"looking for cmp=%s\n",cmp);
      aMap::iterator it = asso.find(cmp);
      int hit=(it!=asso.end());
      if((doCompl==0&&hit) ||(doCompl==1&&hit==0) ){
	printf("%s",original);
	it->second =0;
      }
    }
  }else{
    fprintf(stderr,"Using lines file for extracting lines\n");
    int curLine =1;
    for(uint i=0;i<vec.size();i++){
      int nextLine=vec[i];
    
      while(gzgets(gz,buffer,LENS))
	if(nextLine==(++curLine))
	  break;
      char *tok = strtok(buffer,"\r\t\n ");
      fprintf(stdout,"%c %c",tok[0],tok[1]);
      while((tok = strtok(NULL,"\r\t\n ")))
	fprintf(stdout," %c %c",tok[0],tok[1]);
      fprintf(stdout,"\n");
    }
  }

  FILE *fp = fopen(infokeys,"w");
  printMap(asso,fp);
  fclose(fp);
  return 0;
}
