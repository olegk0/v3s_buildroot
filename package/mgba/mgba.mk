################################################################################
#
# MGBA emul
#
################################################################################

MGBA_VERSION = 0.10.2
MGBA_SITE = package/mgba/src
MGBA_SITE_METHOD = local
MGBA_DEPENDENCIES = sdl


#MGBA_SDL_CFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --cflags`
#MGBA_SDL_LDFLAGS=`$(STAGING_DIR)/usr/bin/sdl-config --libs`

MGBA_CONF_OPTS = -DBUILD_LIBRETRO=OFF -DUSE_SQLITE3=OFF -DUSE_PNG=OFF -DUSE_FFMPEG=OFF -DBUILD_QT=OFF \
		 -DBUILD_GL=OFF -DBUILD_GLES2=OFF -DBUILD_GLES3=OFF -DUSE_EPOXY=OFF -DSDL_VERSION=1 -DUSE_LUA=OFF -DENABLE_SCRIPTING=OFF \
		 -DBUILD_SHARED=OFF -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Debug
# -DUSE_DEBUGGERS=OFF -DENABLE_IO_THREAD=OFF
#USE_DEBUGGERS
#-DUSE_EDITLINE=ON

define MGBA_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0755 $(@D)/sdl/mgba  $(TARGET_DIR)/usr/bin
#        $(INSTALL) -D -m 0644 $(@D)/libmgba.so.0.8.3  $(TARGET_DIR)/usr/lib
endef

$(eval $(cmake-package))
