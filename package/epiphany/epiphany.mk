################################################################################
#
# Epiphany game
#
################################################################################

EPIPHANY_VERSION = 0.1
EPIPHANY_SITE = package/epiphany/src
EPIPHANY_SITE_METHOD = local
EPIPHANY_DEPENDENCIES = sdl sdl_mixer zlib

$(eval $(autotools-package))
