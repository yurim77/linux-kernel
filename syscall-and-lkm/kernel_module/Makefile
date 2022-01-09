obj-m := prime_module.o

CONFIG_MODULE_SIG=n
KDIR := ../../../src/linux-5.15.4/

PWD := $(shell pwd)

all: prime_module.c
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

