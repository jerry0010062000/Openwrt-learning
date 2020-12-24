include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hello-kmod
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

#submenu :在哪個菜單下
#title : 模組簡短描述
#files : 升成模組的位置
#autolaod : 是否自動裝載到kernel parv1:固定 2:模組的載入順序 3:模組名稱
#depends : 如果有依賴模組在此定義
define KernelPackage/hello-kmod
	SUBMENU:=Other modules
	TITLE:=Hello kernel drive  
	FILES:=$(PKG_BUILD_DIR)/hello-kmod.ko
	AUTOLOAD:=$(call AutoProbe,81,hello-kmod)
endef

#定義編譯目錄
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) -R ./src/*  $(PKG_BUILD_DIR)/
endef

#編譯
define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
	$(MAKE_OPTS) CONFIG_HELLO-KMOD=m \
		modules
endef
$(eval $(call KernelPackage,hello-kmod))