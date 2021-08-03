# Openwrt Hotplug

+ [Overview](#overview)
+ [Block事件](#block)

-----

<h1 id="overview">Overview</h1>

當某些事件發生時，procd會執行位於 `/etc/hotplug.d/` 中的腳本，procd不知道要怎麼處理hotplug事件，也沒必要知道，因為他只實現機制，不實現策略，事件的處理是由配置文件`/etc/hotplug.json`決定的。

打開`/etc/hotplug.d`資料夾，你會發現下面還有數個子資料夾，每個資料夾皆存放了相關功能的腳本

<h2 id="uevent">Uevent</h2>

hotplug的過程會分為兩部分:
1. kernel發出uevent事件
	- kernel透過socket來發送uevent，事件的相關訊息會透過環境變數來傳遞，如`$ACTION $DEVPATH $SUBSYSTEM`
2. user space監聽uevent
	- 收到uevent後，根據`/etc/hotplug.json`的描述去呼叫`/sbin/hotplug-call`並帶入不同的參數，再根據`$SUBSYSTEM`到`/etc/hotplug.d/`下的資料夾去依序執行腳本。

--------

配置外部磁碟空間步驟
1. verify storage drivers
2. 驗證OS是否能夠辨識連接的磁碟
3. 在USB磁碟建立partition
4. 在partition中建立filesystem
5. 自動掛載partition


<h1 id="fstab">Fstab</h1>

`fstab`是一個中央配置檔案，用來定義file system(通常為block device)應該如何被monut起來。
fstab UCI子系統定義了需要安裝的所有設備和文件系統的所有選項，位於`/etc/config/fstab`中，Default下，該文件不會存在，需要自行創建

安裝模組block-mount來快速設定
```
opkg update
opkg install block-mount
block detect | uci import fstab
```

conf由global section作為預設值，每當修改fstab設定時，執行以下指令:
```
block umount
block mount
```

UCI語法:

> mount section用來定義要被mount的fs，swap section用來定義partition要被啟動的(類似virtual memory)

<h3>global section</h3>

|option|型態|require|default|Description|
|---|---|---|---|---|
|anon_swap|bool|no|0|掛載沒有自己配置的swap device|
|anon_mount|bool|no|0|掛載沒有自己配置的block dev|
|auto_swap|bool|no|1|當swap device出現時自動掛載|
|auto_mount|bool|no|1|同上|
|delay_root|int|no|0|在掛載root前等待X秒|
|check_fs|bool|no|0|每次mount前執行e2fsck|

<h3>swap section</h3>

|option|型態|require|default|Description|
|---|---|---|---|---|
|device|str|no|-|The swap partition’s device node (e.g. sda1)|
|uuid|str|no|-|他的UUID|
|label|str|no|-|-|

<h3>mount section</h3>
|option|型態|require|default|Description|
|---|---|---|---|---|
|enable|bool|no|1|啟用/關閉UCI設定|
|device|str|no|-|The data partition's device node (e.g. sda1)|
|uuid|str|no|-|他的UUID|
|label|str|no|-|-|



