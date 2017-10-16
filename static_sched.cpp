#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

struct seqStruct{
    int functionID;
    int threadID;
    long int tLower;
    long int tUpper;
    long int intensity;
    long double rectangles;
    char* sync;
    double* sumArr;
    
};

pthread_mutex_t sumIter;

void *seqIntegration(void *seqArgs){
    struct seqStruct a = seqArgs; // I'm lazy okay?
    double sum = 0;  
    
    
    if( a.functionID == 1){      
      double xPos = a.tLower+.5*a.rectangles;
      while(xPos <= a.tUpper){
          sum += f1(xPos, a.intensity)*a.rectangles;
          xPos += a.rectangles;
       }    
     }
    
    
    else if( a.functionID == 2){      
      double xPos = a.tLower+.5*a.rectangles;
      while(xPos <= a.tUpper){
          sum += f2(xPos, a.intensity)*a.rectangles;
          xPos += a.rectangles;
       }    
     }
    
    
    else if( a.functionID == 3){      
      double xPos = a.tLower+.5*a.rectangles;
      while(xPos <= a.tUpper){
          sum += f3(xPos, a.intensity)*a.rectangles;
          xPos += a.rectangles;
       }    
     }
    
    else if( a.functionID == 4){      
      double xPos = a.tLower+.5*a.rectangles;
      while(xPos <= a.tUpper){
          sum += f4(xPos, a.intensity)*a.rectangles;
          xPos += a.rectangles;
       }    
     }
    
    if (std::strcmp(a.sync, "iteration")== 0){
        pthread_mutex_lock(&sumIter);
        a.sumArr += sum;
        pthread_mutex_unlock(&sumIter);
    } 
    else if(std::strcmp(a.sync, "thread")== 0){
        a.sumArr[a.threadID] = sum;
    }
}

int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
   
   int functionID = std::atoi(argv[1]);
   double lower = std::atof(argv[2]);
   double upper = std::atof(argv[3]);
   long int n = std::atoi(argv[4]);
   long int intensity = std::atoi(argv[5]);
   double rectangles = (lower-upper)/n;
   int nbthreads = std::atoi(argv[6]);
   char* sync = argv[7];
   long double sum = 0;
   
   
  
  return 0;
}
