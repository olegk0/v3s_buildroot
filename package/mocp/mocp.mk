################################################################################
#
# mocp
#
################################################################################

MOCP_VERSION = 2.6.alpha3
#MOCP_SITE = https://github.com/jonsafari/mocp/archive/refs/heads
#MOCP_SOURCE = master.zip
#MOCP_SITE_METHOD = git
MOCP_SITE = package/mocp/src
MOCP_SITE_METHOD = local

MOCP_LICENSE = GPL-2.0
MOCP_LICENSE_FILES = COPYING
#MOCP_INSTALL_STAGING = YES
MOCP_DEPENDENCIES += berkeleydb libtool

#define MOCP_EXTRACT_CMDS
#        unzip $(MOCP_DL_DIR)/$(MOCP_SOURCE) -d $(@D)
#        mv $(@D)/`ls $(@D)`/* $(@D)
#endef

#MOCP_AUTORECONF = YES

MOCP_CONF_OPTS += \
	--with-bdb-dir=$(STAGING_DIR)/usr BDB_VERSION=5.3.28

$(eval $(autotools-package))
