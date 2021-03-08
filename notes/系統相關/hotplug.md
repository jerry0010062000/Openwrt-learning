# Hotplug & Uevent

> [linux的統一設備模型](https://www.binss.me/blog/sysfs-udev-and-Linux-Unified-Device-Model/)、
> [openwrt中設備節點的自動創建](https://blog.csdn.net/agave7/article/details/96181789)，
> 參考自[OpenWRT Hotplug原理分析](https://github.com/wywincl/hotplug)
> 

+ [Overview](#overview)
+ [Hotplug](#hotplug)
+ [Uevent](#uevent)

--------

<h1 id="overview">Overview</h1>

當某些events發生時，Procd 會執行位於`/etc/hotplug.d/`中的腳本，eg.interface up/down，檢測到新的儲存驅動時，或者按下buttom時。該功能模組模擬/擴展了以淘汰的hotplug2的功能



-------

<h1 id="hotplug">Hotplug</h1>
在`/etc/hotplug.d`資料夾中包含了block、iface、net、ntp...等資料夾，觸發事件發生後，Procd將按照字母順序執行子資料夾的所有腳本
	- block塊設備(塊設備 up/down)
	- iface interface(WAN、LAN連接關閉時)
	- net 與網路相關事件
	- ntp 時間同步事件
	- button 按鈕事件(預設不建立，/etc/button代替)
	- usb 類似3g-modem和tty的USB設備

在執行hotplug腳本時，procd利用環境變數來提供大量訊息

>塊設備:系統中能夠隨機訪問固定大小數據片的設備稱為塊設備，常見的塊設備為硬碟、軟體驅動器、CD-ROM和快閃記憶體
>字符設備:相對於塊設備，字符設備按照字符流有序進行訪問，如鍵盤。

---

<h1 id="uevent">Uevent</h1>

Procd屬於user space process，與uevent所屬的kernel space必須透過netlink傳遞訊息
1. kernel發出uevent
	uevent首先調用`netlink_kernel_create()`創建socket，`kobject_uevent()`產生uevent事件，事件的部分訊息透過環境參數傳達(如`$ACTION`,`$DEVPATH`)，產生的uevent由`netlink_broadcast_filtered()`發出，最後依照`uevent_helper[]`指定的程式來完成
2. user監聽
	在`proc/plug/hotplug.c`中，創建PF_NETLINK socket來監聽uevent，收到後根據`/etc/hotplug.json`來執行對應的函數。通常情況下，會調用`/sbin/hotplug-call`來處理，他根據`$SUBSYSTEM`分別執行`/etc/hotplug.d`下的腳本。

設備節點是為設備驅動所創建的，而device和driver都是以chain的形式連接在匯流排上的，而設備-驅動-匯流排的更上層就是sysfs

#### sysfs

sysfs基於內存的虛擬文件系統，由kernel提供，掛載到`/sys目錄底下`，負責以設備樹的形式向user namespace提供直觀的設備與驅動資訊，同時/sysfs以不同的視角向我們展示當前系統接入的設備
	- /sys/block 存放塊設備，提供以設備名(sda)到/sys/devices的符號連接
	- /sys/bus按照匯流排類型分類，在某個匯流排目錄下可以找到連接到該匯流排的所有設備
	- /sys/class按照設備的功能分類
	- /sys/dev按照設備的驅動分類(字符/塊)
	- /sys/device包含所有被發現在匯流排上的物理設備。所有物理設備都按照其在BUS上的拓樸結構來顯示

上述的目錄關係為
- device用來描述各種設備，有各個設備的訊息
- driver用於驅動device，保存了能夠被他所驅動的設備鍊表
- bus是連接CPU與device的橋樑，保存了所有掛載在上面的設備鍊表與驅動鍊表

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






