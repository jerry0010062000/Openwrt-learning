+ [Overview](#overview)
	- [目錄結構](#index)
+ [Packages](#package)
	- [Makefile寫法](#makefile)
	- [Rule.mk預定義的變數](#rulemk)
	- [關鍵字的定義](#keywordef)
	- [相依](#depend)
+ [Feeds](#feeds)
	- feeds配置
	- feeds命令
	- 自定義feed
+ [Openwrt Build usage](#usage)

-------------

<h1 id="overview">Overview</h1>

`Package`指的是壓縮過的program和一些腳本，其隨附的配置檔用於將其整合到作業系統中，Package由Package Manger管理(OPKG)，包括下載、開啟、安裝、卸載，基本上，openwrt firmware是由packages圍繞著Linux kernel組成。
每個package都單獨編譯並將其所需文件安裝到臨時資料夾中，然後將資料夾壓縮到firmware的唯讀分區中。
Kernel的處理也像Package一樣，但是是經由bootloader期望的特殊方式。
最後就是創建firmware文件，該文件通常是一個image檔，準備將其寫入flash中。

<h2 id="index">目錄結構</h2>

+ 重要的原生文件及目錄

	- `Makefile`、`rules.mk`、`include/`包含了整個Openwrt的基本Makefiles，定義了許多Makefile巨集及相關變量
	- `package/`目錄包含了Openwrt提供的基本packages，這些package並不一定在openwrt中維護，但是可以透過腳本來獲取。
	- `scripts/`目錄包含了openwrt提供了一些實用功能，例如確認packages的完整性、更新到最新版本等
	- `target/`此目錄中的linux包含了針對不同平台的kernel patch以及特殊配置等
	- `toolchain/`、`tools/`包含了一些通用的命令及編譯工具練，用來生成firmware、編譯器、c libraries等
+ 編譯中生成的目錄

	- 編譯過程中下載的toolchain、target、package會存放在自動建立的目錄`dl/`下，這個目錄會在make distclean時刪除
	- `toolchain/`和`tools/`編譯生成的結果會儲存在以下三個目錄中，`build_dir/host/`是一個暫存目錄，用來儲存不依賴目標平台的工具，`build_dir/toolchain`用來儲存依賴於目標平台的工具，`staging_dir/toolchain/`為toolchain最終安裝位置
	- `bin/`儲存編譯最終生成firmware以及各package的ipk
	- `build_dir/linux-*/`儲存編譯過程中展開的kernel與驅動模組的code以及編譯生成的目標文件
	- `build_dir/target-\*/`儲存編譯過程中展開的所有packages code以及編譯生成的目標文件

> `Build_dir`與`staging_dir`的差異:
> 

+ Build_dir:用來unpack全部的來源檔案，並且compile他們
	- `build_dir/host`用來compiling所有host上所需的工具
	- `build_dir/toolchain`用來compiling cross-C compiler和C標準函式庫，這個區域用來存放跑在host上compiling programs(cross C compiler)，和跑在target上的libraries(uClibc,libm,pthreads...etc)
	- `build_dir/target`用來compiling給target system的packages和kernel
+ Staging_dir:用來"install"所有被編譯過的程式，不論是用來編譯更進一步的packages，或者是用來準備生成firmware image
	- `staging_dir/host`一個小型的Linux root，擁有/bin,/lib...etc，這是host tools安裝的地方，建構系統其餘的部分會將該區域的目錄前綴到Path中。
	- `staging_dir/toolchain`也是一個小型的linux root，包含交叉C編譯器用來建構其餘的firmware，可以用來在openwrt之外編譯可用來載入到firmware的C程式example:`staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-uclibc-gcc`。
	- `staging_dir/target.../root-...`:包含已被安裝版本的package在/bin，/lib下，這在將來會再通過一些調整將其壓縮到實際的firmware跟目錄中

----

<h1 id="package">Packages</h1>

Openwrt 是以packages的集合來做開發和維護的，典型的firmware是由package和kernel構成

Openwrt package 指的是以下兩種組成的其中之一
+ 在一個source package中擁有

	-  Makefile	-描述軟體原始碼如何取得，如何建構和打包的過程(必須)
	-  patches/	-包含一些針對官方原始碼的bug fixes、優化內容或自定義功能(option)
	-  files/	-通常在此目錄下包含一些針對該package的default config以及啟動腳本(option)
+ 一個binary package，GNU tar 兼容檔案，包含隨附的package控制文件，類似於.deb或.rpm

<h2 id="makefile">Makefile</h2>
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

### Package變量

| var | desc |
|---|-----------------------|
|PKG_NAME|此package名字，menuconfig中顯示的名稱|
|PKG_VERSION|軟體的上游版本|
|PKG_RELEASE|Makefile版本號，openwrt自己對每個package賦予的版本|
|PKG_LICENSE|這個package的授權協議，通常來自上游廠商|
|PKG_LICENSE_FILE|描述授權協議的文件|
|PKG_BUILD_DIR|在何處儲存並編譯這個package的source code|
|PKG_SOURCE|原始來源的名稱|
|PKG_SOURCE_URL|從哪裡下載source code|
|PKG_HASH|用來驗證下載的checksum|
|PKG_CAT|如何解壓縮|
|PKG_BUILD_DEPENDS|編譯這個package之前必須先編譯的package，一般而言都是提供link時的依賴library或header|
|PKG_INSTALL|如果設置為1，將調用原始碼中的原生make install並將prefix設為PKG_INSTTALL_DIR|
|PKG_INSTTALL_DIR|定義make install會將編譯好的文件複製到哪裡|
|PKG_FIXUP|將在後面描述|
|PKG_SOURCE_PROTO|獲取原始碼的協議|
|PKG_CONFIG_DEPENDS|指定那些config會影響build，當改變時重新運行 Build/Configure|

#### PKG_FIXUP
许多软件包认为autotools是一个好工具，最终需要修复来解决自动化工具“意外的”认为使用主机工具（就是你的电脑系统上安装的一些构建工具）替代构建环境的工具（指OpenWrt项目上的toolchain中提供的构建工具）更好。OpenWrt定义一些PKG_FIXUP规则帮助解决此事。

PKG_FIXUP可以是下面几个值之一：

1. autoreconf：这将会执行以下动作
    - autoreconf -f -i
    - 创建必须但是缺失的文件
    - 保证openwrt-libtool已被链接好
    - 阻止autopoint/gettext
2. patch-libtool： 如果释出的automake方法已破坏且无法修复，则寻找一个libtool实例，检测其版本并应用OpenWrt的修正补丁。
3. gettext-version： 这个fixup在automake的gettext支持中阻止了版本不匹配的错误。
	提醒：使用autotool工具的软件包应该通过简单的指定PKG_FIXUP:=autoreconf正常工作，否则就可能出现要求特定版本的问题。

在makefile文件中最底部調用的BuildPackage才是真正執行命令的地方，僅接受一個參數:package name，其他所有需要訊息從define中得到。

Openwrt建構系統支持多種不同方式下載source code ，大多數packages使用tar.gz或.tar.bz2打包source code ，透過`PKG_SOURCE_PROTO`決定如何下載
或是直接將source放在`package/<packagename>/`下，一般保存在src目錄中
在makefile聲明下載方法時，透過tarball Http或Https直接取得source code為佳，應避免是用git或SCM clone。

<h2 id="rulemk">Rule.mk預定義變數</h2>
```
$(1)表示嵌入式系統的映象目錄
INSTALL_DIR:=install -d -m0755 意思是建立所屬使用者可讀寫和執行，其他使用者可讀可執行的目錄。 
INSTALL_BIN:=install -m0755 意思是編譯好的檔案存放到映象檔案目錄。

PACKAGE_DIR:=$(BIN_DIR)/packages
PACKAGE_DIR_ALL:=$(TOPDIR)/staging_dir/packages/$(BOARD)
BUILD_DIR:=$(BUILD_DIR_BASE)/$(TARGET_DIR_NAME)
STAGING_DIR:=$(TOPDIR)/staging_dir/$(TARGET_DIR_NAME)
BUILD_DIR_TOOLCHAIN:=$(BUILD_DIR_BASE)/$(TOOLCHAIN_DIR_NAME)
TOOLCHAIN_DIR:=$(TOPDIR)/staging_dir/$(TOOLCHAIN_DIR_NAME)
STAMP_DIR:=$(BUILD_DIR)/stamp
STAMP_DIR_HOST=$(BUILD_DIR_HOST)/stamp
TARGET_ROOTFS_DIR?=$(if $(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(BUILD_DIR))
TARGET_DIR:=$(TARGET_ROOTFS_DIR)/root-$(BOARD)
STAGING_DIR_ROOT:=$(STAGING_DIR)/root-$(BOARD)
STAGING_DIR_IMAGE:=$(STAGING_DIR)/image
BUILD_LOG_DIR:=$(TOPDIR)/logs
PKG_INFO_DIR := $(STAGING_DIR)/pkginfo
```

<h2 id="keywordef">Package關鍵字的define</h2>

Makefile中可能使用define關鍵字定義的常見section如下:
#### Package/{package-name}
	- CATEGORY:定義menuconfig中這個package會出現在哪個子目錄下
	- TITLE:針對本package的簡短描述
	- URL:這個package的官方網站
	- MAINTAINER:主要維護者

#### Package/{package-name}/conffiles (option)
	一份由這個package安裝的配置文件列表，每行一個文件

#### Package/{package-name}/description
	任何用来描述这個package的文字。

#### Build/Prepare (option)
	一些命令的集合，用於解包source code以及應用patch

#### Build/Configure (option)
	很多開源軟體的source在執行前要先執行`./configure`做一 些配置，這個section可以描述如何配置

#### Build/Compile (option)
	定義如何編譯此package，如果不定義openwrt會自動執行make 如果需要傳遞特殊參數可以參考$(call Build/Compile/Default,FOO=bar)

#### Build/Install (option)
	定義如何安裝編譯好的文件，預設是執行make install

#### Package/{package-name}/install
	一系列的命令集合，用於將文件拷貝到ipkg目錄(用$(1)表示)。

#### Package/{package-name}/preinst

------

<h2 id="depend">相依性</h2>

> 來源:https://openwrt.org/docs/guide-developer/package-policies

package可能依賴於其他的package以滿足編譯要求或強制執行特定功能，如shared library在target device上，有以下兩種`dependenices` :
    1. build dependencies:在`PKG_BUILD_DEPENDS`被定義
    2. runtime dependencies:在`DEPENDS`變數中宣告

`build dependence`的依賴關係在編譯時被使用，並指示build system在編譯前download、patch、compile每個提到的依賴關係

`runtime dependencies`描述運行時指令binary package的關係，指示package manager在安裝這個package之前先獲取並安裝列出的dependencies，一個runtime dependence自動的隱含了build dependencies，這代表了如果在來源define/Package之中的`DEPEND`定義了另一個package的define/Package名稱，則build system就會先編譯後者

package的相依性必須來自相同的feed或者是openwrt在`package/`目錄中提供的base feed

### shared library
儘管package的dependency機制會確保build system先編譯需要的libraray但不能保證library不會更新版本，並且在binary package庫中安裝不相容的版本也會破壞依賴關係的package，除非對依賴版本進行限制。

openwrt引進`ABI_VERSION`的概念，來解決program對特定版本的依賴關係，`ABI_VERSION`值被反映在被打包library的`SONAME`中。

shared library的實際檔名:格式為`lib+math+.so+主版本號+小版本號+製作號`
1.主版本號:代表目前共享庫的版本，如果提供的interface有變化的話，版本號+1
2.小版本號:如果引入了新的feature，但其餘沒變化，此版本號+1
3.製作號:通常只用來表示修正bug

`SONAME`(short for shared object name):他是可執行程式要載入lib時尋找的檔名，格式為`lib+math+.so+主版本號`
`link name`:專門為可執行程式生成階段連結共享庫時用的名子，不帶任何版本號

eg. 一個library package`libbar`會在編譯完成後提供以下檔案
```
libbar.so       -> libbar.so.1.2.3 (軟連結)
libbar.so.1     -> libbar.so.1.2.3 (軟連結)
libbar.so.1.2.3      (shared library object)
```

`$(INSTALL_DATA)`和`$(INSTALL_BIN)`將會將當前檔案複製到目標位置中，在實際上我們傾向使用`$(CP)`，他會保留軟連結

```
執行:
$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/libbar.so.* $(1)/usr/lib/
結果:
	libbar.so.1               (regular file)
	libbar.so.1.2.3           (regular file)
```
```
執行:
$(CP) $(PKG_INSTALL_DIR)/usr/lib/libbar.so.* $(1)/usr/lib/
結果:
    libbar.so.1     -> libbar.so.1.2.3 (symlink)
    libbar.so.1.2.3                    (regular file)
```




----

<h2 id="feeds">Feeds</h2>


在openwrt中，feeds是公用一個儲存空間的package集合，如遠端伺服器上，版本控制系統中，本機file system中，feeds都是一些為Openwrt buildroot預先定義好額外的packages的編譯方法。Openwrt預設許多可用的package，使用者可自由選擇是否要編譯某個package，如果需要的package不是openwrt自有的，可以從官方feeds中下載packages以便在openwrt中配置並編譯，也可以創建屬於自己的feeds，加入不屬於官方的packages。

### feeds的配置
Openwrt根目錄下，存在`feeds.conf.deafult`
feeds 設定檔由三個空格分隔的部分組成:`method`，`name`，`source`。
Example:

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
語法:
|method| funtion|
|---|------|
|src-bzr|從來源URL下載|
|src-cpy|從路徑複製|
|src-darcs |使用darcs從路徑或URL下載|
|src-git|使用git clone|
|src-link|創建與feed來源的軟連結|

feed method用於告知Openwrt要透過何種方法或協議來取得feed source所指向的數據。
feed name用於標示feeds，並作為文件目錄名稱，被創建用於保存該feeds相關訊息。
feed source則指名要從何處下載。

### feed命令
```shell
Usage: ./scripts/feeds <command> [options]

Commands:
        list [options]: List feeds, their content and revisions (if installed)
        Options:
            -n :            List of feed names.
            -s :            List of feed names and their URL.
            -r <feedname>:  List packages of specified feed.
            -d <delimiter>: Use specified delimiter to distinguish rows (default: spaces)

        install [options] <package>: Install a package
        Options:
            -a :           Install all packages from all feeds or from the specified feed using the -p option.
            -p <feedname>: Prefer this feed when installing packages.
            -d <y|m|n>:    Set default for newly installed packages.
            -f :           Install will be forced even if the package exists in core OpenWrt (override)

        search [options] <substring>: Search for a package
        Options:
            -r <feedname>: Only search in this feed

        uninstall -a|<package>: Uninstall a package
        Options:
            -a :           Uninstalls all packages.

        update -a|<feedname(s)>: Update packages and lists of feeds in feeds.conf .
        Options:
            -a :           Update all feeds listed within feeds.conf. Otherwise the specified feeds will be updated.
            -i :           Recreate the index only. No feed update from repository is performed.

        clean:             Remove downloaded/generated files.
```

### 自定義feeds

+ 在已存在的feed中加入package

	1. 下載openwrt項目並命名為openwrt
	2. 下載packages feed到此目錄下，並命名為packages
	3. 將自己的package添加到/openwrt/packages下的子目錄中
	4. 編輯feeds.conf如下
	```shell
	#srv-svn packages svn://svn.openwrt.org/openwrt/packages
src-link customfeed /home/user/openwrt/project/packages
	```
+ 添加一個全新的feed

	1. 創建項目目錄openwrt
	2. 建立package目錄並拷貝至/openwrt/project/customfeed/
	3. 編輯feeds.conf加入
	```shell
	src-link customfeed /home/user/openwrt/project/customfeed
	```


最後執行
1.  ./scripts/feeds update customfeed
2.  ./scripts/feeds install -p customfeed
3.  現在可在make menuconfig中看見自定義packages


----------

<h1 id="usage">Building Usage</h1>

> [Openwrt Usage Page](https://openwrt.org/docs/guide-developer/build-system/use-buildsystem)
> 

__請不要使用root或sudo執行 __

1. 更新source code 
2. 更新package feeds
3. 選擇特定的分支
4. 設定firmware image 選項
5. 開始build

### 1.更新source code 

```
git pull
```

### 2.更新package feeds

_安裝在 `./script/feeds`的context中意味著可在make menuconfig中設定_

更新所有feeds
```
./scripts/feeds update -a
```
在make menuconfig中顯示package
```shell
#單一
./scripts/feeds install <PACKAGENAME>
#所有
./scripts/feeds install -a
```

