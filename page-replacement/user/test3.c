#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
// #include <linux/memory_statistic.h>

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
  //syscall(551);
  int i,j;

  struct memory_statistic *memory_status;
  memory_status = malloc(sizeof(*memory_status));

  // printf("---------------------------------------\n");
  system("cat /proc/vmstat");
  printf("---------------------------------------\n");
  int status;
  status = syscall(550, memory_status);

  if (status < 0) {
    printf("Error : status %d\n", status);
    exit(-1);
  }

  print_stats(memory_status);
  
  // Allocate a lot of memory
  int *test;   
  for (i = 0; i < 100000; i++) {
    test = malloc(1024 * sizeof(int)); // 4kb = 4096 byte -> 1page
  }
  for(j=0;j<10000;j++);

  for(j=0;j<1024;j++){
    test[j] = (j+1)*(j+2);
  }
  int a = 32;
  for(j=0;j<1024;j++){
    test[j] *= a;
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


  return 0;
}