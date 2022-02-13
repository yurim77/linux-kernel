#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

struct memory_statistic {
  unsigned long anon_pages;
  unsigned long file_pages;
  unsigned long active_list_ref_pages;
  unsigned long inactive_list_ref_pages;
  unsigned long total_active_to_inactive_pages;
  unsigned long total_inactive_to_active_pages;
  unsigned long total_taken_from_inactive_pages;
};

void print_stats(struct memory_statistic *mem_stats){
  printf("Anon pages: %ld\n", mem_stats->anon_pages, mem_stats->anon_pages/4);
  printf("File pages: %ld\n", mem_stats->file_pages);
  printf("Ref active list: %ld\n", mem_stats->active_list_ref_pages);
  printf("Ref inactive list: %ld\n", mem_stats->inactive_list_ref_pages);
  printf("Active list --> Inactive list: %ld\n", mem_stats->total_active_to_inactive_pages);
  printf("Inactive list --> Active list: %ld\n", mem_stats->total_inactive_to_active_pages);
  printf("Evicted from Inactive list: %ld\n", mem_stats->total_taken_from_inactive_pages);
}

int main() {

  int i,j;

  struct memory_statistic *memory_status;
  memory_status = malloc(sizeof(*memory_status));

  system("cat /proc/vmstat");
  printf("---------------------------------------\n");
  int status;
  status = syscall(550, memory_status);

  if (status < 0) {
    printf("Error : status %d\n", status);
    exit(-1);
  }

  print_stats(memory_status);
  
  int *test;   
  int *test2;
  int *test3;
  // Allocate a lot of memory
  test3 = malloc(4096*sizeof(int));
  for (i = 0; i < 500000; i++) {
    test = malloc(128 * sizeof(int)); // 4kb = 4096 byte -> 1page
    for(j=0;j<128;j++){
      test[j]=j;
    }
  }
  for(i=0;i<128;i++) test[i] = 10 * i;
  
  for(i=0;i<4096;i++){
    test3[i] = i+1 + i*19;
  }

  // Allocate a lot of memory

  for (i = 0; i < 500000; i++) {
    test2 = malloc(128 * sizeof(int));
    for(int j=0; j<128;j++){
        test2[j]=j * 4;
    }
  }
  for(i=0;i<128;i++) test2[i] += i*100;

  for(int i=0;i<128;i++){
    if(i%2==0){
      test2[i] += test[i];
    }
   
  }
  for(i=0; i<128;i++){
    if(i%3==0) test[i] += test2[i] + i;
    if(i%2==0) test3[i*4] += test[i]+test2[i];
  }

  for(j=0;j<100000;j++){
    for(i=0;i<4096;i++) test3[i] = j; 
  }

  for(i=0;i<128;i++){
    if(i%8==0) test[i] = i+8;
  }

  system("cat /proc/vmstat");
  printf("---------------------------------------\n");
  status = syscall(550, memory_status);


  if (status < 0) {
    printf("Error : status %d\n", status);
    exit(-1);

  }
  print_stats(memory_status);

  free(test);
  free(test2);

  return 0;
}