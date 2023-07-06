################################################################################
#
# lua-sdl
#
################################################################################

LUA_SDL_VERSION = 0.1
LUA_SDL_SITE = package/lua-sdl/src
LUA_SDL_SITE_METHOD = local
LUA_SDL_DEPENDENCIES = luainterpreter sdl

ifeq ($(BR2_PACKAGE_LUAJIT),y)
LUA_SDL_LUAVER = JIT
else ifeq ($(BR2_PACKAGE_LUA_5_3),y)
LUA_SDL_LUAVER = 5.3
else
LUA_SDL_LUAVER = 5.1
endif

LUA_SDL_CONF_OPTS += -DWITH_LUAVER=$(LUA_SDL_LUAVER) -DLUA_INCLUDE_DIR=$(STAGING_DIR)/usr/include

ifeq ($(BR2_PACKAGE_SDL_IMAGE),y)
LUA_SDL_DEPENDENCIES += sdl_image
else
LUA_SDL_CONF_OPTS += -DWITH_IMAGE=Off
endif

ifeq ($(BR2_PACKAGE_SDL_MIXER),y)
LUA_SDL_DEPENDENCIES += sdl_mixer
else
LUA_SDL_CONF_OPTS += -DWITH_MIXER=Off
endif

ifeq ($(BR2_PACKAGE_SDL_NET),y)
LUA_SDL_DEPENDENCIES += sdl_net
else
LUA_SDL_CONF_OPTS += -DWITH_NET=Off
endif

ifeq ($(BR2_PACKAGE_SDL_TTF),y)
LUA_SDL_DEPENDENCIES += sdl_ttf
else
LUA_SDL_CONF_OPTS += -DWITH_TTF=Off
endif

$(eval $(cmake-package))
