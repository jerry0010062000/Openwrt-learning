# Linux routing table

+ [Overview](#overview)
+ [IP指令](#ipcommand)
	- [文法](#ipgrammer)
	- [IP link](#iplink)
	- [IP address](#ipaddress)
	- [IP neighbour](#ipneigh)

-----

<h1 id="overview">Overview</h1>

Router最主要的工作是決定要將封包送往何處，而為了達到這個目的，Router必須要做到以下事情

1. 知道目的地端的地址在哪裡
2. 標示出來源端的位址，並學習
3. 尋找這個封包可能要送網的路徑有哪些
4. 從可能的路徑中選出最佳路徑
5. 維護並更新這些路由所需的資料

查看routing table
```
route -n
```
|欄位|意義|
|---|---|
|Destination|目標網段或主機|
|Gateway|閘道器位址|
|Genmask|網路掩碼|
|Flags|U:路由是動態的，H:目標是一個主機，G:路由指向閘道器，R:恢復動態路由產生的表項，D路由由後台程式動態安裝，M路由被後台程式修改，!拒絕路由|
|Metric|路由距離|
|Ref|路由引用次數|
|Use|路由被查詢次數|
|iface|從哪個介面輸出|

## iproute2

iproute2是linux下管理控制TCP/IP網路的工具包，旨在代替過時的nettools，即ifconfig,arp,route等命令
大部分的linux發行版都包含iproute2

iproute2工具包含的命令如下:

+ `ip` : 主要的配置命令，可以配置IPv4和IPv6
+ `tc` : 主要用來進行流量配置
+ `rtmon` : 用來產生link  ,address , routing訊息紀錄文件的命令
+ `rtacct`: 用來查看數據包流量
+ `ifcfg`:類似ifconfig，實際上是用腳本去呼叫ip指令

-------

<h1 id="ipcommand">IP指令</h1>

<h2 id="ipgrammer">文法</h2>

最常見的ip指令為:
```shell 
ip [ OPTIONS ] OBJECT [ COMMAND [ ARGUMENTS ]]
```
> Options是一組多值修飾詞，這些修飾詞會影響ip utility的一般行為和輸出。

|Options|Description|
|---------|-----------|
|-V,-Version|印出ip utility版本|
|-s,-stats,-statistics|輸出更多訊息|
|-f,-family {inet,inet6,link}|如果不存在此選項，則會從其他參數猜測要使用的協議系列|
|-o,-oneline|單行輸出結果(可以用來餵給grep)|

> OBJECT為將要對其進行操作或獲取訊息的對象

|Object|Description|
|---|---|
|link|實體或邏輯的網路設備|
|address|IPv4 or IPv6 address|
|neighbour|ARP or NDISC cache entry|
|route|routing table entry|
|rule|routing policy database|
|maddress|多播address|
|mroute|多播routing cache|
|tunnel|tunnel over IP|

command進行在指定對象上的操作，取決於對象類型，通常可進行`add`、`delete`、`show`、`object`，如果沒有輸入則預設show。

> ARGUMENTS是取決於對象的特殊命令參數，只有兩種型態
>

|ARGUMENT|Description|
|---|---|
|---flags|Keyword的縮寫|
|---parameter|依照value的keyword組合|


------------

<h2 id="iplink">IP link</h2>

>ip link為網路設備配置命令，可以啟用/禁用某個網路設備，改變mtu及mac地址等

共有[set](#iplinkset)、[show](#iplinkshow)

<h3 id="iplinkset">IP link set</h3>
同義:set、s
可以在一個指令中請求多個參數修改，但如果其中一個fail，則會在fail處終止，這可能會造成系統難以預測，盡量避免。

|Argument|Description|
|---|---|
|up / down|改變裝置狀態|
|arp on / arp off| 改變NOARP flag，當設備已經UP的狀態無法設定|
|multicast on / off|更改MULTICAST flag|
|dynamic  on / off|更改DYNAMIC flag|
|name Name|改變裝置名稱|
|txqueuelen NUMBER / txqlen NUMBER|更改對列長度|
|mtu NUMBER|更改mtu|
|address LLADDRESS|更改interface的address|
|boardcast LLADDRESS|更改廣播位址|

example:
```shell
ip link set dummy address 000000000001 #change station address of the interface dummy.

ip link set dummy up #start the interface dummy.
```

<h3 id="iplinkshow">IP link show</h3>
同義: show、list、lst、sh、ls、l

|Argument|Description|
|---|---|
|up|只顯示目前UP的interface|

```shell
kuznet@alisa~:$ ip link ls dummy
2: dummy: <BROADCAST,NOARP> mtu 1500 qdisc noop
link/ether 000000000000 brd ffffffffffff
```
冒號前的數字是interface獨一無二的識別碼，可以從`/proc/net/dev`中查詢
qdisc(queuing discipline)，在括號內的訊息可以有以下標示:

+ UP:此裝置已經被打開，準備好接收data
+ LOOPBACK:此interface不會跟其他主機通訊，而是回送給自己
+ BROADCAST:該設備可以發送packages給同物理網路上的對象
+ POINTPOINT:只有兩兩連接網路，發送和接收都只有同一個對象
+ MULTICAST:多播
+ PROMISC:此設備將所有traffic反饋到kernel，通常由bridge監視設備使用
+ ALLMULTI:接收所有多播的封包
+ NOARP:通常，他表示設備不需要任何地址解析，並且軟體硬體知道該如何處理
+ DYNAMIC:用來標記interface為動態創建和銷毀
+ SLAVE:這個interface綁訂到其他interface下

-----

<h2 id="ipaddress">IP address</h2>

>用於管理某個網路設備與協議有關的地址，與ip link類似，不過增加了協議有關的管理

command共有[add](#ipaddradd)、[delete](#ipaddrdel)、[flush](#ipaddrflush)

<h3 id="ipaddradd">IP address add</h3>
同義:add、a

| Argument | Description |
|---|---|
|local ADDRESS|inter的address(ADDRESS後面可以跟一個/來表示mask，default:/32)|
|peer ADDRESS|點對點interface的遠程端點地址|
|broadcast ADDRESS|廣播地址|
|label NAME|標籤名，string必須在名稱前加上冒號(eth0:duh)|
|scope SCOPE_VALUE|一個有效地址的範圍|
有效的scopes範圍被列在檔案`/etc/iproute2/rt_scopes`

example:
```shell
ip addr add 127.0.0.1/8 dev lo brd + scope host

# adds the usual loopback address to loopback device. The device must be enabled before this address will show up.

ip addr add 10.0.0.1/24 brd + dev eth0

#adds address 10.0.0.1 with prefix length 24 (netmask 255.255.255.0) and standard broadcast to interface eth0
```

<h3 id="ipaddrdel">ip address delete</h3>
同義:delete、del、d
參數名稱與 add一致，設備名稱為必填參數
```shell
ip addr del 127.0.0.1/8 dev lo
# deletes the loopback address from loopback device.
```
<h3 id="ipaddrshow">ip address show</h3>
同義:show、list、lst、sh、ls、l

|Argument|Description|
|---|---|
|dev NAME|裝置名稱|
|scope SCOPE_VAL|只列出這個範圍的address|
|to PREFIX|只列出符合此前綴的address|
|label PATTERN|只列出label符合的正規表示法|
|dynamic / permanent|僅列出動態或者靜態的address(IPv6 only)|
|tentative|列出未通過duplicate detection的address(IPv6)|
|deprecated|列出不推薦使用的address(IPv6)|
|primary / secondary| |

<h3 id="ipaddrflush">IP address flush</h3>
同義:flush、f
這個指令與[show](#iplinkshow)的參數相同
此指令刪除依照條件選擇的協議address，此指令會完全消除，是不可逆的操作

example:
```shell
#Delete all the addresses from private network 10.0.0.0/8:
netadm@amber~ # ip -stat -stat addr flush to 10/8
2 dummy inet 10.7.7.7/16 brd 10.7.255.255 scope global dummy
3 eth0 inet 10.10.7.7/16 brd 10.10.255.255 scope global eth0
4 eth1 inet 10.8.7.7/16 brd 10.8.255.255 scope global eth1
```

-----

<h2 id="ipneigh">ip neighbour </h2>

> 用於neighbor/arp表的管理，如顯示、插入、刪除等
> 

同義:neighbour、neighbor、neigh、n
共有[add](#ipneighadd)、[delete](#ipneighdel)、[flush]

<h3 id="ipneighadd">add、change、replace</h3>
這三個commands意義相似，`add`:增加新的neighbour entry，`change`:從現有的表中做修改，`replace`:結合add與change功能

|Argument|Description|
|---|---|
|to ADDRESS|protocol address|
|dev NAME|該鄰居所連接的interface|
|lladdr LLADDRESS|鄰居的link layer address|
|nud NUD_STATE|表示鄰居表條目的狀態|

NUD_STATE可以為

+ `permant`:此表除管理員外不可修改
+ `noarp`:條目目前是有效的，且不會被驗證，但到期後可被刪除
+ `reachable`:條目目前有效，到期後刪除
+ `stale`:條目有效，但可疑

Example:
```shell
ip neigh add 10.0.0.3 lladdr 000001 dev eth0 nud perm
#add permanent ARP entry for neighbour 10.0.0.3 on the device eth0.
ip neigh chg 10.0.0.3 dev eth0 nud reachable
#change its state to reachable.
```

<h3 id="ipneighdel">ip neighbour delete</h3>
用法同add只是缺少lladdr和nud。

<h3 id="ipneighshow">ip neighbour show</h3>
同義:show、list、sh、ls

|Argument|Description|
|---|---|
|to ADDRESS|利用prefix來縮限|
|dev NAME|只列出附加在device|
|unused|列出沒在使用的|
|nud NUD_STATE|列出特定狀態的|

-----

<h2 id="iproute">ip route</h2>
同義:route、ro、r
> 以指令用來管理kernel routing table中的entry，kernel會保留到其他網路節點路徑的資料
> 

如果有好幾筆route皆符合packet，則依照以下規則決定
1. match最長的prefix，其餘較短的放棄
2. 選擇TOS相同的，其餘放棄
3. 如果TOS沒有符合的，把該欄位不為0的放棄
4. 選擇perference value最好的
5. 如果還有重複選第一個

`route attributes`:每個route key皆引用自route information，route information包含了IP packet所需的data，eg.output device、next hop....
`route types`:一些必要屬性和可選屬性組合而成，用來描述主機到主機的真實路徑，如:unicast routes，example:unicast、unreachable、blackhole、prohibit、local、broadcast、throw、nat、anycast、multicast
`routing table`:Linux將多個route放到routing table中，routing的ID從0-255，預設routes都會新增進主要的main table(ID 254)，實際上kernel維護另一個不可見的table(ID 255)。
> 多個routing table並不會造成辨識困難，透過{tableid,prefix,tos,preference}來獲得route，而下一節的rules決定了要引用哪張routing table
> 

<h3 id="iprouteadd">ip route add</h3>



