--- a/src/video_scale.c	2023-05-07 13:42:09.892251084 +0300
+++ b/src/video_scale.c	2023-05-07 13:43:29.373185647 +0300
@@ -42,15 +42,18 @@
 
 uint scaler;
 
+#define HW_W 800
+#define HW_H 480
+
 const struct Scalers scalers[] =
 {
 #if defined(TARGET_GP2X) || defined(TARGET_DINGUX)
 	{ 320,           240,            no_scale, nn_16,      nn_32,      "None" },
 #else
-	{ 1 * vga_width, 1 * vga_height, no_scale, nn_16,      nn_32,      "None" },
-	{ 2 * vga_width, 2 * vga_height, NULL,     nn_16,      nn_32,      "2x" },
-	{ 2 * vga_width, 2 * vga_height, NULL,     scale2x_16, scale2x_32, "Scale2x" },
-	{ 2 * vga_width, 2 * vga_height, NULL,     NULL,       hq2x_32,    "hq2x" },
+	{ HW_W, HW_H, no_scale, nn_16,      nn_32,      "None" },
+	{ HW_W, HW_H, NULL,     nn_16,      nn_32,      "2x" },
+	{ HW_W, HW_H, NULL,     scale2x_16, scale2x_32, "Scale2x" },
+	{ HW_W, HW_H, NULL,     NULL,       hq2x_32,    "hq2x" },
 	{ 3 * vga_width, 3 * vga_height, NULL,     nn_16,      nn_32,      "3x" },
 	{ 3 * vga_width, 3 * vga_height, NULL,     scale3x_16, scale3x_32, "Scale3x" },
 	{ 3 * vga_width, 3 * vga_height, NULL,     NULL,       hq3x_32,    "hq3x" },
