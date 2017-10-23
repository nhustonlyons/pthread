#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <string.h>


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
    bool sync; //True for Iteration Level, False for Thread
    long double *sumArr;
    long double sumReg;
    
};

pthread_mutex_t sumIter;

long double sumGlob = 0;

void *seqIntegration(void* seqArgs){
    struct seqStruct* a = (struct seqStruct*) seqArgs; // I'm lazy okay?
    long double sum = 0;  
    
    
    if( a->functionID == 1){      
      double xPos = a->tLower+.5*a->rectangles;
      while(xPos <= a->tUpper){
          sum += f1(xPos, a->intensity)*a->rectangles;
          xPos += a->rectangles;
       }    
     }
    
    
    else if( a->functionID == 2){      
      double xPos = a->tLower+.5*a->rectangles;
      while(xPos <= a->tUpper){
          sum += f2(xPos, a->intensity)*a->rectangles;
          xPos += a->rectangles;
       }    
     }
    
    
    else if( a->functionID == 3){      
      double xPos = a->tLower+.5*a->rectangles;
      while(xPos <= a->tUpper){
          sum += f3(xPos, a->intensity)*a->rectangles;
          xPos += a->rectangles;
       }    
     }
    
    else if( a->functionID == 4){      
      double xPos = a->tLower+.5*a->rectangles;
      while(xPos <= a->tUpper){
          sum += f4(xPos, a->intensity)*a->rectangles;
          xPos += a->rectangles;
       }    
     }
   
   
    if (a->sync){
        pthread_mutex_lock(&sumIter);
        sumGlob += sum;
        pthread_mutex_unlock(&sumIter);
    } 
    else if(!a->sync){
        *a->sumArr = sum;
    }
    
    return NULL;

}



int main (int argc, char* argv[]) {

  if (argc < 8) {
   std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
   return -1;
  }
   
   clock_t begin = clock();
   int functionID = std::atoi(argv[1]);
   double lower = std::atof(argv[2]);
   double upper = std::atof(argv[3]);
   long int n = std::atoi(argv[4]);
   long int intensity = std::atoi(argv[5]);
   double rectangles = (upper-lower)/n;
   int nbthreads = std::atoi(argv[6]);
   bool sync;
   
   char iteration[]  = {'i', 't', 'e', 'r', 'a', 't', 'i', 'o', 'n'};
   if(strcmp(argv[7], iteration) == 0){
        sync = true;
   }
   else{
        sync = false;
   }
   
   long double sum = 0;
   
   pthread_t stock[nbthreads];
   struct seqStruct cont[nbthreads];
   long double sumArr[nbthreads]= { 0 };
   double seqGranularity = (upper-lower)/nbthreads;
   pthread_mutex_init(&sumIter, NULL);
   
   for(int k = 0; k < nbthreads; k++){
       cont[k].functionID = functionID;
       cont[k].threadID=k;
       cont[k].rectangles = rectangles;
       cont[k].sync = sync;
       cont[k].tLower = seqGranularity*k+lower;
       cont[k].tUpper = (seqGranularity*(k+1))+lower;
       cont[k].intensity = intensity;
       cont[k].sumArr = &sumArr[k];
       //cont[k].sumReg = sumReg;
       pthread_create(&stock[k], NULL, seqIntegration,(void*) &cont[k]);
   }
   for(int i = 0; i < nbthreads; i++){
       pthread_join (stock[i], NULL);
   }
   
  
   
   if(sync){
       pthread_mutex_lock(&sumIter);
       std::cout << sumGlob << std::endl;
       pthread_mutex_unlock(&sumIter);
   }
   else {
       for(int k = 0; k < nbthreads; k++)
            sum+= sumArr[k];
       std::cout << sum << std::endl;
   }
   pthread_mutex_destroy(&sumIter);
   clock_t end = clock();
   double elapsed = double(end-begin) / CLOCKS_PER_SEC;
   std::cerr<< elapsed << std::endl;
   
  return 0;
}
