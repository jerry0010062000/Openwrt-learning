# USB(Universal Serial Bus)

+ [Overview](#overview)
+ [前置知識](#background)
	- [驅動基礎知識](#basicdriver)
+ [USB子系統架構](#arch)
	- [USB基礎知識](#usbbasic)
	- [USB的組成](#consist)
	- [USB設備偵測](#detect)

---

<h1 id="background">0、前置知識</h1>

<h2 id="basicdriver">0.1、Driver</h2>

設備驅動程式(Device Driver)，是一個允許應用軟體與硬體互動的程式，這個程式建立了一個硬體與硬體，或硬體與軟體溝通的介面(interface)。CPU經由主機板上的總線(BUS)或其他溝通子系統(subsystem)與硬體形成連接，這樣的連接使得硬體之間交換數據成為可能。

> 驅動程式是提供硬體到操作的一個接口，並且協調兩者之間的關係
> 

電腦系統只要硬體由CPU、儲存器(內部和外部)、外部設備組成。驅動的對象一般是儲存器和外部設備，Linux將這些設備分成3大類
1. 字符設備(char):指那些能一個byte一個byte讀取的設備，如LED、鍵盤、滑鼠..等，這種設備通常要在驅動層實現open()、close()、write()、read()、ioctl()等函數，這些函數最終會被filesystem中的相關函數調用，kernel會為這種設備建立一個對應文件。
2. 塊設備(block):指那些能夠被一個固定大小寫入並且能在隨機位置讀取固定大小資料的設備，如硬碟
3. 網路設備(Network):負責主機之間的數據交換


------

<h1 id="arch">1、USB子系統</h1>

<h2 id="usbbasic">1.1、USB基礎知識</h2>

USB支援設備即插即用的hotplug功能，在1994年由intel、IBM、Microsoft等公司提出，在此之前PC接口雜亂，擴展能力差，USB正是為了解決此問題出現。

<h2 id="consist">1.2、USB設備組成</h2>
<div align=center><img src="../image/USB_device.png" width="" height="" alt="USB_device.png"/></div>

+ Configurations: 一次只能使用一種模式，不可能同時有兩種模式並行的運作狀態，常見的應用是將 USB 裝置切換至 Firmware 的更新模式，做韌體升級，此時該硬體的一般模式將沒有作用。
+ Interface:一個 USB 裝置可能有很多個輸入或輸出介面，舉例來說，若是有一個帶麥克風的耳機，就會有兩個介面，分別為『聲音 Input』和『聲音 Output』所使用。
+ Endpoint:USB Device 真正和 Host 間傳輸資料是透過『Endpoint』，等到 USB Device 初始化完，剩下的就是使用 Endpoint 與 USB Device 溝通

<h2 id="detect">1.3、USB設備的偵測</h2>

<h3 id="enum">1.3.1、枚舉</h3>

`enumerate`:在這邊是指從設備讀取一些訊息，包括知道是怎麼樣的設備，以及如何通訊，這樣主機就可以根據這些訊息來載入合適的驅動。

USB架構中，hub負責檢測設備的連接和斷開。在系統啟動時，主機以polling的方式詢問root hub是否有新的設備連接，一旦有新設備連接上，host就會發送一系列的requests給設備所掛載到的hub上，再由hub建立起一條host與device之間通訊的通道，然後主機以控制傳輸的方式對endpoint0發送各種請求進行枚舉操作。

<h3 id="state">1.3.2、設備狀態</h3>
<div align=center><img src="../image/USB_state.png" width="" height="" alt="USB_state.png"/></div>
1. `Attached`設備連接到USB插孔上
2. `Powered`:設備的電源可以來自外部，也可以由hub提供
3. `Default`:設備通電後，還不會回應任何bus處理，直到bus接收復位訊號為止。收到訊號後，可以用default的address找設備
4. `Address`:在復位後主機會分配一個唯一的address給設備，當設備處於suspended狀態時這個address都不會改變
5. `Configured`:在USB設備正常工作之前，必須被正確的設定
6. `Suspended`:設備在沒有與bus傳輸時，為了省電，會將設備暫時掛起。

<h2 id="enumproc">1.2、USB枚舉過程</h2>

1. 用戶把USB設備插入端口 : 這時設備處於加電狀態，端口是無效的
2. Hub監測端口上的電壓 : 設備接口上會有一個上拉電組，當插入時，hub會偵測到電壓上升，便認為有設備插入，並且透過D+、D-來判斷是全速還低速設備
3. Host了解連接的設備 : hub向主機報告端口的狀態，此時設備還不可見，如果連接/斷開事件被告知，host會再發送request詢問此次事件詳情
4. hub接到request後回復host這個設備是高速還是低速的
5. host等待上電完成後，會發出復位請求，使該設備的D+、D-回到低電平
6. host檢測全速設備是否能支持高速模式
7. Hub建立與主機之間的訊息通道 : host會不斷詢問復位是否成功，當成功時轉回default state，address為0，host可以透過這個address和endpoint0通訊
8. host請求取得Descriptor : 在這次請求中host只關心descriptor的訊息長度，其他並不在乎。
9. host分配新的address給device
10. host再度請求Descriptor : 這次請求中，host會先請求新地址的Device descriptor了解支援的配置個數，設備類型，VID，PID。然後依序讀取Configuration Descriptor、字符串等，逐一了解設備更詳細的訊息
11. 主機通過對descriptor的了解，選擇最合適的driver給設備，然後宣告設備已找到，後調用設備模型提供的interface，將設備加到usb bus的設備列表中，然後bus會找驅動列表中的所有驅動哪個跟裝置最合適，之後交給驅動處理。



<h2 id="framework">2、USB總線驅動框架</h2>

USB總線驅動框架總結如下圖:
<div align=center><img src="../image/USB_arch.png" width="" height="" alt="USB_arch.png"/></div>




是由`USB Core`和`USB HCD`組合而成

<h3 id="usbcore">2.1、USB Core</h3>

USB core這個模組為純軟體，他並不代表一個設備，是獨立於硬體的protocol stack，是所有USB設備賴以生存的模組，及USB子系統核心。code在kernel/drivers/usb/core底下

USB core為設備驅動提供服務，一個用於訪問和控制硬體的interface，而不用考慮底層是哪種controller。Core將request mapping到相關的HCD，用戶不能直接訪問HCD，Core就是HCD與USB設備的橋樑。

<h3 id="hcd">2.2、USB HCD</h3>

全名`Host Controller Driver`，在硬體host控制器之上運行，是對主機硬體的一個抽象，實現kernel與controller之間的對話接口，會包含多種USB interface標準:

1. `OHCI`（Open Host Controller Interface）是支援USB1.1的標準，但它不僅僅是針對USB，還支援其他的一些介面，比如它還支援Apple的火線（Firewire，IEEE 1394）介面。與UHCI相比，OHCI的硬體複雜，硬體做的事情更多，所以實現對應的軟體驅動的任務，就相對較簡單。主要用於非x86的USB，如擴展卡、嵌入式開發板的USB主控。

2. `UHCI`（Universal Host Controller Interface），是Intel主導的對USB1.0、1.1的介面標準，與OHCI不相容。UHCI的軟體驅動的任務重，需要做得比較複雜，但可以使用較便宜、較簡單的硬體的USB控制器。Intel和VIA使用UHCI，而其餘的硬體提供商使用OHCI。
   
3. `EHCI`（Enhanced Host Controller Interface），是Intel主導的USB2.0的介面標準。EHCI僅提供USB2.0的高速功能，而依靠UHCI或OHCI來提供對全速（full-speed）或低速（low-speed）設備的支援。
   
4. `xHCI`（eXtensible Host Controller Interface），是最新最火的USB3.0的介面標準，它在速度、節能、虛擬化等方面都比前面3中有了較大的提高。xHCI 支援所有種類速度的USB設備（USB 3.0 SuperSpeed, USB 2.0 High-speed, USB 1.1 Low-speed and Full-speed）。

<h3 id="dd">USB Device Driver</h3>

USB Device是由一些`Configuration`、`Interface`、`Endpoint`組成，亦即一個USB device可以含有一個或多個interface，每個interface含有多個endpoint。一個子驅動對應一個interface而非整個device

## temp

USB1.1規格支援兩種速率：低速(low speed)1.5Mbps和全速(full speed)12Mbps.
USB2.0規格除了支援原有的兩種速度外，還額外支援高速(high speed)480Mbps。
USB3.0支援全雙工，比USB 2.0多了數個觸點，並採用發送列表區段來進行資料發包。USB 3.0暫定的供電標準為900mA，且理論上有支援光纖傳輸的潛力。USB 3.0設計的「Super Speed」傳輸速度為5Gbit/s，理論資料傳輸速度為625MByte/s。


----------

<h1 id="kmod">kernel module</h1>

`kmod-usb-core`
`kmod-usb-storage`:必須的，支援USB儲存
`kmod-usb-storage-extras`(option) kernel支持更多驅動程式，eg.SmartMedia讀卡

`kmod-usb-ohci`支援USB1.1
`kmod-usb2`or `kmod-usb-ehci`支援USB2.0
`kmod-usb3`:支援USB3.0

`kmod-scsi-core`任何大容量儲存都是通用SCSI設備
`block-mount`如果使用fstab UCI配置或LUCI安裝，用於安裝和檢查block(file system的交換)和hot-plug(插入設備時偵測)腳本

### 文件系統
`kmod-fs-ext4`、`kmod-fs-ext3`Linux系統支持，但windows不支持
`kmod-fs-vfat`FAT32格式，Linux和window支持但格式僅支持32GB的最大分區和4GB的單檔寫入
`kmod-fs-ntfs`NTFS格式的USB是最廣泛的一種，支持文件最大可擴展為256TB
`ntfs-3g`如果選擇kmod-fs-ntfs掛載會變成read only，兩個都選mount時需要-t ntfs-3g指定，否則會預設ntfs read only。

-------------


