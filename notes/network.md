# Network (/etc/config/network)

---------------------------

> 本文摘自 https://openwrt.org/docs/guide-user/base-system/basic-networking 並省略了一些細節
> 

UCI將config儲存在`/etc/config`下，而network這個uci子系統負責定義`switch VLANS` `interface configurations` 和 `network routes` 

在修改network設定後 必須經由守護進程`netifd`重新載入
```bash
service network reload
```
或是藉由啟動腳本重新載入
```bash
/etc/init.d/network reload
```
## Netifd

在`netifd`中定義了`Device` `interface` `proto handler` 

而作為一個路由器使用者我們只需關心配置Interface層，創建一個interface並指名其依賴的Device，及綁定上網方式(proto handler)，就完成網路設定可以開始工作，當網路狀態發生改變時，三者也能互相通知。

### 一個LAN interface的範例設定
在UCI下顯示:
```
network.lan=interface
network.lan.type='bridge'
network.lan.ifname='eth0.1'
network.lan.proto='static'
network.lan.netmask='255.255.255.0'
network.lan.ip6assign='60'
network.lan.ipaddr='192.168.1.1'
```
在Config File裡:
```
config interface 'lan'
        option type 'bridge'
        option ifname 'eth0.1'
        option proto 'static'
        option netmask '255.255.255.0'
        option ip6assign '60'
        option ipaddr '192.168.1.1'
```

* Netifd透過讀取以上config創建實例，並套用設定，如果指定的依賴設備不存在(ifname)，則透過此配置中device相關欄位創建新的device。
* 不同於Device，proto必須在Netifd啟動時就預先定義好，config中的proto欄位必須是已經存在的proto handler名稱。
* 實際上一個最小的interface定義只需如下:

```
config 'interface' 'wan'
        option 'proto' 'dhcp'
        option 'ifname' 'eth0.2'
```
### Proto欄位選項

| Proto | Description |
| :--- | :---------- |
|static | 固定的位址和網路遮罩 |
|dhcp|動態主機設定協議|
|dhcpv6|IPv6的動態主機設定協議|
|ppp|點等協定|
|pppoe|乙太網路中封裝PPP|
|pppoa|非同步對等協定|
|3g|第三代行動通訊協定|
|...|etc|
|none|未指定的protocol 忽略其他的interface設定，相當於disable|

根據interface protocol，可能還需要聲明以下參數

| name | type | Default | Description |
| ---- | ---- | ---- | ---- |
|ifname|interface name|none|如果設置為bridge 則此欄位為list，WLAN interface是動態或不可預測的，因此建議在無線配置中分配給bridge|
|types|string|none| |
| stp |bool|0|bridge限定，啟動Spanning Tree Protocol|
|bridge_empty|bool|0|bridge限定，創建空的橋接器|
|igmp_snooping|bool|0|bridge限定，multicast_snooping kernel setting |
|multicast_querier|bool|看igmp|bridge限定，multicast_querier  kernel setting|
|macaddr|mac addr|none|覆寫mac address|
|mtu|number|none|覆寫MTU|
|auto|bool|proto為none時0，其他1|指定是否在啟動時創建interface|
|ipv6|bool|1|是否套用ipv6|

## WAN








