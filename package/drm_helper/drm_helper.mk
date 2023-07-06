################################################################################
#
# DRM helper
#
################################################################################

DRM_HELPER_VERSION = 0.1
DRM_HELPER_SITE = package/drm_helper/src
DRM_HELPER_SITE_METHOD = local
DRM_HELPER_DEPENDENCIES = libdrm
DRM_HELPER_INSTALL_STAGING = YES

DRM_HELPER_DRM_CFLAGS=-I $(STAGING_DIR)/usr/include/libdrm

define DRM_HELPER_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(DRM_HELPER_DRM_CFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS)" \
                -C $(@D)
                
endef

define DRM_HELPER_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/drm_test  $(TARGET_DIR)/usr/bin
endef

define DRM_HELPER_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/*.h  $(STAGING_DIR)/usr/include
	$(INSTALL) -D -m 0644 $(@D)/*.a  $(STAGING_DIR)/usr/lib
endef

$(eval $(generic-package))
