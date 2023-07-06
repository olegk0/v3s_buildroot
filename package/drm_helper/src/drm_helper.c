/*
 * 2023
 *
 * olegvedi@gmail.com
 *
 * used parts from:
 *
 *
 * DRM based mode setting test program
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "drm_helper.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xf86drm.h>

typedef struct _plane_s
{
  uint32_t plane_id; // ro
  // Formats supported
  uint32_t count_formats; // ro
  uint32_t* formats;      // ro
  // Props
  uint32_t count_props;          // ro
  drmModePropertyPtr properties; // ro
  // Zpos
  int zpos_min; // ro
  int zpos_max; // ro
  // FB
  DhPlaneInfo plane_info;
  uint16_t crtc_w, crtc_h;
} plane_s;

typedef struct _dev_s
{
  int fd;
  //	connector_s *connectors;
  uint32_t conn_id, enc_id;
  // CRTC
  uint32_t crtc_id;
  drmModeCrtc crtc;
  drmModeModeInfo mode_info;
  drmModeCrtcPtr saved_crtc;
  // Primary Fb
  drmModeFB primary_fb_info;
  DhPlaneInfo primary_fb;
  //  Planes
  int count_planes;
  plane_s* planes;
  //
  DhHwInfo dh_hw_info;
} dev_s;

static dev_s* dev = NULL;

static int
drm_open(const char* path)
{
  int fd;
  uint64_t has_dumb;

  if ((fd = open(path, O_RDWR | O_CLOEXEC)) < 0) {
    fprintf(stderr, "drmOpen failed or doesn't have dumb buffer");
  } else {
    /* check capability */
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || has_dumb == 0) {
      fprintf(stderr,
              "drmGetCap DRM_CAP_DUMB_BUFFER failed or doesn't "
              "have dumb buffer");
      close(fd);
      fd = -1;
    }
  }
  return fd;
}

static int
drm_find_crt(void)
{
  int ret = -ENXIO;

  drmModeRes* res = drmModeGetResources(dev->fd);
  if (!res) {
    fprintf(stderr, "drmModeGetResources() failed");
  } else {
    // if (res->count_connectors > 0) {
    // dev->connectors = (connector_s
    // *)calloc(res->count_connectors,sizeof(connector_s));
    //  connectors loop
    int i = 0;
    while (i < res->count_connectors && ret) {
      dev->crtc_id = 0;
      // connector_s *connector = &dev->connectors[i];
      // connector->conn_id = -1;
      drmModeConnector* conn = drmModeGetConnector(dev->fd, res->connectors[i]);
      if (!conn) {
        fprintf(stderr, "drmModeGetConnector() failed");
      } else {
        if (conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0) {
          // connector->conn_id = conn->connector_id;
          // connector->enc_id = conn->encoder_id;
          // memcpy(&connector->modeInfo, &conn->modes[0],
          // sizeof(drmModeModeInfo));
          //  FIXME: use default encoder/crtc pair
          drmModeEncoderPtr enc = drmModeGetEncoder(dev->fd, conn->encoder_id);
          if (!enc) {
            fprintf(stderr, "drmModeGetEncoder() failed");
          } else {
            dev->crtc_id = enc->crtc_id;
            dev->conn_id = conn->connector_id;
            dev->enc_id = conn->encoder_id;
            drmModeFreeEncoder(enc);
          }
        }

        if (dev->crtc_id > 0) {
          drmModeCrtcPtr crtc = drmModeGetCrtc(dev->fd, dev->crtc_id);
          if (!crtc) {
            fprintf(stderr, "drmModeGetCrtc() failed");
            dev->crtc_id = 0;
          } else {
            memcpy(&dev->crtc, crtc, sizeof(drmModeCrtc));
            memcpy(&dev->mode_info, &conn->modes[0], sizeof(drmModeModeInfo));
            drmModeFBPtr fb = drmModeGetFB(dev->fd, crtc->buffer_id);
            if (!fb) {
              fprintf(stderr, "drmModeGetFB2() failed\n");
            } else {
              dev->primary_fb.type = dh_plane_type_primary;
              dev->dh_hw_info.count_planes[dh_plane_type_primary] = 1;
              dev->dh_hw_info.height = fb->height;
              dev->dh_hw_info.width = fb->width;
              dev->dh_hw_info.bpp = fb->bpp;
              dev->dh_hw_info.depth = fb->depth;
              dev->dh_hw_info.pitch = fb->pitch;
              memcpy(&dev->primary_fb_info, fb, sizeof(drmModeFB));
              ret = 0;
              drmModeFreeFB(fb);
            }
            drmModeFreeCrtc(crtc);
          }
        }
        drmModeFreeConnector(conn);
      }

      i++;
    } // loop connectors
    /*} else {
            fprintf(stderr, "connectors not found");
    }*/
    drmModeFreeResources(res);
  }
  return ret;
}

