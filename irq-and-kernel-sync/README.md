## **Interrupt & Kernel Synchronization**
driver.c author : EmbeTronicX, modifier : Jonghyeon Kim, Yurim Jang

### Environment
* linux-5.15.4

### Description
* IRQ와 IOCTL을 통해 덧셈과 뺄셈을 수행합니다.
* 그 사이에 적절한 싱크로나이제이션을 추가하여 올바른 값을 계산하도록 수정했습니다. (spin_lock_irqsave, spin_lock 함수)
* 계산 결과가 정확한지 확인하는 테스트 파일이 포함되어 있습니다.
* IRQ sum의 올바른 값 : -15
* IOCTL0 sum의 올바른 값 : 15
* IOCTL1 sum의 올바른 값 : 10
* IOCTL2 sum의 올바른 값 : 25

### Files
* driver.c : ioctl과 irq로 덧셈과 뺄셈을 수행하는 디바이스 드라이버 구현
* simple_ioctl.c : 테스트 파일 1 
* ioctl_test.c :  테스트 파일 2 
* test.sh : 테스트 파일 3 (ioctl_test를 여러번 실행)

### Output Example
```shell
root@vm:~/irq-and-kernel-sync# mknod /dev/mycdev c 42 0
root@vm:~/irq-and-kernel-sync# insmod driver.ko

# Test my_read and my_write
root@vm:~/irq-and-kernel-sync# cat /dev/mycdev
root@vm:~/irq-and-kernel-sync# echo hi > /dev/mycdev

# Test my_ioctl
root@vm:~/irq-and-kernel-sync# ./simple_test
[ 173.549762] Device File Opened...!!!
This is simple ioctl call for irq handler
[ 173.552792] IRQ, sum: -15
[ 173.554234] Device File Closed...!!!

root@vm:~/irq-and-kernel-sync# ./ioctl_test
[ 32.805239] Device File Opened...!!!
[ 32.808590] IOCTL0, sum: 15
[ 32.808777] IOCTL1, sum: 10
[ 32.813341] IRQ, sum: -15
[ 32.813685] IOCTL2, sum: 25
[ 32.815738] Device File Closed...!!!...

root@vm:~/irq-and-kernel-sync# ./test.sh
...
[ 200.686471] Device File Closed...!!!
[ 200.687144] IRQ, sum: -15
[ 200.687393] IOCTL2, sum: 25
[ 200.693388] Device File Closed...!!!
[ 200.695093] Device File Opened...!!!
[ 200.700874] IOCTL0, sum: 15
[ 200.701009] IOCTL1, sum: 10
[ 200.704750] IRQ, sum: -15
[ 200.704943] IOCTL2, sum: 25
[ 200.705378] Device File Closed...!!!
...

expected IRQ : -15, [100/100]
expected IOCTL0: 15, [98/100]
expected IOCTL1: 10, [100/100]
expected IOCTL2: 25, [100/100]
[Total Points = [398/400]]

root@vm:~/irq-and-kernel-sync# rmmod driver
```