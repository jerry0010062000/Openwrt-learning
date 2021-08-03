# Quagga rip note

> [Quagga技術文檔](https://www.nongnu.org/quagga/docs/quagga.html)

---------------------

+ [Overview](#overview)
+ [View Mode](#VMode)
+ [Example](#example)


--------------------
<h2 id = overview>Overview</h2>

Quagga 為一路由軟體套件，此套件為一共享軟體，可以在網路上自由 下載使用。Quagga由著名的路由軟體Zebra改版而來，支援OSPFv2, OSPFv3, RIP v1 and v2, RIPng 以及 BGP-4 協定。

- 使用UDP port `520` 來發送RIP packets
- 運行RIP之前必須先啟動zebra daemon
- zebra 這個 daemon 的功能在更新核心的路由規則；
- RIP 這個 daemon 則是在向附近的其他 Router 溝通協調路由規則的傳送與否。

-------------

<h2 id = VMode>View Mode</h2>

啟動zebra後可藉由`telnet`連線`port 2601`
輸入『 ? 』就能夠知道有多少指令可使用

```C
  echo      Echo a message back to the vty
  enable    Turn on privileged mode command
  exit      Exit current mode and down to previous mode
  help      Description of the interactive help system
  list      Print command list
  quit      Exit current mode and down to previous mode
  show      Show running system information
  terminal  Set terminal line parameters
  who       Display who is on vty
```

show ip route
K ：代表以類似 route 指令加入核心的路由規則，包括 route-ethN 所產生的規則；
C ：代表由你的網路介面所設定的 IP 而產生的相關的路由規則
S ：以 zebra 功能所設定的靜態路由資訊；
R ：就是透過 RIP 協定所增加的路由規則囉！

----------------------

<h2 id = example>Example</h2>

> [範例參考自鳥哥blog](http://linux.vbird.org/linux_server/0230router.php#dynamicroute)

配置檔在`/etc/quagga/`目錄底下

###　zebra.conf example
```bash
hostname www.centos.vbird           <==給予這個路由器一個主機名稱，隨便取！
password linuxz1                    <==給予一個密碼！
enable password linuxz1             <==將這個密碼生效！
log file /var/log/quagga/zebra.log  <==將所有 zebra 產生的資訊存到登錄檔中
```
###  ripd.conf example
```bash
hostname www.centos.vbird           <==這裡是設定 Router 的主機名稱而已
password linuxz1                    <==設定好你自己的密碼喔！
debug rip events                    <==可以記錄較多的錯誤訊息！
debug rip packet                    <==鳥哥透過這個訊息解決很多問題
router rip                          <==啟動 Router 的 rip 功能
 version 2                          <==啟動的是 RIPv2 的服務 (預設值)
 network 192.168.1.0/24             <==這兩個就是我們管理的介面囉！
 network 192.168.100.0/24
interface eth0                      <==針對外部的那個介面，要略過身份驗證的方式
 no ip rip authentication mode      <==就是這個項目！不要驗證身份！
log file /var/log/quagga/zebra.log  <==登錄檔設定與 zebra 相同即可
```



---------------

<h2 id = ref>參考資料</h2>

https://blog.laoliu.eu.org/archives/e3c95af8.html
https://scottj.idv.tw/blog/2014/03/10/%E5%AF%A6%E5%81%9A-ripv2-with-ubuntu12-04/
http://speed.cis.nctu.edu.tw/~ydlin/course/cn/exp/Exp_Linux-Router/Linux-router.pdf
https://www.nongnu.org/quagga/docs/quagga.html#RIP
http://linux.vbird.org/linux_server/0230router.php#dynamicroute
