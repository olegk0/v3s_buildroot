################################################################################
#
# lua-newt
#
################################################################################

LUA_NEWT_VERSION = 0.2
LUA_NEWT_SITE = package/lua-newt/src
LUA_NEWT_LICENSE = MIT
LUA_NEWT_SITE_METHOD = local
LUA_NEWT_DEPENDENCIES = luainterpreter newt


define LUA_NEWT_BUILD_CMDS
	$(MAKE) -C $(@D) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS) -fPIC" \
		PREFIX="$(STAGING_DIR)/usr"
endef

define LUA_NEWT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/newt.so $(TARGET_DIR)/usr/lib/lua/$(LUAINTERPRETER_ABIVER)/
endef

$(eval $(generic-package))
