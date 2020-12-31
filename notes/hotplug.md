# Hotplug

當某些event發生時(例如interface up or down、偵測到新的儲存驅動或按紐事件，procd將依照腳本開頭的數字大小依序執行 `/etc/hotplug.d`下的腳本

|資料夾|Description|
|---|------|
|block|Block device events (block device connected/disconnected)|
|button|按鍵事件，/etc/rc.button產生|
|iface|interface events(LAN、WAN連線)|
|net|網路相關事件|
|ntp|同步時間是件|
|usb|USB裝置(3g-modem 、tty)|

在執行hotplug腳本時，procd利用環境變數來提供大量訊息

### iface
```bash
ACTION				#“ifup”, “ifdown”, “ifupdate”
INTERFACE			#Name of the logical interface which went up or down (e.g. “wan” or “ppp0”)
DEVICE				#Physical device name which interface went up or down (e.g. “eth0.1” or “br-lan”)
IFUPDATE_ADDRESSES	#“1” if address changed
IFUPDATE_PREFIXES	#“1” if prefix updated FIXME what constitutes an update?
```
一個自己的腳本範例
```bash
cat << "EOF" > /etc/hotplug.d/iface/99-my-action
[ "${ACTION}" = "ifup" ] && {
    logger -t hotplug "Device: ${DEVICE} / Action: ${ACTION}"
} 
EOF
```
