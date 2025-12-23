obj-m += tsulab.o # [cite: 25]
PWD := $(CURDIR)  # [cite: 26]

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules # [cite: 28, 29]

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean # [cite: 31, 32]