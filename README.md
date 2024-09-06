
#### filter driver.

- **charfdo 实例的功能是：**
	- 一次将一个数字（0～9）转换成一个中文（零～九）。
- **FilterSample 实例的功能是：**
	- 一次可接收多个数字（0～9），然后一个一个传递给charfdo，待所有的数字转换完毕后，完成一次操作。
- **Test_FilterSample 实例的功能是：**
	- 下发CharSample_IOCTL_800 IRP到FilterSample处理后传输给FDO;
	- 下发CharSample_IOCTL_801 IRP到FilterSample不处理，直接传输给FDO;
	- 获取转换后的字符串，并转换大小端。


#### install.  

# 微软pnp驱动安装器(nopnp),用于测试程序,设备管理器中可以看到设备安装情况
hdwwiz.exe # 安装pnp设备: Next --> Advanced --> Next(Show All Devices) --> Have Disk --> Ok

#### test.

Test_FilterSample.exe

#### ref.

filter_sample\x64\Release\package

filter and fdo:

FilterSample.inf

fdo:

charfdo.inf

filter:

todo...