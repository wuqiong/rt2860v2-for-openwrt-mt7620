rt2860v2-for-openwrt-mt7620
===========================

rt2860v2_ap/rt2860v2_sta driver for openwrt trunk(3.10.x kernel) on MT7620 SOC. (Test on mt7620n 16m flash + 64M ram)



port and build steps:

1. go and get the sources of openwrt trunk:

   $mkdir -p ~/openwrt
   $cd ~/openwrt
   $git clone git://git.openwrt.org/openwrt.git  openwrt-trunk
   $cd openwrt-trunk
   $./scripts/feeds update -a
   $./scripts/feeds install -a

2. defconfig and make:

   $make defconfig
   $make menuconfig
   
   -----------------------------------------------
   -------select the following configuration------
   -----------------------------------------------
   Target System: (Ralink 288x/3xxx)
   Subtarget: (MT7620N based boards)
   
   $make V=99
   
   wait make command to be done.
   when make finished. the firmware will be in the directory: $HOME/openwrt/openwrt-trunk/bin/ramips/
   then, your can download burn the new firmware to the mt7620 SOC Board via TFTP in the uboot interface.
   but, unlikely, that default configuraion don't has wifi support, so we cann't startup a Hotspot on our
   MT7620 SOC for incoming connections from Smartphones and laptops.
   
3. all steps are the default configuarions from the openwrt official site.
   and this step will guide you to port rt2860v2_ap driver to the kernel tree and build it to fix that problems.
   
   $cd ~/
   $git clone https://github.com/wuqiong/rt2860v2-for-openwrt-mt7620.git
   $cp -r rt2860v2-for-openwrt-mt7620/ralink ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/
   $cp -r rt2860v2-for-openwrt-mt7620/rt2860v2  rt2860v2-for-openwrt-mt7620/rt2860v2_ap  ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/
   
   after copied the driver code to the kernel build tree. then, let's to add Kconfig and Makefile objs. in each directory of "mtd" and "wireless".
   add the following line before the end of file: ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/Kconfig
    
    source "drivers/mtd/ralink/Kconfig"

  add the following line before the end of file: ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/mtd/Makefile
  
  obj-$(CONFIG_MTD_RALINK)    += ralink/
  
  add the following line before the end of file: ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/Kconfig
  
  source "drivers/net/wireless/rt2860v2_ap/Kconfig"
  
  
  add the following line before the end of file: ~/openwrt/openwrt-trunk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/linux-ramips_mt7620n/linux-3.10.28/drivers/net/wireless/Makefile
  
  obj-$(CONFIG_RT2860V2_AP)   += rt2860v2_ap/
  
  
  
  
4. Okey, configuration finished. go back to the ~/openwrt/openwrt-trunk directory. and run shell command:
    $make kernel_menuconfig

    enter this menu:
    A:  Device Driver -->  <*> Memory Technology Device (MTD) support  ---> Ralink Flash Type support  --->  [*] Ralink MTD support  
    B:  Device Driver -->  [*] Network device support  --->  [*]   Wireless LAN  --->
        <*>   Ralink RT2860 802.11n AP support                                                               
            [*]     LED Support                                                                                  
            [ ]     WSC (WiFi Simple Config)                                                                     
            [*]     LLTD (Link Layer Topology Discovery Protocol)                                                
            [*]     WDS                                                                                          
            [*]     MBSSID                                                                                       
            [*]       New MBSSID MODE                                                                            
            [*]     AP-CLient Support                                                                            
            [*]       MAC Repeater Support                                                                       
            [ ]     IGMP snooping                                                                                
            [ ]     NETIF Block                                                   
            [ ]     DFS                                                          
            [*]     Carrier Detect                                                
            [ ]     DLS ((Direct-Link Setup) Support                            
            [ ]     IDS (Intrusion Detection System) Support                                                     
            [*]     CoC Support                                                                                  
            [ ]     Memory Optimization                                                                          
            [ ]     Video Turbine support                                                                        
            [ ]     802.11n Draft3                                                                               
            [ ]     Adjust Power Consumption Support                                                             
            [ ]     Single SKU                                                                                   
                  Choose Power Design (Internal PA and Internal LNA)  --->  
            [ ]   TSSI Compensation                                            
            [ ]   Temperature Compensation
  
  
  
       we select "*" to building the driver  into the kernel, not a kernel module.
       after selecting the configration. save and exit menuconfig.
  
5.  Now. we have add and configured the wifi driver to the kernel build tree.  just type the make V=99 command in directory: ~/openwrt/openwrt-trunk , and wait for some minutes.
6.  after builded, download and burn the new firmware with wifi driver. you can found that, wifi device ra0 will appear. but your cann't ifconfig ra0 up. because we need lack the etc/Wireless/RT2860/RT2860.dat file and /etc_ro directory, also an scripts file in /lib/wifi directory which first introduced by lintel.
7.  enjoy diy with your hands. I will not provide the dat file and script file. you need to scratch up those file by yourself.
8.  no thanks. but welcome to contact me.  www.diveinedu.cn
              
