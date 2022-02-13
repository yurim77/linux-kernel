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
  printf("Anon pages: %ld\n", mem_stats->anon_pages);
  printf("File pages: %ld\n", mem_stats->file_pages);
  printf("Ref active list: %ld\n", mem_stats->active_list_ref_pages);
  printf("Ref inactive list: %ld\n", mem_stats->inactive_list_ref_pages);
  printf("Active list --> Inactive list: %ld\n", mem_stats->total_active_to_inactive_pages);
  printf("Inactive list --> Active list: %ld\n", mem_stats->total_inactive_to_active_pages);
  printf("Evicted from Inactive list: %ld\n", mem_stats->total_taken_from_inactive_pages);
}

int main() {
  struct memory_statistic *memory_status;
  memory_status = malloc(sizeof(*memory_status));

  int status;
  status = syscall(550, memory_status);

  if (status < 0) {
    printf("Error : status %d\n", status);
    exit(-1);
  }

  printf("\nMemory statistics without allocating a lot of memory...\n");
  printf("\n");

  system("cat /proc/vmstat");
  printf("---------------------------------------\n");
  print_stats(memory_status);
  printf("---------------------------------------\n");

  
  // Allocate a lot of memory
  int *test;
  for (int i = 0; i < 20000; i++) {
  test = malloc(1024 * sizeof(int));
    for(int j=0; j<1024; j++){
        test[j] = j;
    }
  }
  for(int i=0; i<1024; i++) printf("%d ",test[i]);
  printf("\n");

  // Allocate a lot of memory
  int *test2;
  for (int i = 0; i < 20000; i++) {
    test2 = malloc(1024 * sizeof(int));
    for(int j=0; j<1024; j++){
        test2[j] = j;
    }
  }
  for(int i=0; i<1024; i++) printf("%d ", test2[i]);
  printf("\n");
  
  // Allocate a lot of memory
  int *test3;
  for (int i = 0; i < 20000; i++) {
    test3 = malloc(1024 * sizeof(int));
    for(int j=0; j<1024; j++){
        test3[j] = j;
    }
  }
  for(int i=0; i<1024; i++) printf("%d ", test3[i]);
  printf("\n");

  // Allocate a lot of memory
  int *test4;
  for (int i = 0; i < 40000; i++) {
    test4 = malloc(1024 * sizeof(int));
    for(int j=0; j<1024; j++){
        test4[j] = j;
    }
  }
  for(int i=0; i<1024; i++) printf("%d ", test4[i]);
  printf("\n");

  // Allocate a lot of memory
  int *test5;
  for (int i = 0; i < 40000; i++) {
    test5 = malloc(1024 * sizeof(int));
    for(int j=0; j<1024; j++){
        test5[j] = j;
    }
  }
  for(int i=0; i<1024; i++) printf("%d ", test5[i]);
  printf("\n");

  status = syscall(550, memory_status);

  if (status < 0) {
    printf("Error : status %d\n", status);
    exit(-1);
  }

  printf("\nMemory statistics with allocating a lot of memory...\n");
  printf("\n");

  system("cat /proc/vmstat");
  printf("---------------------------------------\n");
  print_stats(memory_status);
  printf("---------------------------------------\n");


  free(test);
  free(test2);
  free(test3);
  free(test4);
  free(test5);

  return 0;
}