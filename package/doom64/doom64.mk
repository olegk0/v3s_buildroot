################################################################################
#
# Doom64
#
################################################################################

DOOM64_VERSION = 0.1
DOOM64_SITE = package/doom64/src
DOOM64_SITE_METHOD = local
DOOM64_DEPENDENCIES = sdl2 sdl2_net
DOOM64_INSTALL_STAGING = YES


DOOM64_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`
DOOM64_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl2-config --libs`

#define DOOM64_BUILD_CMDS
#        $(TARGET_MAKE_ENV) $(MAKE) \
#                CC="$(TARGET_CC)" \
#                CFLAGS="$(TARGET_CFLAGS) $(DOOM64_SDL_CFLAGS)" \
#                LDFLAGS="$(TARGET_LDFLAGS) $(DOOM64_SDL_LDFLAGS)" \
#                -C $(@D)
#endef

sample_CONF_OPTS = \
        -DCMAKE_BUILD_TYPE=Release \
        -DMDBX_INSTALL_MANPAGES=OFF \
        -DBUILD_FOR_NATIVE_CPU=OFF \
        -DMDBX_BUILD_CXX=$(if $(BR2_PACKAGE_LIBMDBX_CXX),ON,OFF) \
        -DMDBX_BUILD_TOOLS=$(if $(BR2_PACKAGE_LIBMDBX_TOOLS),ON,OFF)


define DOOM64_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/drm_test  $(TARGET_DIR)/usr/bin
endef

define DOOM64_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/*.h  $(STAGING_DIR)/usr/include
	$(INSTALL) -D -m 0644 $(@D)/*.a  $(STAGING_DIR)/usr/lib
endef

$(eval $(cmake-package))
