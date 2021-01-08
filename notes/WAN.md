

# WAN

+ [PPP](#ppp)
	- [PPPoE](#pppoe)
	- [PPPoA](#pppoa)
	- [PPP on 3g](#3g)
+ USB modem
	- [QMI](#qmi)
	- [NCM](#ncm)
	

### bridge mode

>[Openwrt手冊-bridge mode](https://openwrt.org/docs/guide-user/network/wan/bridge-mode)

橋接模式是一種特殊的操作模式，將當前的gateway/modem當成bridge，將所有traffic轉發到下游設備，防火牆的橋接模式是由kmod-br-netfilter模組提供

-----------------------------

### PPPoE vs PPPoA
兩者主要差別在於發起PPP的設備不同，PPPoE是由已太網卡發起，PPPOA則是由ATM專用設備或ADSL MODEM發起，相對於PPPoE可以說沒有優勢，ADSL很少採用這種方式。

## Netifd - proto setting
<h2 id="ppp">PPP</h2>

> The package `ppp` must be installed to use PPP.

|Name|Type|Description|
|---|---|----------|
|username|string|用於PAP/CHAP認證的使用者名稱|
|password|string|用於PAP/CHAP認證的密碼|
|connect|file path|指定自製的ppp連線腳本|
|disconnect|file path|指定ppp中斷連線腳本|
|keepalive|2numbers|兩個數字必須用空格分開，數字一表示ping多少次失敗後中斷連線，數字二表示ping之間相隔秒數|
|demand|number|不活動關閉連線前等待秒數|
|defaultroute|bool|用ppp連線取代現有的default route|
|peerdns|bool|是否修改/etc/resolv.conf|
|dns|list of ip address|override peer assigned DNS|
|ipv6|0,1,auto|是否使用IPv6|

PAP用於驗證遠端帳密是否正確 ，CHAP則是確定可連線後，認證方發出一個challenge給用戶端，用戶端會用one-way-hash去計算值寄回認證端，符合則允許。

--------------------------

<h2 id="pppoe">PPPoE</h2>

> The packages `ppp`, `kmod-pppoe` and `ppp-mod-pppoe` must be installed to use PPPoE.

透過乙太網路(ethernet)建立ppp會話，使主機能連接到遠端的寬頻進入伺服器上。
|Name|Type|Description|
|---|---|----------|
|ac|string|指定一個Access Concentrator連線，如果沒有pppd會用第一個發現的|
|service|string|類似於ac|

其餘與PPP相同。

------------
<h2 id="pppoa">PPPoA</h2>
> The package ppp-mod-pppoa must be installed to use PPPoA.
> 

|Name|Type|Description|
|---|---|----------|
|vci|number|PPPoA VCI|
|vpi|number|PPPoA VPI|
|atmdev|number|通常ATM adapter起始為0，不需要設置|
|encaps|string|llcv or vc|

其餘與PPP相同。

----------------------
+ 撥號連線
	或稱撥接上網、撥號上網，是指通過本地電話線經由數據機連接網際網路，於1990年代網路剛興起時比較普及，但因收費昂貴、速度慢，漸被寬頻連線取代
+ 寬頻
	- ADSL:
        橋接，直接提供靜態IP
        PPPoA，基於ATM的點對點協定
        PPPoE，基於乙太網的點對點協定
	- 光纖

--------------------

<h2 id="3g">3g</h2>
#### PPP over EV-DO,CDMA,UMTS,GPRS

> The package comgt must be installed to use 3G.
> 

|Name|Type|Description|
|---|---|----------|
|device|file path|數據機裝置節點|
|service|string|3g服務的類型|
|apn|string|access point name|
|pincode|number|SIM卡的PIN碼|
|dialnumber|string|Modem dial string|
|maxwait|number|等待modem ready的秒數|

----------------
## USB Modem

<h2 id="qmi">QMI</h2>

> The package comgt-ncm + modem specific driver must be installed to use NCM.
> 

|Name|Type|Description|
|---|---|----------|
|device|file path|QMI設備|
|apn|access point name|
|pincode|number|PIN碼|
|username|string|用於PAP/CHAP認證的使用者名稱|
|password|string|用於PAP/CHAP認證的密碼|
|auth|string|PAP/CHAP/both/none|
|modes|string|all,lte,umts, gsm, cdma, td-scdma|

-----------------------------

<h2 id="ncm">NCM</h2>

> [參考這裡](https://wowothink.com/588ebc22/)
> The package comgt-ncm + modem specific driver must be installed to use NCM

Network Control Model協議用於設備和主機之間交換ethernet frame，NCM設備一般用在支持3.5/4g無線網路卡上。NCM是建立在ECM的基礎上，進行改進以支援更高的傳輸速率。

|Name|Type|Description|
|---|---|----------|
|device|file path| typically /dev/cdc-wdm0 or /dev/ttyUSB#|
|apn|string|access point name|
|pincode|number|PIN碼|
|username|string|用於PAP/CHAP認證的使用者名稱|
|password|string|用於PAP/CHAP認證的密碼|
|auth|string|PAP/CHAP/both/none|
|mode|string|preferlte,preferumts,lte,umts,gsm,auto|
|pdptype|string|IP,IPV6,IPV4V6|
|delay|number|等待連上數據機的時間|

----------------------------------

## WWAN
>  The package wwan must be installed to use this feature. 
>  [參考](https://www.rohm.com.tw/electronics-basics/wireless/wireless_what1)
>  

-------------------------------



