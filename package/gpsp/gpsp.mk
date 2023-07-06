################################################################################
#
# gpsp
#
################################################################################

GPSP_VERSION = 0.91
GPSP_SOURCE = main.zip
GPSP_SITE = https://github.com/hsinyuwang/gpsp/archive/refs/heads
GPSP_LICENSE = GPL-2.0
GPSP_LICENSE_FILES = COPYING LICENSE-CTK
GPSP_DEPENDENCIES = sdl drm_helper

#GPSP_CONF_OPTS = -DLINUX_AUDIO_BACKEND=Gr-audio

#ifeq ($(BR2_TOOLCHAIN_HAS_LIBATOMIC),y)
#GQRX_CONF_OPTS += -DCMAKE_EXE_LINKER_FLAGS=-latomic
#endif

define GPSP_EXTRACT_CMDS
        unzip $(GPSP_DL_DIR)/$(GPSP_SOURCE) -d $(@D)
        mv $(@D)/`ls $(@D)`/* $(@D)
        #cp $(HOST_GPSP_PKGDIR)/Makefile $(@D)
endef


GPSP_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --cflags`
GPSP_DRM_CFLAGS=-I $(STAGING_DIR)/usr/include/libdrm

define GPSP_BUILD_CMDS
        $(TARGET_MAKE_ENV) $(MAKE) PLATFORM=UNIX \
                CC="$(TARGET_CC)" \
                STRIP="/bin/true" \
                SDL_CONFIG="$(STAGING_DIR)/usr/bin/sdl-config" \
		CFLAGS="$(TARGET_CFLAGS) -DARM_ARCH -DPC_BUILD $(GPSP_SDL_CFLAGS) $(GPSP_DRM_CFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS) -lm" \
                -C $(@D)
                
endef

define GPSP_INSTALL_TARGET_CMDS
        cp $(@D)/gpsp $(TARGET_DIR)/usr/bin/
	mkdir -p $(TARGET_DIR)/root/gpsp
	cp $(@D)/game_config.txt $(TARGET_DIR)/root/gpsp/
endef

$(eval $(generic-package))
