
#### filter driver.

- **charfdo ʵ���Ĺ����ǣ�**
	- һ�ν�һ�����֣�0��9��ת����һ�����ģ��㡫�ţ���
- **FilterSample ʵ���Ĺ����ǣ�**
	- һ�οɽ��ն�����֣�0��9����Ȼ��һ��һ�����ݸ�charfdo�������е�����ת����Ϻ����һ�β�����
- **Test_FilterSample ʵ���Ĺ����ǣ�**
	- �·�CharSample_IOCTL_800 IRP��FilterSample��������FDO;
	- �·�CharSample_IOCTL_801 IRP��FilterSample������ֱ�Ӵ����FDO;
	- ��ȡת������ַ�������ת����С�ˡ�


#### install.  

# ΢��pnp������װ��(nopnp),���ڲ��Գ���,�豸�������п��Կ����豸��װ���
hdwwiz.exe # ��װpnp�豸: Next --> Advanced --> Next(Show All Devices) --> Have Disk --> Ok

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