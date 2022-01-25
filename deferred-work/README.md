## **Deferred Work**

### Environment
* linux-5.15.4

### Description
* 몇가지 간단한 지연 작업과 적절한 동기화 구현
* Understanding deferred work
* Implementation of common tasks that uses deferred work
* Understanding the features of synchronization for deferred work
* keyword : workqueue, kernel threads, tasklet, timer, softirq

### Files
```bash
├── deffered/ : 인터럽트 컨텍스트에서 우회적으로 블로킹 오퍼레이션 수행 (워크큐 이용)
│   ├── include/
│   │   └── deferred.h
│   ├── kernel/
│   │   ├── deferred.c
│   │   ├── makenode 
│   │   └── Makefie
│   ├── user/
│   │   ├── test.c
│   │   ├── sleep.sh
│   │   └── Makefie
│   │
│   └──syscall_64.tbl
│ 
├── kthread/ : 커널 스레드 생성-삭제와 모듈 로딩-언로딩 지연
│   ├── kthread.c
│   └── Makefile
│
└── timer/ : 타이머 초기화 및 스케줄링
    ├── timer.c
    └── Makefile
```

### Output Example
```shell
### deferred folder
root@vm:~/deferred/kernel# ./makenode
root@vm:~/deferred/kernel# insmod deferred.ko

# set timer that expire after 2 sec
root@vm:~/deferred/user# ./test s 2   
[deferred_open] Device opened
Set timer to 2 seconds
[deferred_ioctl] Command: Set timer
[deferred_release] Device released
[timer_handler] pid = 0, comm = swapper/0

# set timer that expires after 2 sec
# When timer expires, put task to sleep for 5 sec
root@vm:~/deferred/user# ./test a 2   
Allocate memory after 2 seconds
[deferred_ioctl] Command: Allocate memory
[deferred_release] Device released
[timer_handler] pid = 0, comm = swapper/0
I've been sleeping for 5 seconds.

### kthread folder
# load module
root@vm:~/kthread# insmod kthread.ko
[kthread_init] Init module
[my_thread_f] Current process id is 393 (mykthread0)

# kthread is alive
root@vm:~/kthread# lsmod
  Tainted: G
kthead 16384 0 - Live 0xffffffffc039d000 (O)

# kthread is dead + unload module
root@vm:~/kthread# rmmod kthread
[my_thread_f] Exiting
[kthread_exit] Exit module

### timer folder
root@vm:~/timer# insmod timer.ko
[timer_init] Init module
[timer_handler] nseconds = 1
[timer_handler] nseconds = 2
[timer_handler] nseconds = 3
...
```