static int
parse_planes(dev_s* dev)
{
  int ret = -ENXIO;
  drmModePlaneRes* plane_resources = drmModeGetPlaneResources(dev->fd);
  if (!plane_resources) {
    fprintf(stderr, "drmModeGetPlaneResources failed: %s\n", strerror(errno));
  } else {
    dev->planes =
      (plane_s*)calloc(plane_resources->count_planes, sizeof(plane_s));
    if (!dev->planes) {
      fprintf(stderr, "error memory alloc for cur_plane->formats\n");
    } else {
      dev->count_planes = plane_resources->count_planes;
      printf("Planes:\n");
      printf("id\tcrtc\tfb\tCRTC x,y\tx,y\tgamma size\tpossible crtcs\n");
      for (int pli = 0; pli < plane_resources->count_planes; pli++) {
        plane_s* cur_plane = &dev->planes[pli];
        cur_plane->plane_info.plane_uid = pli + 1;
        drmModePlane* ovr =
          drmModeGetPlane(dev->fd, plane_resources->planes[pli]);
        if (!ovr) {
          printf("drmModeGetPlane failed: %s\n", strerror(errno));
          continue;
        }
        cur_plane->count_formats = ovr->count_formats;
        cur_plane->formats =
          malloc(sizeof(cur_plane->formats) * ovr->count_formats);
        if (cur_plane->formats) {
          memcpy(cur_plane->formats,
                 ovr->formats,
                 sizeof(cur_plane->formats) * ovr->count_formats);
        } else {
          fprintf(stderr, "error memory alloc for cur_plane->formats\n");
        }
        printf("%d\t%d\t%d\t%d,%d\t\t%d,%d\t%-8d\t0x%08x\n",
               ovr->plane_id,
               ovr->crtc_id,
               ovr->fb_id,
               ovr->crtc_x,
               ovr->crtc_y,
               ovr->x,
               ovr->y,
               ovr->gamma_size,
               ovr->possible_crtcs);

        cur_plane->plane_id = ovr->plane_id;
        drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(
          dev->fd, plane_resources->planes[pli], DRM_MODE_OBJECT_PLANE);
        if (props) {
          printf("Property:\n");
          printf("id\tname\tvalue\n");

          cur_plane->count_props = 0;
          cur_plane->properties = (drmModePropertyPtr)calloc(
            props->count_props, sizeof(drmModePropertyRes));
          if (!cur_plane->properties) {
            fprintf(stderr, "error memory alloc for cur_plane->properties\n");
          } else {
            for (int pri = 0; pri < props->count_props; pri++) {
              drmModePropertyPtr prop =
                drmModeGetProperty(dev->fd, props->props[pri]);
              if (!prop) {
                fprintf(
                  stderr, "drmModeGetProperty failed: %s\n", strerror(errno));
              } else {
                memcpy(&cur_plane->properties[cur_plane->count_props],
                       prop,
                       sizeof(drmModePropertyRes));
                cur_plane->count_props++;
                printf("%d\t%s\t%" PRIu64 "\n",
                       prop->prop_id,
                       prop->name,
                       props->prop_values[pri]);
                if (strcmp(prop->name, "zpos") == 0) {
                  // DRM_MODE_PROP_RANGE or
                  // DRM_MODE_PROP_SIGNED_RANGE
                  if (prop->count_values > 1) {
                    cur_plane->zpos_min = prop->values[0];
                    cur_plane->zpos_max = prop->values[1];
                  }
                  cur_plane->plane_info.zpos = props->prop_values[pri];
                  // set_plane_property(dev->fd, ovr->plane_id,
                  // prop->name, prop->prop_id, 2); //0-bg 1-fg
                  // 2-cursor
                } else if (strcmp(prop->name, "type") == 0) {
                  cur_plane->plane_info.type = props->prop_values[pri];
                  if (cur_plane->plane_info.type <= dh_plane_type_cursor) {
                    dev->dh_hw_info.count_planes[cur_plane->plane_info.type]++;
                  }
                }
                ret = 0;
                // dump_prop(dev->fd, prop, props->props[pri],
                // props->prop_values[pri]);
                drmModeFreeProperty(prop);
              }
            }
            // free(cur_plane->properties);
            // cur_plane->properties = NULL;
          }
          drmModeFreeObjectProperties(props);
        }
        // set_plane(dev->fd, dev, ovr->plane_id, 200, 200, 2);
        drmModeFreePlane(ovr);
      }
      printf("\n");
    }
    drmModeFreePlaneResources(plane_resources);
  }
  return ret;
}

static int
set_property_by_id(int object_id,
                   uint32_t object_type,
                   uint32_t prop_id,
                   uint64_t value)
{
  //	if (!dev->use_atomic)
  int ret =
    drmModeObjectSetProperty(dev->fd, object_id, object_type, prop_id, value);
  //	else
  //		ret = drmModeAtomicAddProperty(dev->req, plane_id, prop_id,
  // value);
  if (ret < 0) {
    fprintf(stderr,
            "failed to set property %d for object %d to %" PRIu64 ": %s\n",
            prop_id,
            object_id,
            value,
            strerror(errno));
  }

  return ret;
}

