rt2860v2-for-openwrt-mt7620
===========================

rt2860v2_ap/rt2860v2_sta driver for openwrt trunk(3.10.x kernel) on MT7620 SOC. (Test on mt7620n 16m flash + 64M ram)




###1. go and get the sources of openwrt trunk:

```shell
$mkdir -p ~/openwrt

$cd ~/openwrt

$git clone git://git.openwrt.org/openwrt.git  openwrt-trunk

$cd openwrt-trunk

$./scripts/feeds update -a

$./scripts/feeds install -a
```
###2. defconfig and make:

```shell
$make defconfig

$make menuconfig
```
#	   -------Select the following configuration------

	Target System: (Ralink 288x/3xxx)
	Subtarget: (MT7620N based boards)

```shell
$make V=99
```
   Wait make command to be done.

   When make finished.
   The firmware will be in the directory: `$HOME/openwrt/openwrt-trunk/bin/ramips/`

   Then, you can download and burn the new firmware to the mt7620 SOC Board via TFTP in the uboot stage.

   But, unfortunately, that default configuraion don't has wifi driver support,

   So We cann't startup a Hotspot on our MT7620 SOC for incoming connections from Smartphones and laptops.

   All steps above are the default configuarions from the openwrt official site.

##3.Now, I will guide you to port rt2860v2_ap driver to the kernel tree and build it to fix that problem.

```shell
$cd ~/

$git clone https://github.com/wuqiong/rt2860v2-for-openwrt-mt7620.git

$cp -r rt2860v2-for-openwrt-mt7620/ralink ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/

$cp -r rt2860v2-for-openwrt-mt7620/rt2860v2  rt2860v2-for-openwrt-mt7620/rt2860v2_ap  ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/
```
   After copied the driver code to the building kernel  tree. 
   Then, Let's to add Kconfig and Makefile objs in each directory of "mtd" and "wireless".

   Add the following line before the end of Kconfig file:` ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/Kconfig`

```shell
source "drivers/mtd/ralink/Kconfig"
```
  Add the following line before the end of makefile:` ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/Makefile`

```shell
  obj-$(CONFIG_MTD_RALINK)    += ralink/
```
------------------------------------------------------------------------------------------------------------------
  Add the following line before the end of Kconfig file: ` ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/Kconfig`
```shell
  source "drivers/net/wireless/rt2860v2_ap/Kconfig"
```

  Add the following line before the end of makefile: `~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/Makefile`

```shell
  obj-$(CONFIG_RT2860V2_AP)   += rt2860v2_ap/
```


##4. Okey, Driver Code Configuration Finished. 
Go back to the `~/openwrt/openwrt-trunk` directory.
And run the shell command:
```shell
    $make kernel_menuconfig
```

#####Enter this menu:

	A:
		Device Driver -->  
        				<\*> Memory Technology Device (MTD) support  
        					---> Ralink Flash Type support  
                            	--->  [\*] Ralink MTD support  

------------------------------------------------------------------------------------------------------------------
	B:
    Device Driver
    	-->  [\*] Network device support
        	--->  [\*]   Wireless LAN
            	---><\*>   Ralink RT2860 802.11n AP support
			            [\*]     LED Support
            			[ ]     WSC (WiFi Simple Config)
            			[\*]     LLTD (Link Layer Topology Discovery Protocol)
            			[\*]     WDS
            			[\*]     MBSSID
            			[\*]       New MBSSID MODE
            			[\*]     AP-CLient Support
            			[\*]       MAC Repeater Support
            			[ ]     IGMP snooping
            			[ ]     NETIF Block
            			[ ]     DFS
            			[\*]     Carrier Detect
            			[ ]     DLS ((Direct-Link Setup) Support
            			[ ]     IDS (Intrusion Detection System) Support
            			[\*]     CoC Support
            			[ ]     Memory Optimization
            			[ ]     Video Turbine support
            			[ ]     802.11n Draft3
            			[ ]     Adjust Power Consumption Support
            			[ ]     Single SKU
                  			Choose Power Design (Internal PA and Internal LNA)  --->
            			[ ]   TSSI Compensation
            			[ ]   Temperature Compensation





We select "*" to building the driver into the kernel, not a kernel module.

After selecting the configration. save and exit menuconfig.

##5.Now. Just Hit "Make V=99" to build.
we have added and configured the wifi driver code to the kernel build tree.

just type the `make V=99` command in directory:` ~/openwrt/openwrt-trunk` ,
```shell
$make V=99
```
And wait for some minutes to be builded.

##6.  After builded, burn the new firmware with wifi driver.
You will found out that, wifi device ra0 will be appeared.

test with:
```shell
ifconfig ra0
```
But your cann't successfully ifconfig ra0 up.

Because we lack the `/etc/Wireless/RT2860/RT2860.dat` file(or symbol link to `/tmp/RT2860.dat`),

Which generated by an scripts file in `/lib/wifi` directory which first introduced by lintel.

##7.  Enjoy DIY with your hands.
~~I will not provide the dat file or script file. you need to scratch up those file by yourself.~~

Last commit, I havn't shared the script file.

This Time, I shared the script file named: `rt2860v2.sh`.

I fixed the wrong default mac address problem via shell command in this script file, you know, it's not the best, but works well.

Your can define your perefered mac addres in `/etc/config/wireless` file with `option macaddr '1a:1a:1a:1a:1a:1a'`;

Or you can read out the factory default mac address in eeprom, using this shell:
```shell
hexdump -s 4 -n 6 -C /dev/mtd2 | head -n 1 | sed 's/\ \ /:/g' | cut -d: -f 2 | sed 's/\ /:/g'
```
substitute and eval this shell in the appropriate place in `rt2860v2.sh` file. you can get everything automatically.

####Where is the best and perfect firmware?
####I will not to Share out any more.
####Dive into the code and fix it by yourself, it's so easy.


##8.  Say thanks to me as much as you can.
sorry for these Chinglish, wish you can read it.


* * *

#Welcome to Contact Me.

#大茶园丁 QQ:147957232

#Welcome to introduce students to my college:

#The best iOS and Linux Developer College in China.

#Dive in Education！ === [WWW.DiveInEdu.Com](http://www.diveinedu.com)


