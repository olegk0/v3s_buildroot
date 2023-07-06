################################################################################
#
# SDL test
#
################################################################################

SDL_TEST_VERSION = 0.1
SDL_TEST_SITE = package/sdl_test/src
SDL_TEST_SITE_METHOD = local
SDL_TEST_DEPENDENCIES = sdl


SDL_TEST_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --cflags`
SDL_TEST_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --libs`

define SDL_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(SDL_TEST_SDL_CFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS) $(SDL_TEST_SDL_LDFLAGS)" \
                -C $(@D)
                
endef

define SDL_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/sdl_test  $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
