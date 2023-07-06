################################################################################
#
# Mednafen emul
#
################################################################################

MEDNAFEN_VERSION = 0.1
MEDNAFEN_SITE = package/mednafen/src
MEDNAFEN_SITE_METHOD = local
MEDNAFEN_DEPENDENCIES = sdl
# flac

MEDNAFEN_CONF_OPTS += --disable-jack --disable-ssfplay --disable-ss --disable-sasplay --disable-psx --disable-pcfx --disable-lynx --disable-apple2 --disable-fancy-scalers --without-libflac \
--disable-debugger --disable-cjk-fonts --disable-snes-faust --disable-gba --disable-gb 


$(eval $(autotools-package))
