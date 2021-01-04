# littlebee
基于x86自研学习开发操作系统

### 依赖环境
虚拟机 Bochs<br>
汇编编译器 NASM<br>

下载NASM https://www.nasm.us/pub/nasm/releasebuilds/2.14.02/nasm-2.14.02.tar.gz<br>

安装NASM
```
tar xzvf nasm-2.14.02.tar.gz
cd nasm-2.14.02
./configure
make 
make install 
```
配置Bochs<br>
新建文件bochsrc.bxrc
```
# Tell  bochs to use our  boot  sector  code as  though  it were
# a floppy  disk  inserted  into a computer  at boot  time.
romimage: file=$BXSHARE\BIOS-bochs-latest, address=0xf0000
megs: 16
vgaromimage: $BXSHARE\VGABIOS-lgpl-latest

#floppya: 1_44=boot_sect.bin , status=inserted
floppya: 1_44=os-image , status=inserted
boot: a
```

内核调试Bochs配置
```
"\Bochs-2.2.pre4\bochsdbg" -q -f bochsrc.bxrc
```
