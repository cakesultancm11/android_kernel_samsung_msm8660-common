obj-m += kcontrol_gpu_msm.o

kcontrol_gpu_msm-y	:= kcontrol_module_msm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CROSS_COMPILE)strip --strip-debug kcontrol_gpu_msm.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f modules.order *~ *.o