static int
set_plane_property_by_name(plane_s* plane, char* prop_name, uint64_t value)
{
  for (int i = 0; i < plane->count_props; i++) {
    if (strcmp(plane->properties[i].name, prop_name) == 0) {
      return set_property_by_id(plane->plane_id,
                                DRM_MODE_OBJECT_PLANE,
                                plane->properties[i].prop_id,
                                value);
    }
  }

  return -ENXIO;
}

//************************************ BO management
//***********************************
static int
bo_map(int fd, DhPlaneInfo* plane_info, DhBufInfo* buf_info)
{
  void* map;
  int ret;
  uint64_t offset;

  ret = drmModeMapDumbBuffer(fd, buf_info->fbi.handles[0], &offset);
#ifdef DEBUG
  fprintf(stderr,
          "drmModeMapDumbBuffer, handle: %d, offset: %" PRIu64 ", ret: %d\n",
          buf_info->fbi.handles[0],
          offset,
          ret);
#endif
  if (ret)
    return ret;

  map = mmap(
    0, plane_info->fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
#ifdef DEBUG
  fprintf(stderr,
          "mmap, map_plane: %p,  size:%" PRIu64 "\n",
          map,
          plane_info->fb_size);
#endif
  if (map == MAP_FAILED)
    return -EINVAL;

  buf_info->map_planes[0] = map;

  return 0;
}

static void
bo_unmap(DhPlaneInfo* plane_info, DhBufInfo* buf_info)
{
  if (!buf_info->map_planes[0])
    return;
#ifdef DEBUG
  fprintf(stderr,
          "munmap, map_plane: %p,  size:%" PRIu64 "\n",
          buf_info->map_planes[0],
          plane_info->fb_size);
#endif
  munmap(buf_info->map_planes[0], plane_info->fb_size);
  buf_info->map_planes[0] = NULL;
}

void
bo_destroy(int fd, DhBufInfo* buf_info)
{
  int ret;
#ifdef DEBUG
  fprintf(
    stderr, "destroy dumb buffer, handle: %d\n", buf_info->fbi.handles[0]);
#endif
  ret = drmModeDestroyDumbBuffer(fd, buf_info->fbi.handles[0]);
  if (ret)
    fprintf(stderr, "failed to destroy dumb buffer: %s\n", strerror(errno));
}

int
bo_create(int fd, DhPlaneInfo* plane_info, DhBufInfo* buf_info)
{
  unsigned int virtual_height;
  unsigned int bpp;
  int ret = -EINVAL;

  switch (buf_info->fbi.pixel_format) {
    case DRM_FORMAT_C8:
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
      bpp = 8;
      break;

    case DRM_FORMAT_ARGB4444:
    case DRM_FORMAT_XRGB4444:
    case DRM_FORMAT_ABGR4444:
    case DRM_FORMAT_XBGR4444:
    case DRM_FORMAT_RGBA4444:
    case DRM_FORMAT_RGBX4444:
    case DRM_FORMAT_BGRA4444:
    case DRM_FORMAT_BGRX4444:
    case DRM_FORMAT_ARGB1555:
    case DRM_FORMAT_XRGB1555:
    case DRM_FORMAT_ABGR1555:
    case DRM_FORMAT_XBGR1555:
    case DRM_FORMAT_RGBA5551:
    case DRM_FORMAT_RGBX5551:
    case DRM_FORMAT_BGRA5551:
    case DRM_FORMAT_BGRX5551:
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
    case DRM_FORMAT_UYVY:
    case DRM_FORMAT_VYUY:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_YVYU:
      bpp = 16;
      break;

    case DRM_FORMAT_BGR888:
    case DRM_FORMAT_RGB888:
      bpp = 24;
      break;

    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_RGBX8888:
    case DRM_FORMAT_BGRA8888:
    case DRM_FORMAT_BGRX8888:
    case DRM_FORMAT_ARGB2101010:
    case DRM_FORMAT_XRGB2101010:
    case DRM_FORMAT_ABGR2101010:
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_RGBA1010102:
    case DRM_FORMAT_RGBX1010102:
    case DRM_FORMAT_BGRA1010102:
    case DRM_FORMAT_BGRX1010102:
      bpp = 32;
      break;

    case DRM_FORMAT_XRGB16161616F:
    case DRM_FORMAT_XBGR16161616F:
    case DRM_FORMAT_ARGB16161616F:
    case DRM_FORMAT_ABGR16161616F:
      bpp = 64;
      break;

    default:
      fprintf(
        stderr, "unsupported format 0x%08x\n", buf_info->fbi.pixel_format);
      return ret;
  }

  plane_info->bpp = bpp;

  switch (buf_info->fbi.pixel_format) {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
      virtual_height = buf_info->fbi.height * 3 / 2;
      break;

    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
      virtual_height = buf_info->fbi.height * 2;
      break;

    default:
      virtual_height = buf_info->fbi.height;
      break;
  }

  ret = drmModeCreateDumbBuffer(fd,
                                buf_info->fbi.width + 1,
                                virtual_height,
                                plane_info->bpp,
                                0,
                                &buf_info->fbi.handles[0],
                                &buf_info->fbi.pitches[0],
                                &plane_info->fb_size);

#ifdef DEBUG
  fprintf(stderr,
          "drmModeCreateDumbBuffer, handle: %d, pitch: %d, fb_size: %" PRIu64
          "\n",
          buf_info->fbi.handles[0],
          buf_info->fbi.pitches[0],
          plane_info->fb_size);
#endif
  if (ret) {
    fprintf(stderr, "failed to create dumb buffer: %s\n", strerror(errno));
    return ret;
  }

  ret = bo_map(fd, plane_info, buf_info);
  if (ret) {
    fprintf(stderr, "failed to map buffer: %s\n", strerror(-errno));
    bo_destroy(fd, buf_info);
    return ret;
  }

  /* just testing a limited # of formats to test single
   * and multi-planar path.. would be nice to add more..
   */
  switch (buf_info->fbi.pixel_format) {
    case DRM_FORMAT_UYVY:
    case DRM_FORMAT_VYUY:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_YVYU:
      buf_info->fbi.offsets[0] = 0;
      // handles[0] = bo->handle;
      // pitches[0] = bo->pitch;
      // fbi->map_planes[0] = fbi->fb_buf;
      break;

    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
      buf_info->fbi.offsets[0] = 0;
      // handles[0] = bo->handle;
      // pitches[0] = bo->pitch;
      buf_info->fbi.pitches[1] = buf_info->fbi.pitches[0];
      buf_info->fbi.offsets[1] =
        buf_info->fbi.pitches[0] * buf_info->fbi.height;
      buf_info->fbi.handles[1] = buf_info->fbi.handles[0];
      // fbi->map_planes[0] = fbi->fb_buf;
      buf_info->map_planes[1] =
        buf_info->map_planes[0] + buf_info->fbi.offsets[1];
      break;

    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
      buf_info->fbi.offsets[0] = 0;
      // handles[0] = bo->handle;
      // pitches[0] = bo->pitch;
      buf_info->fbi.pitches[1] = buf_info->fbi.pitches[0] / 2;
      buf_info->fbi.offsets[1] =
        buf_info->fbi.pitches[0] * buf_info->fbi.height;
      buf_info->fbi.handles[1] = buf_info->fbi.handles[0];
      buf_info->fbi.pitches[2] = buf_info->fbi.pitches[1];
      buf_info->fbi.offsets[2] =
        buf_info->fbi.offsets[1] +
        buf_info->fbi.pitches[1] * buf_info->fbi.height / 2;
      buf_info->fbi.handles[2] = buf_info->fbi.handles[0];
      // fbi->map_planes[0] = fbi->fb_buf;
      buf_info->map_planes[1] =
        buf_info->map_planes[0] + buf_info->fbi.offsets[1];
      buf_info->map_planes[2] =
        buf_info->map_planes[0] + buf_info->fbi.offsets[2];
      break;

    case DRM_FORMAT_C8:
    case DRM_FORMAT_ARGB4444:
    case DRM_FORMAT_XRGB4444:
    case DRM_FORMAT_ABGR4444:
    case DRM_FORMAT_XBGR4444:
    case DRM_FORMAT_RGBA4444:
    case DRM_FORMAT_RGBX4444:
    case DRM_FORMAT_BGRA4444:
    case DRM_FORMAT_BGRX4444:
    case DRM_FORMAT_ARGB1555:
    case DRM_FORMAT_XRGB1555:
    case DRM_FORMAT_ABGR1555:
    case DRM_FORMAT_XBGR1555:
    case DRM_FORMAT_RGBA5551:
    case DRM_FORMAT_RGBX5551:
    case DRM_FORMAT_BGRA5551:
    case DRM_FORMAT_BGRX5551:
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
    case DRM_FORMAT_BGR888:
    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_RGBA8888:
    case DRM_FORMAT_RGBX8888:
    case DRM_FORMAT_BGRA8888:
    case DRM_FORMAT_BGRX8888:
    case DRM_FORMAT_ARGB2101010:
    case DRM_FORMAT_XRGB2101010:
    case DRM_FORMAT_ABGR2101010:
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_RGBA1010102:
    case DRM_FORMAT_RGBX1010102:
    case DRM_FORMAT_BGRA1010102:
    case DRM_FORMAT_BGRX1010102:
    case DRM_FORMAT_XRGB16161616F:
    case DRM_FORMAT_XBGR16161616F:
    case DRM_FORMAT_ARGB16161616F:
    case DRM_FORMAT_ABGR16161616F:
      buf_info->fbi.offsets[0] = 0;
      // handles[0] = bo->handle;
      // pitches[0] = bo->pitch;
      // fbi->map_planes[0] = virtual;
      break;
  }

  //	util_fill_pattern(format, pattern, planes, width, height, pitches[0]);
  //	bo_unmap(bo);

  return ret;
}
//***********************************************************************
void
free_plane_info(DhPlaneInfo* plane_info)
{
  for (int i = 0; i < 3; i++) {
    DhBufInfo* buf_info = &plane_info->buf_info[i];
    if (buf_info->map_planes[0]) {
#ifdef DEBUG
      fprintf(
        stderr, "free_plane_info: %d, fb_id: %d\n", i, buf_info->fbi.fb_id);
#endif
      drmModeRmFB(dev->fd, buf_info->fbi.fb_id);
      bo_unmap(plane_info, buf_info);
      bo_destroy(dev->fd, buf_info);
    }
    buf_info->buf_ready = false;
  }
  plane_info->count_bufs = 0;
  plane_info->exposed_buf = 0;
  plane_info->opened_buf = 0;
  plane_info->back_buf = 0;
}

static int
bo_fb_create(int fd,
             DhPlaneInfo* plane_info,
             uint16_t width,
             uint16_t height,
             uint32_t fourcc_format,
             uint8_t count_bufs)
{
#ifdef DEBUG
  fprintf(stderr,
          "bo_fb_create, width: %d,  height: %d,  fourcc_format: %d,  "
          "count_bufs: %d\n",
          width,
          height,
          fourcc_format,
          count_bufs);
#endif
  int ret = 0;
  for (int i = 0; i < count_bufs; i++) {
    DhBufInfo* buf_info = &plane_info->buf_info[i];

    buf_info->fbi.width = width;
    buf_info->fbi.height = height;
    buf_info->fbi.pixel_format = fourcc_format;

    ret = bo_create(fd, plane_info, buf_info);

    if (ret) {
      break;
    }

    ret = drmModeAddFB2(fd,
                        buf_info->fbi.width,
                        buf_info->fbi.height,
                        buf_info->fbi.pixel_format,
                        buf_info->fbi.handles,
                        buf_info->fbi.pitches,
                        buf_info->fbi.offsets,
                        &buf_info->fbi.fb_id,
                        0);

#ifdef DEBUG
    fprintf(stderr, "drmModeAddFB2, fb_id: %d\n", buf_info->fbi.fb_id);
#endif
    if (ret) {
      fprintf(stderr,
              "failed to add fb (%ux%u): %s\n",
              buf_info->fbi.width,
              buf_info->fbi.height,
              strerror(errno));
      bo_unmap(plane_info, buf_info);
      bo_destroy(fd, buf_info);
      break;
    }
  }

  if (ret) {
    free_plane_info(plane_info);
  } else {
    plane_info->count_bufs = count_bufs;
    plane_info->exposed_buf = 0;
    plane_info->buf_info[0].buf_ready = 1;
    switch (count_bufs) {
      case 2:
        plane_info->opened_buf = 1;
        plane_info->back_buf = 1;
        break;
      case 3:
        plane_info->opened_buf = 2;
        plane_info->back_buf = 1;
        break;
      default:
        plane_info->opened_buf = 0;
        plane_info->back_buf = 0;
    }
  }

  return ret;
}

//******************************
// Primary*****************************************
static void
free_primary_fb()
{
#ifdef DEBUG
  fprintf(stderr, "free_primary_fb\n");
#endif
  if (dev->saved_crtc) {
    drmModeSetCrtc(dev->fd,
                   dev->saved_crtc->crtc_id,
                   dev->saved_crtc->buffer_id,
                   dev->saved_crtc->x,
                   dev->saved_crtc->y,
                   &dev->conn_id,
                   1,
                   &dev->saved_crtc->mode);
    drmModeFreeCrtc(dev->saved_crtc);
    dev->saved_crtc = NULL;
  }
  free_plane_info(&dev->primary_fb);
}

static int
alloc_primary_fb(uint32_t fourcc_format,
                 uint16_t width,
                 uint16_t height,
                 uint16_t x,
                 uint16_t y,
                 uint8_t count_bufs)
{
#ifdef DEBUG
  fprintf(stderr, "alloc_primary_fb\n");
#endif
  DhPlaneInfo* primary_fb = &dev->primary_fb;
  if (primary_fb->buf_info[0].map_planes[0]) {
    fprintf(stderr, "primary_fb in use\n");
    return -EBUSY;
  }

  if (width == 0) {
    width = dev->primary_fb_info.width;
    //		x = 0;
  }

  if (height == 0) {
    height = dev->primary_fb_info.height;
    //		y = 0;
  }

  /* just use single plane format for now.. */
  int ret =
    bo_fb_create(dev->fd, primary_fb, width, height, fourcc_format, count_bufs);
  if (ret == 0) {
#ifdef DEBUG
    printf("fd:%d, primary_fb, dev->crtc_id:%d, count_bufs:%d\n",
           dev->fd,
           dev->crtc_id,
           count_bufs);
#endif
    dev->saved_crtc =
      drmModeGetCrtc(dev->fd, dev->crtc_id); // must store crtc data
    if (!dev->saved_crtc) {
      fprintf(stderr, "drmModeGetCrtc failed\n");
    } else {
      if (drmModeSetCrtc(dev->fd,
                         dev->crtc_id,
                         primary_fb->buf_info[0].fbi.fb_id,
                         x,
                         y,
                         &dev->conn_id,
                         1,
                         &dev->mode_info) == 0) {
        primary_fb->x = x;
        primary_fb->y = y;
        return 0;
      }
      fprintf(stderr, "drmModeSetCrtc failed\n");
    }

    fprintf(stderr, "failed to enable primary_fb: %s\n", strerror(errno));
    free_primary_fb();
  }
  return ret;
}

//******************************
// Planes*****************************************

int
set_plane_property_zpos(plane_s* plane, uint8_t value)
{
  int ret = 0;

  if (value < plane->zpos_min || value > plane->zpos_max) {
    fprintf(stderr,
            "value zpos (%d) must be in range: [%d:%d]",
            value,
            plane->zpos_min,
            value > plane->zpos_max);
    ret = -EINVAL;
  } else {
    if (plane->plane_info.zpos != value) {
      ret = set_plane_property_by_name(plane, "zpos", value);
      if (ret == 0) {
        plane->plane_info.zpos = value;
      }
    }
  }

  return ret;
}

void
free_plane(plane_s* plane)
{
#ifdef DEBUG
  fprintf(stderr, "free_plane:%d\n", plane->plane_id);
#endif
  set_plane_property_zpos(plane, 0);
  free_plane_info(&plane->plane_info);
}

static int
alloc_plane(plane_s* plane,
            uint32_t fourcc_format,
            uint16_t width,
            uint16_t height,
            uint16_t x,
            uint16_t y,
            uint8_t count_bufs,
            uint8_t fullscreen)
{
#ifdef DEBUG
  fprintf(stderr, "alloc_plane\n");
#endif
  if (plane->plane_info.buf_info[0].map_planes[0]) {
    fprintf(stderr, "plane %d in use\n", plane->plane_id);
    return -EBUSY;
  }

  int crtc_w, crtc_h;

  /* just use single plane format for now.. */
  int ret = bo_fb_create(
    dev->fd, &plane->plane_info, width, height, fourcc_format, count_bufs);
  if (ret == 0) {
    if (width == 0) {
      width = dev->mode_info.hdisplay;
    }

    if (height == 0) {
      height = dev->mode_info.vdisplay;
    }

    if (fullscreen) {
      crtc_w = dev->mode_info.hdisplay;
      crtc_h = dev->mode_info.vdisplay;
    } else {
      crtc_w = width;
      crtc_h = height;
    }
#ifdef DEBUG
    printf("fd:%d, plane_id:%d, dev->crtc_id:%d, count_bufs:%d\n",
           dev->fd,
           plane->plane_id,
           dev->crtc_id,
           count_bufs);
#endif
    /* note src coords (last 4 args) are in Q16 format */
    ret = drmModeSetPlane(dev->fd,
                          plane->plane_id,
                          dev->crtc_id,
                          plane->plane_info.buf_info[0].fbi.fb_id,
                          0,
                          x,
                          y,
                          crtc_w,
                          crtc_h,
                          0,
                          0,
                          width << 16,
                          height << 16);
    if (ret) {
      fprintf(stderr, "failed to enable plane: %s\n", strerror(errno));
      fprintf(stderr,
              "width:%d, height:%d, fourcc_format: 0x%X\n",
              width,
              height,
              fourcc_format);
      free_plane(plane);
    } else {
      plane->plane_info.x = x;
      plane->plane_info.y = y;
      plane->crtc_w = crtc_w;
      plane->crtc_h = crtc_h;
      plane->plane_info.fullscreen = fullscreen;
    }
  }
  return ret;
}

void
free_planes()
{
#ifdef DEBUG
  fprintf(stderr, "free_planes\n");
#endif
  if (dev->planes) {
    for (int i = 0; i < dev->count_planes; i++) {
      plane_s* plane = &dev->planes[i];
      free_plane(plane);
      if (plane->formats) {
        free(plane->formats);
        plane->formats = NULL;
      }
      if (plane->properties) {
        free(plane->properties);
        plane->properties = NULL;
      }
    }
    free(dev->planes);
    dev->planes = NULL;
  }
}

static int
set_plane_pos(plane_s* plane, int8_t buf_num, uint16_t x, uint16_t y)
{
  int ret = 0;
  if (buf_num < 0) {
    buf_num = plane->plane_info.exposed_buf;
  }
  /* note src coords (last 4 args) are in Q16 format */
  ret = drmModeSetPlane(dev->fd,
                        plane->plane_id,
                        dev->crtc_id,
                        plane->plane_info.buf_info[buf_num].fbi.fb_id,
                        0,
                        x,
                        y,
                        plane->crtc_w,
                        plane->crtc_h,
                        0,
                        0,
                        plane->plane_info.buf_info[0].fbi.width << 16,
                        plane->plane_info.buf_info[0].fbi.height << 16);

  plane->plane_info.x = x;
  plane->plane_info.y = y;
  return ret;
}

static inline int
updateBufIds(DhPlaneInfo* plane_info)
{
  if (plane_info->count_bufs > 1) {
    uint8_t next_exp_buf = plane_info->exposed_buf + 1;
    if (next_exp_buf >= plane_info->count_bufs) {
      next_exp_buf = 0;
    }
    if (plane_info->buf_info[next_exp_buf].buf_ready == 0) {
      return -EBUSY;
    }
    if (plane_info->count_bufs > 2) {
      uint8_t next_back_buf = next_exp_buf + 1;
      if (next_back_buf > 2) {
        next_back_buf = 0;
      }
      plane_info->back_buf = next_back_buf;
      // plane_info->buf_info[next_back_buf].buf_ready = 0;
    }

    plane_info->opened_buf = plane_info->exposed_buf;
    plane_info->exposed_buf = next_exp_buf;

  } else {
    plane_info->exposed_buf = 0;
    plane_info->opened_buf = 0;
  }
  return 0;
}

void
page_flip_handler(int fd,
                  unsigned int frame,
                  unsigned int sec,
                  unsigned int usec,
                  void* data)
{
  unsigned char* waiting = (unsigned char*)data;
  *waiting = 0;
}

// not work for overlays
int
flip_primary(DhPlaneInfo* plane_info, uint8_t sync)
{
  unsigned char waiting = 1;
  int ret =
    drmModePageFlip(dev->fd,
                    dev->crtc_id,
                    plane_info->buf_info[plane_info->exposed_buf].fbi.fb_id,
                    DRM_MODE_PAGE_FLIP_EVENT,
                    &waiting);
  if (ret < 0) {
    fprintf(stderr, "Failed to enqueue page flip: %d\n", ret);
    return ret;
  }

  if (sync) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(dev->fd, &fds);
    drmEventContext evCtx;
    evCtx.page_flip_handler2 = NULL;
    evCtx.sequence_handler = NULL;
    evCtx.vblank_handler = NULL;
    evCtx.version = DRM_EVENT_CONTEXT_VERSION;
    evCtx.page_flip_handler = page_flip_handler;

    while (waiting) {
      struct timeval timeout = { .tv_sec = 3, .tv_usec = 0 };
      ret = select(dev->fd + 1, &fds, NULL, NULL, &timeout);
      if (ret < 0) {
        // Most of the time, select() will return an error because the
        // user pressed Ctrl-C.  So, only print out a message in debug
        // mode, and just check for the likely condition and release
        // the current buffer object before getting out.
        fprintf(stderr, "select timed out or error (ret %d)\n", ret);
        return ret;
      }
      drmHandleEvent(dev->fd, &evCtx);
    }
  }

  return 0;
}

