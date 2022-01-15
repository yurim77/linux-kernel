## **Character Device Driver**
skeleton developed by UPB, modified for Ajou Univ. SCE394

### Environment
* linux-5.15.4

### Description
* 디바이스 드라이버란 컴퓨터 주변 장치를 제어하기 위한 프로그램으로, 리눅스 커널에서 모듈로 구현합니다.
* 해당 파일은 간단한 문자 디바이스 드라이버를 구현한 커널 모듈입니다.
* file operation 중 open, release, read, write, ioctl (unlocked_ioctl) 이 구현되어 있습니다.
* open : 유저 영역에서 해당 디바이스를 열 때 마다 호출되며 수행되는 로직입니다.
* release : 유저 영역에서 해당 디바이스를 닫을때 수행되는 로직입니다.
* read : 유저 영역에서 해당 디바이스로부터 데이터를 받을때 호출됩니다.
* write : 유저 영역으로 부터 전달받은 데이터를 저장합니다.
* ioctl : 보통 하드웨어의 제어와 상태 정보를 얻기 위해 사용하나 이 코드에서는 ioctl을 이용해 실제 하드웨어를 제어하도록 구현하지 않았습니다. 사용법을 간단히 익히는 정도로만 사용했습니다.

### Files
* my_cdev.c : 문자 디바이스 드라이버 (커널 모듈) 를 구현한 c 파일
* my_cdev.h : my_cdev.c의 헤더 파일
* my_cdev_test.c : 구현한 문자 디바이스 드라이버를 테스트하기 위한 c 파일

### Output Example
* 커널 모듈 실행
```shell
# Output example
root@vm:~/lab# mknod /dev/mycdev c 42 0

root@vm:~/lab# insmod my_cdev.ko
my_cdev: loading out-of-tree module taints kernel.
mycdev loaded

# 문자열 확인
root@vm:~/lab# cat /dev/mycdev
mycdev opened
mycdev read() called
hello
mycdev read() called
mycdev released

# 문자열 추가
root@vm:~/lab# echo hi > /dev/mycdev
mycdev opened
mycdev write() called
mycdev released

root@vm:~/lab# rmmod my_cdev
mycdev unloaded
```