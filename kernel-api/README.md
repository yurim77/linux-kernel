## **Kernel API**

### Environment
* linux-5.15.4

### Description
* 리눅스 커널의 리스트 자료 구조를 통해 문자열을 추가, 삭제하는 커널 모듈입니다.
* addf str : 리스트 머리에 문자열 str을 추가합니다.
* adde str : 리스트 꼬리에 문자열 str을 추가합니다.
* delf str : 문자열 str과 일치하는 첫번째 문자열을 리스트에서 삭제합니다.
* dela str : 문자열 str과 일치하는 모든 문자열을 리스트에서 삭제합니다.
* echo를 통해 리스트를 조작하고 cat을 통해 리스트 내용을 확인할 수 있습니다. (실행 예제 참조)

### Output Example
* 커널 모듈 실행
```shell
# Output example
root@vm:~/kernel-api# insmod list.ko

root@vm:~/kernel-api# echo addf hello > /proc/list/management

root@vm:~/kernel-api# cat /proc/list/preview
hello

root@vm:~/kernel-api# rmmod list

```