//***************************Extern*************************
int
DrmHelperWorkGetBufId(DhPlaneInfo* plane_info)
{
  if (plane_info) {
    if (plane_info->count_bufs == 3 &&
        plane_info->buf_info[plane_info->back_buf].buf_ready == 0) {
      return plane_info->back_buf;
    }
    if (plane_info->buf_info[plane_info->opened_buf].buf_ready == 0)
      return plane_info->opened_buf;
  }
  return -1;
}

int
DrmHelperFlip(DhPlaneInfo* plane_info)
{
  int ret = -EINVAL;
  if (plane_info->plane_uid <= dev->count_planes) {
    ret = updateBufIds(plane_info);
    if (ret == 0) {
      /*
fprintf(
stderr,
"plane_uid:%d   opened_buf:%d back_buf:%d exposed_buf:%d  ret: %d\n",
plane_info->plane_uid, plane_info->opened_buf, plane_info->back_buf,
plane_info->exposed_buf, ret);
*/
      if (plane_info->plane_uid == 0) {
        ret = flip_primary(plane_info, 1);
      } else {
        ret = set_plane_pos(&dev->planes[plane_info->plane_uid - 1],
                            -1,
                            plane_info->x,
                            plane_info->y);
      }
      plane_info->buf_info[plane_info->opened_buf].buf_ready = 0;
    }
  }

  return ret;
}

