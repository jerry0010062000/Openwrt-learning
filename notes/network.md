# Network (/etc/config/network)

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
作為一個路由器使用者我們只需關心配置Interface層，創建一個interface並指名其依賴的Device，及綁定上網方式(proto handler)，就完成網路設定可以開始工作，當網路狀態發生改變時，三者也能互相通知。

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




