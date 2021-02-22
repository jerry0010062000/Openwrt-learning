# Procd
==========================================================

+ [什麼是Procd?](#introduction)
+ [Procd初始化流程](#init)
	- [初始化_preinit階段](#preinit)
	- [初始化_procd正式啟動與inittab](#procd_state)
+ [自製init腳本](#DIY)
	- 

----------------------------------
<h3 id="introduction">Procd是什麼?</h3>

Procd是一個用C語言編寫的守護行程(daemon)，他持續追蹤那些透過init script啟動的process，並在config/environment改變時啟動或重啟服務。

----------------------------------------------------------------------------------------------
<h3 id="init">系統開機流程</h3>
Openwrt從kernel_start()完成後，執行preinit和init到Procd取代流程如下:
<div align=center><img src="image/init-img.png" width="" height="" alt="init-proc"/></div>
> [參考自這裡](https://dongshao.blog.csdn.net/article/details/102767797)

---------------------------------------------------------------------------------------------
<h2 id="preinit">初始化_preinit階段</h2>

> [官方preinit頁面](https://openwrt.org/docs/techref/preinit_mount#development)

雖然kernel啟動結束後會先執行preinit.sh，但由於變數尚未設定，會直接執行init主要動作如下
	- bring up basic mounts如 /proc /sys /dev
	- 創建一些必須的資料夾如 /tmp
	- bring up /dev/console並打印消息
	- 設置PATH環境變數
	- 檢查init_debug是否被設置
	- 初始化watchdog

接著Init總共創建出三支子程式執行任務 分別是 `kmodloader` `procd` `preinit.sh` 

+ Kmodloader維護了一個AVL tree並執行了以下動作

  - 開啟`/proc/modules`文件中記錄已安裝的模組插入AVL tree中並設為LOADED
  - 掃描`/lib/modules/核心版本/*.ko`判斷外部模組是否在AVL中，否則加入並設為SCANNED
  - 掃描`/etc/modules-boot.d/`將數字開頭的檔案由小到大循序載入，其他設為PROBE

+ 在Init中 fork出來的procd代入參數`/etc/hotplug-preinit.json`執行兩項檢測動作
  - 韌體升級uevent 執行腳本`/sbin/hotplug-call`加載`/lib/firmware`下的升級
  - 判斷uevent "SUBSYSTEM"為button，執行`/etc/rc.bottom/failsafe`建立檔案`/tmp/failsafe-button`

+ preinit.sh定義了五個hook後執行`/lib/preinit/`目錄下腳本，每個腳本定義了一個function並且將其掛到五個hook上，分別是:
```bash
>#hook
>preinit_essential
>preinit_main
	>>#script
    >>define_default_set_state
    >>preinit_ip	
    >>pi_indicate_preinit
    >>do_sysinfo_generic
    >>failsafe_wait    		#判斷failsafe-button是否存在 有則設置變數FAILSAFE為真
    >>run_failsafe_hook    	#若FAILSAFE為真則執行hook failsafe
    >>indicate_regular_preinit
    >>initramfs_test
    >>do_mount_root
    >>run_init

>failsafe
    >>indicate_failsafe
    >>failsafe_netlogin
    >>failsafe_shell

>initramfs
>preinit_mount_root
```
實際上只執行`preinit_essential`和 `preinit_main`

`/lib/preinit/00_preinit.conf`是由preinit根據base-file所產生的，可經由make menuconfig設定
其中`pi_ifname`、`pi_ip`、`pi_broadcast`、`pi_netmask`是為了在preinit期間發送訊息而設置

> path : Image Configuration ->[Y]preinit configuration options 


HOOK `preinit_essentials`原本是用來掛載必要的filesystem如proc和初始化console，但在後來的版本被procd取代

在Openwrt原生系統中，將檔案系統分成兩部分`rootfs`和`rootfs_data`(詳見etc)
在kernel啟動階段掛載唯讀`rootfs`到`/rom`並作為`/`，而在preinir階段才將可讀寫`rootfs_data`掛載到`/overlay`並透明掛載到`/`上，之後對檔案有寫入刪除動作皆是記錄在`rootfs_data`上。

Failsafe模式的進入點在掛載`rootfs_data`之前，可以確保其與/rom的一致性

如果沒有進入failsafe模式的話，結束`preinit.sh`回到init，執行callback function結束init，由procd取代他，成為pid = 1的process。

---------------------

<h2 id="procd_state">初始化_procd正式啟動與inittab</h2>

---------------------

<h2 id= "DIY">如何製作init腳本</h2>

必須放在`/etc/init.d/`
init腳本模板由rc.common提供，需在script前加上
```shell
#!/bin/sh /etc/rc.common
USE_PROCD=1
```
此腳本用途主要在，定義啟動instacnce時所需配置，以及定義重新啟動服務

## rc.common定義函數

|Function|Description|
|-----|--------------------|
|start_service()|向Procd註冊並啟動服務|
|stop_service()|停止正在運行的服務|
|service_trigger()|config或interface改變時觸發重新讀取|
|service_runnung()|查詢服務狀態|
|reload_service()|如果定義此函數，trigger時啟動此函數，否則調用start|
|service_started|判斷process是否啟動成功|

### trigger函數使用方法
```c
service_triggers()
{
        procd_add_reload_trigger "<uci-file-name>" "<second-uci-file>"
        procd_add_reload_interface_trigger <interface>
}
```
兩者皆使用`/etc/init.d/<foo> reload`作為handler

openwrt也支援重啟信號`SIGHUP`傳遞用法如下
```C
reload_service() {
         procd_send_signal service_name [instance_name] [signal]
}
//其中service_name為 init.d下腳本名稱
//instance_name允許自訂義實例名稱，若使用*視為未指定，發給服務所有實例
```


----------------

`procd_set_param`可用參數列表

|Parv|Type|Description|
|---|---|-----------|
|env|key-value list|設定環境參數key=value給產生出的process|
|data|key-value list|設置給ubus service status，主要用於儲存額外meta data服務|
|limits|key-value list|設置資源限制|
|command|list|設置vector來啟動程式|
|netdev|list|將linux網路設備名稱給procd以監視改變，當啟動reload時，如果network device interface index有被改變時，傳遞信號|
|file|list|將文件名給procd監控，當reload時檢查到checksum被改變時向其傳遞信號|
|respawn|list|三個數字用空格隔開，分別代表重啟秒數--------|
|watch|list|list ubus namespace ，procd將訂閱他們，當有ubus事件將會比對已註冊的JSON腳本觸發器|
|error|list||
|nice|int|設定重啟優先權(最高)-20~19(最低)|
|term_timeout|int|收到TERM SIGNAL後到完全結束等待秒數，如過時間到還未結束則發出KILL SIGNAL，default=5|
|stdout|bool|如果為真，procd將生成process的stdout設置到 system log中，LOG_INFO|

