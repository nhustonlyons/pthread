#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <string.h>
#include <string>

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


struct dynStruct{
    int functionID;
    int threadID;
    long int intensity;
    long double rectangles;
    bool sync[3]; //True for Iteration Level, False for Thread    
};

struct stateStruct{
    int n;
    int curLower;
    int origin;
    int granularity;
    bool loopState;
};

struct returnStruct{
    double tLower;
    double tUpper;
};

pthread_mutex_t sumMutex, getNextMutex, loopDoneMutex;
pthread_cond_t readyCond;
bool ready = true;
struct stateStruct glob;
long double sumGlob = 0;

bool loopDone(){
    pthread_mutex_lock(&loopDoneMutex);
    bool returnMe = glob.loopState;
    pthread_mutex_unlock(&loopDoneMutex);
    return returnMe;
}

//This is an attempt to prevent undefined behavior. I'm aware that there could be a time lag inbetween reading the value in loopDone and getting your next stuff in getNext, but this way it'll return add 0 to the funciton if that happens.
struct returnStruct getNext() {
    
    pthread_mutex_lock(&getNextMutex);
    while(!ready){
        pthread_cond_wait(&readyCond, &getNextMutex);
    }
    ready = !ready; 
    
    struct returnStruct b;
    if (glob.loopState) {
        b.tLower = glob.curLower;
        glob.curLower += glob.granularity;
        b.tUpper = glob.curLower;
    }
    else{
        b.tLower = 0;
        b.tUpper = 0;
    }
    if(glob.curLower >= glob.n){ 
        glob.loopState = false;
    }
    ready = true;
    pthread_cond_signal(&readyCond);
    
    pthread_mutex_unlock(&getNextMutex);
    return b;
}


void *dynIntegration(void* dynArgs) {
    struct dynStruct* a = (struct dynStruct*) dynArgs; // I'm lazy okay?
    long double sum = 0;
    while (loopDone()) {
        struct returnStruct b = getNext();
        b.tLower = b.tLower * a->rectangles + glob.origin;
        b.tUpper = b.tUpper * a->rectangles + glob.origin;
        if (a->functionID == 1) {
            double xPos = b.tLower + .5 * a->rectangles;
            if (a->sync[1]) {
                while (xPos <= b.tUpper) {
                    pthread_mutex_lock(&sumMutex);
                    sumGlob += f1(xPos, a->intensity) * a->rectangles;
                    pthread_mutex_unlock(&sumMutex);
                    xPos += a->rectangles;
                }
            } else if (a->sync[2]) {
                sum = 0;
                while (xPos <= b.tUpper) {
                    sum += f1(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
                pthread_mutex_lock(&sumMutex);
                sumGlob += sum;
                pthread_mutex_unlock(&sumMutex);
                
            } else if (a->sync[3]) {
                while (xPos <= b.tUpper) {
                    sum += f1(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
            }
        }


        else if (a->functionID == 2) {
            double xPos = b.tLower + .5 * a->rectangles;
            if (a->sync[1]) {
                while (xPos <= b.tUpper) {
                    pthread_mutex_lock(&sumMutex);
                    sumGlob += f2(xPos, a->intensity) * a->rectangles;
                    pthread_mutex_unlock(&sumMutex);
                    xPos += a->rectangles;
                }
            } else if (a->sync[2]) {
                sum = 0;
                while (xPos <= b.tUpper) {
                    sum += f2(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
                pthread_mutex_lock(&sumMutex);
                sumGlob += sum;
                pthread_mutex_unlock(&sumMutex);
            } else if (a->sync[3]) {
                while (xPos <= b.tUpper) {
                    sum += f2(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
            }
        }

        else if (a->functionID == 3) {
            double xPos = b.tLower + .5 * a->rectangles;
            if (a->sync[1]) {
                while (xPos <= b.tUpper) {
                    pthread_mutex_lock(&sumMutex);
                    sumGlob += f3(xPos, a->intensity) * a->rectangles;
                    pthread_mutex_unlock(&sumMutex);
                    xPos += a->rectangles;
                }
            } else if (a->sync[2]) {
                sum = 0;
                while (xPos <= b.tUpper) {
                    sum += f3(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
                pthread_mutex_lock(&sumMutex);
                sumGlob += sum;
                pthread_mutex_unlock(&sumMutex);
            } else if (a->sync[3]) {
                while (xPos <= b.tUpper) {
                    sum += f3(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
            }
        }


        else if (a->functionID == 4) {
            double xPos = b.tLower + .5 * a->rectangles;
            if (a->sync[1]) {
                while (xPos <= b.tUpper) {
                    pthread_mutex_lock(&sumMutex);
                    sumGlob += f4(xPos, a->intensity) * a->rectangles;
                    pthread_mutex_unlock(&sumMutex);
                    xPos += a->rectangles;
                 }
            } else if (a->sync[2]) {
                sum = 0;
                while (xPos <= b.tUpper) {
                    sum += f4(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
                pthread_mutex_lock(&sumMutex);
                sumGlob += sum;
                pthread_mutex_unlock(&sumMutex);
            } else if (a->sync[3]) {
                while (xPos <= b.tUpper) {
                    sum += f4(xPos, a->intensity) * a->rectangles;
                    xPos += a->rectangles;
                }
            }
        }

        if (a->sync[3]) {
            pthread_mutex_lock(&sumMutex);
            sumGlob += sum;
            pthread_mutex_unlock(&sumMutex);
        }
    }

    return NULL;

}

  
int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
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
   int granularity = std::atoi(argv[8]);
   bool sync[3];
   
   char iteration[]  = {'i', 't', 'e', 'r', 'a', 't', 'i', 'o', 'n'};
   char chunk[] = {'c', 'h', 'u', 'n', 'k'};
   if(strcmp(argv[7], iteration) == 0){
        sync[0] = true;
   }
   else if(strcmp(argv[7], chunk) == 0){
        sync[1] = true;
   }
   else{
        sync[2] = true;
   }
   
    glob.n = n;
    glob.curLower = 0;
    glob.origin = lower;
    glob.granularity = granularity;
    glob.loopState = true;
   
    
   
    pthread_t stock[nbthreads];
    struct dynStruct cont[nbthreads];
    pthread_mutex_init(&sumMutex, NULL);
    pthread_mutex_init(&loopDoneMutex, NULL);
    pthread_mutex_init(&getNextMutex, NULL);
    pthread_cond_init(&readyCond, NULL);
    
    for(int k = 0; k < nbthreads; k++){
       cont[k].functionID = functionID;
       cont[k].threadID=k;
       cont[k].rectangles = rectangles;
       cont[k].sync[1] = sync[1];
       cont[k].sync[2] = sync[2];
       cont[k].sync[3] = sync[3];
       cont[k].intensity = intensity;
       pthread_create(&stock[k], NULL, dynIntegration,(void*) &cont[k]);
    }

    for (int i = 0; i < nbthreads; i++) {
        pthread_join(stock[i], NULL);
    }

    pthread_mutex_lock(&sumMutex);
    std::cout << sumGlob << std::endl;
    pthread_mutex_unlock(&sumMutex);
    
    pthread_mutex_destroy(&sumMutex);
    pthread_mutex_destroy(&loopDoneMutex);
    pthread_mutex_destroy(&getNextMutex);
    pthread_cond_destroy(&readyCond);
    
    clock_t end = clock();
    double elapsed = double(end-begin) / CLOCKS_PER_SEC;
    std::cerr<< elapsed << std::endl;
  return 0;
}
