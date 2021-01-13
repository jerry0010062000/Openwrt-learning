# 無法分類筆記

+ Default gateway與Default route的差異

  Default Route設定於Layer3環境，而Default Gateway設定於Layer2環境，都是以Layer3的IP進行定址。

+ 私有IP位址
  10.0.0.0 - 10.255.255.255 (10/8 prefix)
  172.16.0.0 - 172.31.255.255 (172.16/12 prefix)
  192.168.0.0 - 192.168.255.255 (192.168/16 prefix)

+ Router vs Modem
  Modem 是由網絡供應商提供的調解器，其名字是由英文全名 modulator-demodulator 縮寫而成，它的作用就是把互聯網的類比訊號解為電腦讀取的數碼訊號，同樣地 Modem 也可把電腦的數碼訊號轉化為互聯網的類比訊號。簡單一點，Modem 就是把訊號不斷轉化及解碼，以乎合電腦及遠端的數據機傳輸訊息。
  Router 名為路由器，它就如一個部門主管，其主要工作就是以有線的方式，或以無線訊號（Wi-Fi）連接家中各部裝置，如電腦、電視、手機、平板電腦等，從而建立一個內聯網。

  如果以公路作為比喻，互聯網就是高速公路，Modem 就是高速公路的一個出入口，而 Router 就是通過出入口之後的網路交匯處，互聯網的訊號就是通過以上的路徑，最終去到目的地即是我們的電腦及手機等裝置。

-------------------

## MAC address的唯一性
MAC address一共占用6個bytes(48bit)，以16進制顯示，ex:
```
6e:77:0f:b8:8b:6b
```
IEEE為了保證其唯一性將前24位做為廠商標示符（Organizationally Unique Identifier，OUI），後面的24位才是序列號
<div align=center><img src="image/mac-address.png" width="" height="" alt="mac-address.png"/></div>

--------------------

### kernel space & user space

process指令從user space切換到kernel space時會經過context switch
<div align=center><img src="image/context-exchange.png" width="" height="" alt="context-exchange.png"/></div>

現代CPU都具有不同的操作模式，不同的級別有不同的功能，其擁有資源也不盡相同，Linux在系統設計時也利用這個特性，分出了最高級(kernel)和最低級別(user)，kernel和user擁有各自的memory space，所以必須經過context switch

---------------------------
### Linux Bridge

>[原文](https://opengers.github.io/openstack/openstack-base-virtual-network-devices-bridge-and-vlan/#linux-bridge)

Bridge是Linux上工作在第二層的虛擬Switch，依靠軟體實現，與普通第二層功能相同。可以添加網路設備(em0,eth0,tap...)到Bridge上。在Bridge中會維護一個類似物理交換機的MAC address表，來達成數據轉發的功能。  與物理Switch不同的是，運行Bridge的Linux本身需要IP位址和其他通訊設備，但分配到bridge上的網卡是不能配置IP的(因為工作在第二層)，而 Bridge本身是可以被分配IP的，一旦被分配了IP，就意味著Bridge可以作為Router interface，如下圖。

<div align=center><img src="image/bridge-path.png" width="" height="" alt="br-path.png"/></div>

上圖主機中有em1、em2兩張實體網卡，bridge `br0`，VPN、虛擬機`qemu-kvm`。

1. Bridge處理封包流程
   
- 封包從外部網路(A)發往虛擬機(P2)過程，首先封包從em2(B)物理網卡進入，之後em2將封包轉發給vlan子設備em2.100，經過bridge check(L)發現子設備em2.100屬於Bridge設備，因此封包不會往上層協議發送(T)，而是進入Bridge內處理，封包從em2.100進入br0，經過`bridging decision`發現封包應該從`tap0(E)`發出，此時離開主機網路協議，發往被user process打開的設備(N)，最終qemu-kvm讀取數據。在此過程中，A發出的封包不會經過上層協議。
  
2. Bridging decision
上圖`br0`收到封包後，根據目的地MAC的不同，Bridging decision有不同的決定:
	- 目的MAC為br0本身，收到發往主機的封包，往上層送(D->J)
	- 廣播，轉送到br0上所有interface(br0,tap0,tap1,...)
	- 目的存在MAC表中，查表到對應接口(D->E)
	- 目的不存在MAC表中，flooding到所有接口(br0,tap0,tap1,...)
	- IP address不是br0,橋不處理，直接轉送給上層。

3. VLAN

   VLAN又稱虛擬網路，其原理是在第二層協議中插入額外的VLAN數據，同時保持和傳統二層設備的兼容性。VLAN設備的作用是建立一個個帶不同vlan tag的子設備，他並不能建立多個可以交換轉發數據的接口，因此需要借助bridge，把VLAN建立的子設備(eg.em2.100)橋接到br0上

----------------

### VLAN

> [參考這篇](https://blog.csdn.net/zqixiao_09/article/details/79180214)
> 

根據IEEE802.1Q標準定義了VLAN HEADER格式，其在Frame header SRC Addr後，帶vlan的switch port分為兩種
1. Access port
	這些port被打上了vlan tag，離開switch的Access port的header不會有vlan tag的存在。
2. Trunk port
	在多個switch的狀況下，需要有一條線連接兩個switch，連接的port稱為trunk port，離開和進入都會保留vlan tag。
	

配置了vlan的switch比傳統switch多了以下機制:
1. MAC表格中每行有不同的VLAN ID，做比較時會拿Frame的MAC addr和VLAN ID進行比對
2. 如果沒有吻合的選項，則從所有相同VLAN ID的port和 Trunk轉發出去。