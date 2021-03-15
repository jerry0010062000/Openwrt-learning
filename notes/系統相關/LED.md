

# Openwrt LED

----

+ [Overview](#overview)
+ [簡單操作](#easy)
+ [LED的UCI操作](#uci)
+ [Example](#example)

----

<h1 id="overview">Overview</h1>

在Openwrt中，除了電源(PWR)是有電就亮，及一些Ethernet狀態(WAN & LAN)是Ethernet controller直接控制以外，其他LED是接到SoC的GPIO，Linux Kernel有個platform子系統，並有一些針對GPIO的現成驅動程式。例如:leds-gpio用來控制LED，只要把GPIO的位置，所使用的驅動程式以及功能在編譯前設定好，編譯後的kernel就能支援GPIO

-----

<h1 id="easy">直接操作</h1>

每個LED都有一個個別的檔案系統目錄，直接進入`/sys/class/leds`底下，以`<路由器名稱>:顏色:<功能>`命名的目錄均對應一個LED，進入要操作的目錄，`trigger`(觸發方式)、`brightness`(開關)、`delay_off`(熄燈時間)、`delay_on`(亮燈時間)

對文件brightness寫入0/1，關閉/打開LED燈，如echo 0 > brightness
```bash
#閃爍
echo timer > trigger
echo 1000 > delay_on
echo 1000 > delay_off
```

範本:關閉所有LED腳本
```bash
#!/bin/ash
for i in `ls /sys/class/leds` 
do cd /sys/class/leds 
cd $i 
echo 0 > brightness
done
```

----

<h1 id="uci">LED的UCI操作</h1>

UCI文檔位於`/etc/config/system`

LED啟動腳本:`/etc/init.d/led`

LED可以被各種事件操作，依賴`trigger`option選擇，根據不同的trigger，LED會提供額外的option，我們可以藉由目錄底下的trigger檔案來查看trigger可以填入的值
```
#example
root@lede:/# cat /sys/class/leds/tp-link:green:qss/trigger
[none] switch0 timer default-on netdev usbdev phy0rx phy0tx phy0assoc phy0radio phy0tpt 
```

Trigger可被填入的原生系統參數如下 : [none](#none)、[switch0](#switch)、[timer](#timer)、[heartbeat](#heartbeat)、[nand-disk](#flashwrite)、[netdev](#netdev)、[WiFi Activity triggers](#wifi)、[usbdev](#usb)、


<h2 id ="none">none</h2>

> LED始終處於deafult狀態，未列出的預設為OFF，因此僅在LED始終為ON時有用
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|裝置名稱|



<h2 id ="switch">switch0</h2>
> 如果已在已配置的switch port上建立連結，則該指示燈發亮

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|裝置名稱|
|port_mask|int|no|0|16進位mask，用來定位特定的port|
|speed_mask|int|no|none|16進位mask用來過濾ethernet速度|

`port_mask`例子0x1e二進制為000111110，從右到左，不包括CPU，包括4個switch port，並將其餘位置設置為0
`port_speed`以標準BASE-T來分配

<h2 id ="timer">timer</h2>
> 配置LED閃爍頻率，必須包含`kmod-ledtrig-timer`
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|裝置名稱|
|delayoff|int|yes|none|LED要關閉多久(millisecond)|
|delayon|int|yes|none|LED要打開多久(millisecond)|

<h2 id ="heartbeat">heartbeat</h2>

> LED會模擬心跳，必須安裝`kmod-ledtrig-heartbeat`

<h2 id ="flashwrite">nand-disk</h2>
> 當寫入NAND flash時LED閃爍

<h2 id ="netdev">netdev</h2>
> 當被配置的interface傳送或接收時，LED會閃爍
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|LED裝置名稱|
|dev|string|yes|none|network interface名稱|
|mode|string|yes|no|none|tx,rx|
|interval|int|no|閃爍的間隔(milliseconds)|

<h2 id ="wifi">WiFi Activity triggers</h2>
> LED只會被物理界面觸發，注意trigger不是填標題名稱，可填入值在下
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|裝置名稱|

trigger可填入的值
+ phy0rx - 接收時閃爍
+ phy0tx - 傳輸時閃爍
+ phy0assoc - client連上時閃爍
+ phy0radio 
+ phy0tpt - 與tr、tx模式相比 緩慢但穩定的閃爍

<h2 id ="usb">usbdev</h2>
> LED會在USB連接時閃爍，必須安裝`kmod-ledtrig-usbdev`
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|
|sysfs|string|yes|none|裝置名稱|
|trigger|string|yes|none|usbdev或usbport|
|interval|int|yse|none|裝置active時的間隔(ms)|
|dev|string|yes|none|USB裝置名稱|

#### GPIO
> 由GPIO來控制，必須安裝`kmod-ledtrig-gpio`
> 

|Name|Type|require|default|Description|
|---|---|---|--|--------|
|default|int|no|0|在trigger前狀態0:off;1:on|

#### Net filter
> 當有LED通過機器時閃爍，必須安裝`kmod-ipt-led`

-------

<h1 id="example">Example</h1>

heartbeat led
```
config 'led'
	option 'sysfs'		'wrt160nl:amber:wps'
	option 'trigger'	'heartbeat'
```

WLAN led
```
config 'led' 'wlan_led'
	option 'name'           'WLAN'
	option 'sysfs'          'tl-wr1043nd:green:wlan'
	option 'trigger'        'netdev'
	option 'dev'            'wlan0'
	option 'mode'           'link tx rx'
```

3G LED

```
config 'led'
	option 'name'           '3G'
	option 'sysfs'          'asus:blue:3g'
	option 'trigger'        'netdev'
	option 'dev'            '3g-wan'
	option 'mode'           'link'
```

Timer LED
```
config 'led'
	option 'sysfs'		'wrt160nl:blue:wps'
	option 'trigger'	'timer'
	option 'delayon'	'500'
	option 'delayoff'	'2000'
```
