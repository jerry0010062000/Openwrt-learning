# Overview

`Package`指的是壓縮過的program和一些腳本，其隨附的配置檔用於將其整合到作業系統中，Package由Package Manger管理(OPKG)，包括下載、開啟、安裝、卸載，基本上，openwrt firmware是由packages圍繞著Linux kernel組成。
每個package都單獨編譯並將其所需文件安裝到臨時資料夾中，然後將資料夾壓縮到firmware的唯讀分區中。
Kernel的處理也像Package一樣，但是是經由bootloader期望的特殊方式。
最後就是創建firmware文件，該文件通常是一個image檔，準備將其寫入flash中。

----

## Package compile方式

如果曾經閱讀Makefile，可能會注意到他列出了將要被編譯的官方source code下載連結。
在`/patch`資料夾底下找到的patch將會被應用在下載後，編譯前。或許會有uci的配置文件。
所有的package都會經由toolchain來編譯，

## package feeds

並非所有的package都包含在openwrt項目中，Packages來自openwrt主要的repository是由核心開發人員維護，package feeds則是額外添加的package，由社群提供。官方feeds是由官方server提供且編譯，但依然是由社群維護的，packages依照feeds名稱劃分，例如

-----

## Building package

openwrt 是以packages的集合來做開發和維護的，典型的firmware是由package和kernel構成

Oopenwrt package 指的是以下兩種事物其中之一
+ 一個source package
	-  Makefile描述software如何取得，建構和打包過程(必須)
	-  patch (option)
	-  其他靜態文件，eg. init script, config file...(option)
+ 一個binary package，GNU tar 兼容檔案，包含隨附的package控制文件，類似於.deb或.rpm

-----

## Download
在makefile聲明下載方法時，透過tarball Http或Https直接取得source code為佳，應避免是用git或SCM clone。

-----

## Building
Default下，系統使用標準./configure和 make來build packages，參考`package-default.mk`如何實現。

錯誤的方式:
```shell
define Build/Compile 
        (cd $(PKG_BUILD_DIR)/nonstandard/dir/; make)
endef
```
正確的方式:
```shell
MAKE_PATH := nonstandard/dir/
```

----

## makefile製作

以下是一個example
```shell
include $(TOPDIR)/rules.mk
 
PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1
 
PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)
PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bridge
PKG_HASH:=9b7dc52656f5cbec846a7ba3299f73bd
 
include $(INCLUDE_DIR)/package.mk
 
define Package/bridge
  SECTION:=base
  CATEGORY:=Network
  TITLE:=Ethernet bridging configuration utility
  #DESCRIPTION:=This variable is obsolete. use the Package/name/description define instead!
  URL:=http://bridge.sourceforge.net/
endef
 
define Package/bridge/description
 Ethernet bridging configuration utility
 Manage ethernet bridging; a way to connect networks together to
 form a larger network.
endef
 
define Build/Configure
  $(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR))
endef
 
define Package/bridge/install
        $(INSTALL_DIR) $(1)/usr/sbin
        $(INSTALL_BIN) $(PKG_BUILD_DIR)/brctl/brctl $(1)/usr/sbin/
endef
 
$(eval $(call BuildPackage,bridge))
```

package變數:

| var | desc |
|---|-----------------------|
|PKG_NAME|名稱|
|PKG_VERSION|版本|
|PKG_RELEASE|Makefile版本|
|PKG_LICENSE| |
|PKG_LICENSE_FILE| |
|PKG_BUILD_DIR|編譯位置|
|PKG_SOURCE|原始來源的名稱|
|PKG_SOURCE_URL|原始來源的鏈結|
|PKG_HASH|用來驗證下載的checksum|
|PKG_CAT|如何解壓縮|
|PKG_BUILD_DEPENDS|這個package依賴的package|
|PKG_CONFIG_DEPENDS|指定那些config會影響build，當改變時重新運行 Build/Configure|


------

## feeds 

在openwrt中，feeds是公用一個儲存空間的package集合，如遠端伺服器上，版本控制系統中，本地file system中

### feeds configurtion

feeds 設定檔由三個空格分隔的部分組成:`method`，`name`，`source`。
feeds.conf.deafult :
```shell
src-git packages https://git.openwrt.org/feed/packages.git
src-git luci https://git.openwrt.org/project/luci.git
src-git routing https://git.openwrt.org/feed/routing.git
src-git telephony https://git.openwrt.org/feed/telephony.git
#src-git video https://github.com/openwrt/video.git
#src-git targets https://github.com/openwrt/targets.git
#src-git management https://github.com/openwrt-management/packages.git
#src-git oldpackages http://git.openwrt.org/packages.git
#src-link custom /usr/src/openwrt/custom-feed
```

|method| funtion|
|---|------|
|src-bzr|從來源URL下載|
|src-cpy|從路徑複製|
|src-darcs |使用darcs從路徑或URL下載|
|src-git|使用git clone|

