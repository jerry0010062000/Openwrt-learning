include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ubus_test
PKG_RELEASE:=1.0
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=Utilities
	CATEGORY:=Utilities
	TITLE:=UBUS_TEST
	DEPENDS:=+libubus +libubox +libuci
endef

define Package/ubus_test/description
	This package testing ubus api
endef

define Build/prepare
	mkdir -p $(PKG_BUILD_DIR)	
	$(CP) -v ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install 
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ubus_test.init $(1)/etc/init.d/ubus_test

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubus_server $(1)/usr/sbin/myserver
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubus_client $(1)/usr/sbin/myclient
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubus_sserver $(1)/usr/sbin/mySS
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubus_sclient $(1)/usr/sbin/mySC
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/read_uci $(1)/usr/sbin/myRUCI
endef

$(eval $(call BuildPackage,$(PKG_NAME)))