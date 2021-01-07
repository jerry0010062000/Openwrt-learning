# Base system

Kernel啟動過程結束時，將執行init daemon。Openwrt使用procd，接著遵循`/etc/rc.d`逐一啟動腳本。大多數運行openwrt的平台可能會沒有鍵盤或顯示器，因此我們需要透過serial port或ethernet port訪問。