int
DrmHelperFlipConfirm(DhPlaneInfo* plane_info)
{
  plane_info->buf_info[plane_info->opened_buf].buf_ready = 1;
  return DrmHelperFlip(plane_info);
}

int
DrmHelperSetZpos(DhPlaneInfo* plane_info, uint8_t value)
{
  int ret = -EINVAL;
  if (plane_info->plane_uid > 0 && plane_info->plane_uid <= dev->count_planes) {
    plane_s* plane = &dev->planes[plane_info->plane_uid - 1];
    ret = set_plane_property_zpos(plane, value);
    if (ret == 0) {
      set_plane_pos(plane, -1, plane_info->x, plane_info->y);
    }
  } else {
    fprintf(
      stderr, "DrmHelperSetZpos: Invalid plane: %d\n", plane_info->plane_uid);
  }
  return ret;
}

int
DrmHelperSetPlanePos(DhPlaneInfo* plane_info, uint16_t x, uint16_t y)
{
  int ret = -EINVAL;
  if (plane_info->plane_uid > 0 && plane_info->plane_uid <= dev->count_planes) {
    if (x >= dev->mode_info.hdisplay) {
      x = dev->mode_info.hdisplay - 1;
    }
    if (y >= dev->mode_info.vdisplay) {
      y = dev->mode_info.vdisplay - 1;
    }
    ret = set_plane_pos(&dev->planes[plane_info->plane_uid - 1], -1, x, y);
  } else {
    fprintf(stderr,
            "DrmHelperSetPlanePos: Invalid plane: %d\n",
            plane_info->plane_uid);
  }
  return ret;
}

