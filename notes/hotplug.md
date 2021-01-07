# Hotplug 

> [linux的統一設備模型](https://www.binss.me/blog/sysfs-udev-and-Linux-Unified-Device-Model/)
> [openwrt中設備節點的自動創建](https://blog.csdn.net/agave7/article/details/96181789)
> 參考自[OpenWRT Hotplug原理分析](https://github.com/wywincl/hotplug)

Procd屬於user space process，與uevent所屬的kernel space必須透過netlink傳遞訊息
1. kernel發出uevent
	uevent首先調用`netlink_kernel_create()`創建socket，`kobject_uevent()`產生uevent事件，事件的部分訊息透過環境參數傳達(如`$ACTION`,`$DEVPATH`)，產生的uevent由`netlink_broadcast_filtered()`發出，最後依照`uevent_helper[]`指定的程式來完成

2. user監聽
	在`proc/plug/hotplug.c`中，創建PF_NETLINK socket來監聽uevent，收到後根據`/etc/hotplug.json`來執行對應的函數。通常情況下，會調用`/sbin/hotplug-call`來處理，他根據`$SUBSYSTEM`分別執行`/etc/hotplug.d`下的腳本。
	
### /etc/hotplug.d/

|資料夾|Description|
|---|------|
|block|Block device events (block device connected/disconnected)|
|button|按鍵事件，/etc/rc.button產生|
|iface|interface events(LAN、WAN連線)|
|net|網路相關事件|
|ntp|同步時間事件|
|usb|USB裝置(3g-modem、tty)|

在執行hotplug腳本時，procd利用環境變數來提供大量訊息

### iface
```bash
ACTION				#“ifup”, “ifdown”, “ifupdate”
INTERFACE			#Name of the logical interface which went up or down (e.g. “wan” or “ppp0”)
DEVICE				#Physical device name which interface went up or down (e.g. “eth0.1” or “br-lan”)
IFUPDATE_ADDRESSES		#“1” if address changed
IFUPDATE_PREFIXES		#“1” if prefix updated FIXME what constitutes an update?
```
一個自己的腳本範例
```shell
cat << "EOF" > /etc/hotplug.d/iface/99-my-action
[ "${ACTION}" = "ifup" ] && {
    logger -t hotplug "Device: ${DEVICE} / Action: ${ACTION}"
} 
EOF
```






