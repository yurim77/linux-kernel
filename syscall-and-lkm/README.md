## **System Call & Linux Kernel Module**

### Environment
* linux-5.15.4

### Description
* n개의 랜덤 숫자를 생성한 후, n개의 숫자 중 소수가 몇 개인지 계산하는 프로그램입니다.
* 사용자에게 숫자 n을 입력 받습니다.
* 유저 영역에서만 실행, 새로운 시스템 콜을 통해 실행, 커널 모듈을 통해 실행 총 3가지 방식으로 구현되어있습니다.
* 각 프로그램은 소수의 개수를 계산하는데 걸리는 시간을 측정합니다.
* malloc()과 kmalloc()의 차이를 알 수 있습니다.

### Files
```bash
├── user
│   ├── prime_user.c : 유저 영역 코드
│   └── Makefile
├── system_call
│   ├── sys
│   │   ├── prime_sys.c : 시스템 콜 정의
│   │   └── Makefile
│   └── sys_test
│       ├── prime_sys_test.c : 시스템 콜을 호출하는 유저 영역 코드
│       └── Makefile
└── kernel_module
    ├── prime_module.c : 커널 모듈 코드
    └── Makefile
```

### Output Example
* 바이너리 파일 실행
```shell
# Execute binary file
# Output example

# Usage: ./prime_user -n <number of random numbers>
root@vm:~/syscall-and-lkm/user# ./prime_user -n 10000
The number of total random numbers : 10000
The number of prime numbers : 1246
Processing time : 114660 us

# Usage: ./prime_sys_test -n <number of random numbers>
root@vm:~/syscall-and-lkm/system_call# ./prime_sys_test -n 10000
[  486.396184] The number of total random numbers : 10000
[  486.451566] The number of prime numbers : 1246
[  486.451730] Processing time : 73984 us

# Usage: insmod prime_module.ko n=<number of random numbers>
root@vm:~/syscall-and-lkm/kernel_module# insmod prime_module.ko n=10000
[  433.465728] The number of total random numbers : 10000
[  433.593631] The number of prime numbers : 1256
[  433.593891] Processing time : 71754 us
```