DhPlaneInfo*
DrmHelperAllocFb(DhPlaneType type,
                 uint32_t fourcc_format,
                 uint16_t width,
                 uint16_t height,
                 uint16_t x,
                 uint16_t y,
                 uint8_t count_bufs,
                 uint8_t fullscreen)
{
#ifdef DEBUG
  fprintf(stderr, "DrmHelperAllocFb\n");
#endif
  DhPlaneInfo* dhpi = NULL;
  if (count_bufs > 0 && count_bufs <= 3) {
    switch (type) {
      case dh_plane_type_cursor:
      case dh_plane_type_overlay:
        for (int i = 0; i < dev->count_planes; i++) {
          plane_s* plane = &dev->planes[i];
          if (plane->plane_info.type == type &&
              plane->plane_info.buf_info[0].map_planes[0] == NULL) {
            int rt = alloc_plane(plane,
                                 fourcc_format,
                                 width,
                                 height,
                                 x,
                                 y,
                                 count_bufs,
                                 fullscreen);
            if (rt == 0) {
              dhpi = &plane->plane_info;
              break;
            }
          }
        }
        break;
      case dh_plane_type_primary: {
        int rt =
          alloc_primary_fb(fourcc_format, width, height, x, y, count_bufs);
        if (rt == 0) {
          dhpi = &dev->primary_fb;
        }
      } break;
      default:
        fprintf(stderr, "Invalid fb type: %d\n", type);
    }
  } else {
    fprintf(stderr, "Invalid count_bufs: %d\n", count_bufs);
  }
  if (!dhpi) {
    fprintf(stderr,
            "Error alloc FB, type: %d, fourcc: %d, w: %d, h: %d, x: %d, y: %d, "
            "bufs: %d, fs: %d\n",
            type,
            fourcc_format,
            width,
            height,
            x,
            y,
            count_bufs,
            fullscreen);
  }

  return dhpi;
}

