--- a/video.c	2022-10-23 11:49:38.000000000 +0300
+++ b/video.c	2023-05-30 01:37:36.575261005 +0300
@@ -20,6 +20,7 @@
 #include "common.h"
 #define WANT_FONT_BITS
 #include "font.h"
+#include <drm_helper.h>
 
 #ifdef PSP_BUILD
 
@@ -3301,6 +3302,8 @@
     current_scanline_ptr += pitch;                            \
   }
 
+int DRM_id=-1;
+dh_fb_info_s fb_info;
 SDL_Surface *rl_screen;
 #define Average(A, B) ((((A)&0xF7DE) >> 1) + (((B)&0xF7DE) >> 1) + ((A) & (B)&0x0821))
 
@@ -3632,6 +3635,25 @@
 #define GBA_SCREEN_WIDTH 240
 #define GBA_SCREEN_HEIGHT 160
 
+
+void flip_screen_hw()
+{
+  //printf("w:%d  h:%d \n", rl_screen->w, rl_screen->h);
+  uint8_t *fb_map = fb_info.map_bufs[0];
+  uint8_t *psrc = rl_screen->pixels;
+  for (int i = 0; i < fb_info.fbi.height; i++)
+  {
+    uint8_t *lpsrc = psrc;
+    for (int j = 0; j < fb_info.fbi.width/2; j++)// 16 * 2 -> 32bit
+    {
+	*(uint32_t *)fb_map = *(uint32_t *)lpsrc;
+	fb_map +=4;
+	lpsrc +=4;
+    }
+    psrc += rl_screen->pitch;
+  }
+}
+
 void flip_screen()
 {
   SDL_Rect srect, drect;
@@ -3705,7 +3727,10 @@
   {
     SDL_BlitSurface(screen, NULL, rl_screen, NULL);
   }
-  SDL_Flip(rl_screen);
+  
+   // SDL_Flip(rl_screen);
+  flip_screen_hw();
+
 }
 
 #endif
@@ -3817,21 +3842,30 @@
 {
   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE);
 
-#ifdef GP2X_BUILD
-  SDL_GP2X_AllowGfxMemory(NULL, 0);
+  if (DrmHelperInit(0))
+  {
+	printf("error DrmHelperInit\n");
+	return;
+  }
 
-  hw_screen = SDL_SetVideoMode(320 * video_scale, 240 * video_scale,
-                               16, SDL_HWSURFACE);
 
-  screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 240 * video_scale,
-                                160 * video_scale, 16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
+  
 
-  warm_change_cb_upper(WCB_C_BIT | WCB_B_BIT, 1);
-#else
-  rl_screen = SDL_SetVideoMode(320 * video_scale, 240 * video_scale, 16, SDL_HWSURFACE);
+//  printf("Init 1\n");
+
+  /*rl_screen = */SDL_SetVideoMode(320 * video_scale, 240 * video_scale, 16, SDL_HWSURFACE);
+    DRM_id = DrmHelperAllocFb(plane_type_overlay, DRM_FORMAT_RGB565, 320 * video_scale, 240 * video_scale, 0, 0, 1, &fb_info);
+  if (DRM_id < 0)
+  {
+	printf("DrmHelperAllocFb ret:%d\n", DRM_id);
+	return;
+  }
+  
+  DrmHelperSetZpos(DRM_id, 1);
+  
+  rl_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320 * video_scale, 240 * video_scale, 16, 0, 0, 0, 0);
   screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 240 * video_scale, 160 * video_scale, 16, 0, 0, 0, 0);
-  // screen = SDL_SetVideoMode(240 * video_scale, 160 * video_scale, 16, 0);
-#endif
+
   SDL_ShowCursor(0);
 }
 
@@ -4046,65 +4080,35 @@
 
 void video_resolution_large()
 {
-  current_scale = unscaled;
+current_scale = unscaled;
 
-#ifdef GP2X_BUILD
-  SDL_FreeSurface(screen);
-  SDL_GP2X_AllowGfxMemory(NULL, 0);
-  hw_screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE);
-  screen = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0xFFFF,
-                                0xFFFF, 0xFFFF, 0);
+  
+//  printf("Init 2\n");
+  
   resolution_width = 320;
   resolution_height = 240;
-  SDL_ShowCursor(0);
+  //rl_screen = SDL_SetVideoMode(resolution_width * video_scale, resolution_height * video_scale, 16, SDL_HWSURFACE);
+  if(screen)
+	SDL_FreeSurface(screen);
+  screen = SDL_CreateRGBSurface(SDL_SWSURFACE, resolution_width * video_scale, resolution_height * video_scale, 16, 0, 0, 0, 0);
+
 
-  warm_change_cb_upper(WCB_C_BIT | WCB_B_BIT, 1);
-#else
   resolution_width = 320;
   resolution_height = 240;
-  rl_screen = SDL_SetVideoMode(resolution_width * video_scale, resolution_height * video_scale, 16, SDL_HWSURFACE);
-  screen = SDL_CreateRGBSurface(SDL_SWSURFACE, resolution_width * video_scale, resolution_height * video_scale, 16, 0, 0, 0, 0);
-  /*screen = SDL_SetVideoMode(320, 240, 16, 0);*/
-
-#endif
 }
 
 void video_resolution_small()
 {
   current_scale = screen_scale;
 
-#ifdef GP2X_BUILD
-  int w, h;
-  SDL_FreeSurface(screen);
-  SDL_GP2X_AllowGfxMemory(NULL, 0);
-
-  w = 320;
-  h = 240;
-  if (screen_scale == scaled_aspect || screen_scale == fullscreen)
-  {
-    w = small_resolution_width * video_scale;
-    h = small_resolution_height * video_scale;
-  }
-  if (screen_scale == scaled_aspect)
-    h += 20;
-  hw_screen = SDL_SetVideoMode(w, h, 16, SDL_HWSURFACE);
-
-  w = small_resolution_width * video_scale;
-  if (screen_scale == scaled_aspect_sw)
-    w = 320;
-  screen = SDL_CreateRGBSurface(SDL_HWSURFACE,
-                                w, small_resolution_height * video_scale,
-                                16, 0xFFFF, 0xFFFF, 0xFFFF, 0);
+//  printf("Init 3\n");
+//  rl_screen = SDL_SetVideoMode(320 * video_scale, 240 * video_scale, 16, SDL_HWSURFACE);
+  if(screen)
+	SDL_FreeSurface(screen);
+  screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320 * video_scale, 240 * video_scale, 16, 0, 0, 0, 0);
+
 
-  SDL_ShowCursor(0);
 
-  warm_change_cb_upper(WCB_C_BIT | WCB_B_BIT, 1);
-#else
-  rl_screen = SDL_SetVideoMode(320 * video_scale, 240 * video_scale, 16, SDL_HWSURFACE);
-  screen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320 * video_scale, 240 * video_scale, 16, 0, 0, 0, 0);
-  /*screen = SDL_SetVideoMode(small_resolution_width * video_scale,
-   small_resolution_height * video_scale, 16, 0);*/
-#endif
   resolution_width = small_resolution_width;
   resolution_height = small_resolution_height;
 
@@ -4127,6 +4131,7 @@
   default:
     break;
   }
+
 }
 
 void set_gba_resolution(video_scale_type scale)
