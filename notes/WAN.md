# WAN

### bridge mode

>https://openwrt.org/docs/guide-user/network/wan/bridge-mode

橋接模式是一種特殊的操作模式，將當前的gateway/modem當成bridge，將所有traffic轉發到下游設備，防火牆的橋接模式是由kmod-br-netfilter模組提供

## proto setting
### pppoe
透過乙太網路(ethernet)建立ppp會話，使主機能連接到遠端的寬頻進入伺服器上。

|Name|Type|Description|
|---|---|----------|
|username|string|用於PAP/CHAP認證的使用者名稱|
|password|string|用於PAP/CHAP認證的密碼|
|ac|string|指定一個Access Concentrator連線，如果沒有pppd會用第一個發現的|
|service|string|類似於ac|
|connect|file path|指定自製的ppp連線腳本|
|disconnect|file path|指定ppp中斷連線腳本|
|keepalive|2numbers|兩個數字必須用空格分開，第一個表示ping多少次失敗後中斷連線，第二個表示ping之間相隔秒數|
|demand|number|不活動關閉連線前等待秒數|
|defaultroute|bool|用ppp連線取代現有的default route|
|peerdns|bool||


PAP用於驗證遠端帳密是否正確 ，CHAP則是確定可連線後，認證方發出一個challenge給用戶端，用戶端會用one-way-hash去計算值寄回認證端，符合則允許。