int
DrmHelperFreeFb(DhPlaneInfo* plane_info)
{
  int ret = 0;
  if (plane_info->plane_uid == 0) { // primary_fb
    free_primary_fb();
  } else if (plane_info->plane_uid > 0 &&
             plane_info->plane_uid <= dev->count_planes) {
    free_plane(&dev->planes[plane_info->plane_uid - 1]);
  } else {
    fprintf(
      stderr, "DrmHelperFreeFb: Invalid plane: %d\n", plane_info->plane_uid);
    ret = -EINVAL;
  }
  return ret;
}

void
DrmHelperFree(void)
{
  fprintf(stderr, "DrmHelperFree\n");
  if (dev) {
    free_primary_fb();
    free_planes();

    if (dev->fd >= 0) {
      close(dev->fd);
      dev->fd = -1;
    }
    free(dev);
    dev = NULL;
  }
}
//
DhHwInfo*
DrmHelperInit(int drm_id)
{
  if (dev) {
    fprintf(stderr, "DrmHelper already in use\n");
    return NULL;
  }

  dev = calloc(1, sizeof(dev_s));
  if (!dev) {
    fprintf(stderr, "Error memory alloc for dev\n");
    return NULL;
  }
  dev->fd = -1;

  char dp_name[32];
  snprintf(dp_name, sizeof(dp_name), "/dev/dri/card%d", drm_id);

  dev->fd = drm_open(dp_name);
  if (dev->fd >= 0) {
    int ret = drm_find_crt();
    if (ret == 0) {
      ret = parse_planes(dev);
      if (ret == 0) {
        return &dev->dh_hw_info;
      } else {
        fprintf(stderr, "Error parse_planes\n");
      }
    } else {
      fprintf(stderr, "Error drm_find_crt\n");
    }
  } else {
    fprintf(stderr, "Error drm_open\n");
  }
  DrmHelperFree();
  return NULL;
}

#ifdef CONSTRUCT
__attribute__((constructor)) void
construct()
{
  // printf("construct\n");
  dev = NULL;
}

__attribute__((destructor)) void
destruct()
{
  DrmHelperFree();
}
#endif