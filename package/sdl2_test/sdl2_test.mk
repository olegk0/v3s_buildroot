################################################################################
#
# SDL2 test
#
################################################################################

SDL2_TEST_VERSION = 0.1
SDL2_TEST_SITE = package/sdl2_test/src
#SDL2_TEST_SITE = sdl2_test
SDL2_TEST_SITE_METHOD = local
SDL2_TEST_DEPENDENCIES = sdl2


SDL2_TEST_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`
SDL2_TEST_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl2-config --libs`

define SDL2_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(SDL2_TEST_SDL_CFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS) $(SDL2_TEST_SDL_LDFLAGS)" \
                -C $(@D)
                
endef

define SDL2_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/sdl2_test  $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
