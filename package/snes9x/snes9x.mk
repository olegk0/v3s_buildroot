################################################################################
#
# SNES9x emul
#
################################################################################

SNES9X_VERSION = 1.53.0
SNES9X_SITE = package/snes9x/src
SNES9X_SITE_METHOD = local
SNES9X_DEPENDENCIES = sdl
# flac


SNES9X_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --cflags`
SNES9X_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --libs`

#SNES9X_CONF_OPTS += --disable-jack --disable-ssfplay --disable-ss --disable-sasplay --disable-psx --disable-pcfx --disable-lynx --disable-apple2 --disable-fancy-scalers --without-libflac \
#--disable-debugger --disable-cjk-fonts --disable-snes-faust --disable-md --disable-pce --disable-snes --disable-nes --disable-sms --disable-wswan \
#--disable-vb --disable-pce-fast --disable-ngp 
SNES9X_CONF_OPTS = --enable-neon --disable-screenshot snes9x_cv_option_g=yes snes9x_cv_option_o3=yes snes9x_cv_option_omit_frame_pointer=yes \
        snes9x_cv_option_mfpu="yes" snes9x_cv_option_march="yes" snes9x_cv_option_prefetch="yes" snes9x_cv_option_no_exceptions="yes" \
        snes9x_cv_option_no_rtti="yes" snes9x_cv_option_pedantic="yes" snes9x_cv_option_Wall="yes" snes9x_cv_option_W="yes"\
        snes9x_cv_option_Wno_unused_parameter="yes" snes9x_sar_int8="yes" snes9x_sar_int16="yes" snes9x_sar_int32="yes" snes9x_sar_int64="yes" \
        SDL_CFLAGS="$(SNES9X_SDL_CFLAGS)" \
        SDL_LDLAGS="$(SNES9X_SDL_LDFLAGS)" \
        --with-sdl-prefix=$(STAGING_DIR)/usr \
        --with-sdl-exec-prefix=$(STAGING_DIR)/usr

define SNES9X_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0755 $(@D)/snes9x-sdl  $(TARGET_DIR)/usr/bin
endef

$(eval $(autotools-package))
