## **New Page Replacement Policy**

### Description
Part 1: Memory management statistics - 메모리 관리 통계 출력  
Collect a number of memory management of Linux kernel
1. The current number of pages in the current LRU page lists 
    - ANON: ACTIVE, INACTIVE
    - FILE: ACTIVE, INACTIVE
2. The current number of pages those reference bits are set in active/inactive list
3. The cumulative number of pages moved from active list to inactive list and vice versa
4. The cumulative number of pages evicted from inactive list
    - When we reboot the system, reset the cumulative numbers
<br/>
<br/>

Part 2: Count based clock page replacement - 페이지 교체 알고리즘 수정  
Replace the second chance LRU approximation algorithm with a counter based clock algorithm in both active and inactive page lists
1. Keep a reference counter for each page
  Counter is initialized at 0
2. When try_to_free_pages () is called,
  Scan pages
  Add reference bit of each page to reference counter of each page
    - Clear reference bit to next scan
  Check the reference count
    - If counter is 0, evict page to destination(inactive, swap space, or free)
    - Otherwise, decrement the counter by 1 and move the page to tail of list

<br/>
메모리 관리 통계 출력
<br/>
<br/>

- Provide a system call that report memory management statistics to user level applications
<br/>
<br/>

 in more detail : http://www.cs.fsu.edu/~baker/opsys/assign/P4.html
<br/>
<br/>

### Environment
* linux-5.15.4
<br/>
<br/>

### Files
```bash
# 프로젝트를 위해 수정, 추가한 파일들
# look kernel.diff file
# src/linux-5.15.4/
├── Makefile
│
├── arch/x86/entry/syscalls/
│   └──syscall_64.tbl
│ 
├── mm/
│   ├── vmscan.c
│   ├── swap.c
│   └── page_alloc.c
│
├── include/linux/
│   ├── syscalls.h
│   ├── mm_types.h
│   └── memory_statistic.h
│
└── my_syscall/ # mkdir my_syscall
    ├── Makefile
    └── memory_statistic.c

# 테스트용 유저 레벨 애플리케이션
# project/user/
├── Makefile
├── test1.c
├── test2.c
└── test3.c
```
<br/>
<br/>

### Usage
```shell
# Step 1
# Build your kernel in src/linux-5.15.4/
$ make -j$(nproc)

# Step 2
# Boot your machine and add swap file
# 0. boot your virtual machine
# 1. Create a file that will be used for swap :
root# fallocate -l 256m /swapfile

# or, if you don't have fallocate, use following command
root# dd if=/dev/zero of=/swapfile bs=256 count=262144

# 2. Set the permission type of swap file:
root# chmod 600 /swapfile

# 3. Use the mkswap utility to set up the file as linux swap area
root# mkswp /swapfile

# 4. Enable the swap with the following command:
root# swapon /swapfile

# 4-1. To make the change permanent, open the /etc/fstab file 
#      and append th following line:
# In /etc/fstab
/swapfile swap swap defaults 0 0

# 5. To verify that the swap is active, use either `swapon` or the `free`
#    as shown below:
root# swapon --show

# or
root# free -h

# Step 3
# Test Page Replcement in project/user/
root/project/user# make
root/project/user# ./test1
```
<br/>
<br/>

### Outut Example
![image](./image.png)
