#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]) {
  uint8_t *buffer=NULL;
  float exp = 0;
  int dpr = 0;
  float N = 0;
  uint64_t i=0;

  time_t t;
  double time_start=0; //start-time
  double time_since=0; //time since last iteration [ms]
  double time_end=0; //end-time [ms]
  double time_period=1000.0; //period [ms]
  double max_duration=20000*1000; //exit condition [ms]
  int page_size=4096; // memory page size of system

  //Checking that argument is passed and correct
  if (argc != 3) {
    printf("Wrong number of arguments\n");
    exit(-3);
  }

  //Assigning value from command line
  exp=atof (argv[1]);
  dpr=atoi (argv[2]); // dirty page rate pages/s

  if (exp <= 0 || dpr <= 0) {
    printf("Improper parameters passed\n");
    exit(-4);
  }

  if (dpr >= pow(10,exp)/page_size) {
    //printf("Dirty page rate over max memory pages\n");
    //exit(-4);
    dpr = floor(pow(10,exp)/page_size);
  }

  //Initializing random number generator
  srand((unsigned) time(&t));

  //Initializing N
  N=pow(10,exp);

  //Heap memory allocation and initialization
  buffer=(uint8_t*)malloc(N*sizeof(uint8_t));

  //Checking allocation
  if (buffer==NULL) {
    printf("Error allocating memory\n");
    exit(-2);
  }

  //Initializing memory
  for (int i=0; i<N; i++){
    buffer[i]=(uint8_t)(rand() % 255);
  }

  //Initializing timeout variables
  time_start = (double) clock() * 1000 / CLOCKS_PER_SEC; //start-time
  time_since = time_start;

  //Initializing buffer index related variable
  int page_index_start=0;
  int page_index_end=-1;
  int bias=0;
  int max_page_index= N/page_size-1;

  while (time_since < time_start + max_duration) //Max duration loop
  {
    // Define the range of buffer to be changed in sub-iteration
    page_index_start=page_index_end+1;
    page_index_end+=dpr-1;
    if (page_index_end>max_page_index){
      page_index_end=page_index_end-max_page_index;
    }

    // Define the duration of sub-iteration
    time_end = time_since + time_period; //Updating end time

    // Do the sub iteration
    do {
      if (page_index_end>page_index_start){
        // In the condition of buffer in continuous range to be changed
        for (i=page_index_start; i<=page_index_end; i++) {
          buffer[i*page_size+bias]=(uint8_t)(rand() % 255); //Writing random bytes to bias-th byte in each memory page
        }
      }
      else{
        // In the condition of buffer in discontinuous range to be changed (head and tail)
        for (i=page_index_start; i<=max_page_index; i++) {
          buffer[i*page_size+bias]=(uint8_t)(rand() % 255); //Writing random bytes to bias-th byte in each memory page
        }
        for (i=0; i<=page_index_end; i++) {
          buffer[i*page_size+bias]=(uint8_t)(rand() % 255); //Writing random bytes to bias-th byte in each memory page
        }
      }

      time_since = (double) clock() * 1000 / CLOCKS_PER_SEC; //Updating current time

      // If in the first iteration the execution time already exceed the defined duration, the target dpr can never be reached.
      if (bias==0 && time_since > time_end){
        printf("Dirty page rate not reachable!!!!\n");
      }

      bias++; //Incrementing number of bias
      // If a page is fully updated, reset the bias to iterate from the beginning of each page
      if (bias==4096){
        //printf("Fully update a page...");
        bias=0;
      }
    } while (time_since <= time_end); //Checking if second elapsed

    bias = 0; //Reset the bias
  }

  //Freeing up memory
  free(buffer);

  return 0;
}
