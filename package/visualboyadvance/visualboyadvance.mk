################################################################################
#
# VISUALBOYADVANCE emul
#
################################################################################

VISUALBOYADVANCE_VERSION = 1.8.0
VISUALBOYADVANCE_SITE = package/visualboyadvance/src
VISUALBOYADVANCE_SITE_METHOD = local
VISUALBOYADVANCE_DEPENDENCIES = sdl
# flac

#VISUALBOYADVANCE_CONF_OPTS += --disable-jack --disable-ssfplay --disable-ss --disable-sasplay --disable-psx --disable-pcfx --disable-lynx --disable-apple2 --disable-fancy-scalers --without-libflac \
#--disable-debugger --disable-cjk-fonts --disable-snes-faust --disable-md --disable-pce --disable-snes --disable-nes --disable-sms --disable-wswan \
#--disable-vb --disable-pce-fast --disable-ngp 
VISUALBOYADVANCE_CONF_OPTS = --disable-profiling \
        CXXFLAGS="$(TARGET_CXXFLAGS) -DFINAL_VERSION" \
        --with-sdl-prefix=$(STAGING_DIR)/usr \
        --with-sdl-exec-prefix=$(STAGING_DIR)/usr


$(eval $(autotools-package))
