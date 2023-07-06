################################################################################
#
# GRAFX2 
#
################################################################################

GRAFX2_VERSION = 2.0
GRAFX2_SITE = package/grafx2/src
GRAFX2_SITE_METHOD = local
GRAFX2_DEPENDENCIES = sdl sdl_image sdl_ttf


#GRAFX2_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --cflags`
#GRAFX2_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --libs`
GRAFX2_SDL_CONFIG=$(STAGING_DIR)/usr/bin/sdl-config

define GRAFX2_BUILD_CMDS
	$(TARGET_MAKE_ENV) NORECOIL=1 NO_X11=1 SDLCONFIG=$(GRAFX2_SDL_CONFIG) NO_TIFF=1 PREFIX=/usr $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
                -C $(@D)/src
                
endef

define GRAFX2_INSTALL_TARGET_CMDS
#	$(INSTALL) -D -m 0755 $(@D)/bin/grafx2-sdl  $(TARGET_DIR)/usr/bin
	$(TARGET_MAKE_ENV) PREFIX=/usr NORECOIL=1 NO_X11=1 NO_TIFF=1 $(MAKE) DESTDIR=$(TARGET_DIR) -C $(@D)/src install
endef

$(eval $(generic-package))
