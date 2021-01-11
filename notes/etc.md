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

---------------------------
### Linux Bridge
Bridge是Linux上工作在第二層的虛擬Switch，依靠軟體實現，與普通第二層功能相同。可以添加網路設備(em0,eth0,tap...)到Bridge上。在Bridge中會維護一個類似物理交換機的MAC address表，來達成數據轉發的功能。  與物理Switch不同的是，運行Bridge的Linux本身需要IP位址和其他通訊設備，但分配到bridge上的網卡是不能配置IP的(因為工作在第二層)，

 Bridge本身是可以被分配IP的，一旦被分配了IP，就意味著Bridge可以作為Router interface，如下圖。

<div align=center><img src="image/bridge-path.png" width="" height="" alt="br-path.png"/></div>

