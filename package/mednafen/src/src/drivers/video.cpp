/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "main.h"

#ifdef WIN32
#include <SDL_syswm.h>
#include <mednafen/win32-common.h>
#else
#include "icon.h"
#endif

#include <trio/trio.h>

#include "video.h"
#ifdef OPENGL
#include "opengl.h"
#include "shader.h"
#endif
#include "nongl.h"

#include "netplay.h"
#include "cheat.h"

#include "nnx.h"
#include "debugger.h"
#include "fps.h"
#include "help.h"
#include "video-state.h"

#ifdef WANT_FANCY_SCALERS
#include "scalebit.h"
#include "hqxx-common.h"
#include "2xSaI.h"
#endif

class SDL_to_MDFN_Surface_Wrapper : public MDFN_Surface {
    public:
	INLINE SDL_to_MDFN_Surface_Wrapper(SDL_Surface *sdl_surface) : ss(sdl_surface)
	{
		// Locking should be first thing.
		if (SDL_MUSTLOCK(ss))
			SDL_LockSurface(ss);

		format = MDFN_PixelFormat(MDFN_COLORSPACE_RGB, ss->format->BitsPerPixel >> 3, ss->format->Rshift,
					  ss->format->Gshift, ss->format->Bshift, ss->format->Ashift, 8 - ss->format->Rloss,
					  8 - ss->format->Gloss, 8 - ss->format->Bloss, 8 - ss->format->Aloss);

		pixels_is_external = true;

		pixels16 = NULL;
		pixels = NULL;

		if (ss->format->BitsPerPixel == 16) {
			pixels16 = (uint16 *)ss->pixels;
			pitchinpix = ss->pitch >> 1;
		} else {
			pixels = (uint32 *)ss->pixels;
			pitchinpix = ss->pitch >> 2;
		}

		w = ss->w;
		h = ss->h;
	}

	INLINE ~SDL_to_MDFN_Surface_Wrapper()
	{
		if (SDL_MUSTLOCK(ss))
			SDL_UnlockSurface(ss);
		ss = NULL;
	}

    private:
	SDL_Surface *ss;
};

enum { VDRIVER_OPENGL = 0, VDRIVER_SOFTSDL = 1, VDRIVER__COUNT };

enum {
	NTVB_NONE = 0,

	NTVB_HQ2X,
	NTVB_HQ3X,
	NTVB_HQ4X,

	NTVB_SCALE2X,
	NTVB_SCALE3X,
	NTVB_SCALE4X,

	NTVB_NN2X,
	NTVB_NN3X,
	NTVB_NN4X,

	NTVB_NNY2X,
	NTVB_NNY3X,
	NTVB_NNY4X,

	NTVB_2XSAI,
	NTVB_SUPER2XSAI,
	NTVB_SUPEREAGLE,
};

static const MDFNSetting_EnumList VDriver_List[] = {
	{ "default", VDRIVER__COUNT, gettext_noop("Default"),
	  gettext_noop(
		  "Selects the default video driver.  Currently, this is OpenGL for all platforms, but may change in the future if better platform-specific drivers are added.") },

	{ "opengl", VDRIVER_OPENGL, "OpenGL",
	  gettext_noop("All video-related Mednafen features are available with this video driver.") },
	{ "softfb", VDRIVER_SOFTSDL, "Software Blitting to Framebuffer",
	  gettext_noop(
		  "Slower with lower-quality scaling than OpenGL, but if you don't have hardware-accelerated OpenGL rendering, it will probably be faster than software OpenGL rendering. Bilinear interpolation not available. OpenGL shaders do not work with this output method, of course.") },

	// Backwards-compat:
	{ "0", VDRIVER_OPENGL },
	{ "1", VDRIVER_SOFTSDL },
	{ "sdl", VDRIVER_SOFTSDL },
	{ "overlay", VDRIVER__COUNT },

	{ NULL, 0 },
};

enum { CURSORVIS_HIDDEN = 0, CURSORVIS_VISIBLE };

static const MDFNSetting_EnumList CursorVis_List[] = { { "hidden", CURSORVIS_HIDDEN, gettext_noop("Hidden") },
						       { "visible", CURSORVIS_VISIBLE, gettext_noop("Visible") },

						       { "0", CURSORVIS_HIDDEN },
						       { "1", CURSORVIS_VISIBLE },

						       { NULL, 0 } };

static const MDFNSetting_EnumList GLFormat_List[] = {
	{ "auto", EVFSUPPORT_NONE, gettext_noop("Auto"),
	  gettext_noop("Currently the same as \"truecolor\", but may automatically select deeper color formats in the future.") },
	//{ "deepcolor", EVFSUPPORT_RGB101010, gettext_noop("Deepcolor, 1B colors"), gettext_noop("RGB, 10 bits per color component.") },
	{ "truecolor", EVFSUPPORT_NONE, gettext_noop("Truecolor, 16M colors"), gettext_noop("RGB, 8 bits per color component.") },
	{ "hicolor", EVFSUPPORT_RGB555 | EVFSUPPORT_RGB565, gettext_noop("Hicolor, 32K/64K colors"),
	  gettext_noop("RGB565 or RGB555, with priority given to RGB565.") },

	{ "rgb565", EVFSUPPORT_RGB565, gettext_noop("RGB565, 64K colors") },
	{ "rgb555", EVFSUPPORT_RGB555, gettext_noop("RGB555, 32K colors") },

	{ NULL, 0 },
};

static const MDFNSetting GlobalVideoSettings[] = {
	{ "video.driver", MDFNSF_NOFLAGS, gettext_noop("Video output driver."), NULL, MDFNST_ENUM, "default", NULL, NULL, NULL,
	  NULL, VDriver_List },

	{
		"video.fs",
		MDFNSF_NOFLAGS,
		gettext_noop("Enable fullscreen mode."),
		NULL,
		MDFNST_BOOL,
		"0",
	},
	{ "video.fs.display", MDFNSF_NOFLAGS, gettext_noop("Display to use with fullscreen mode."),
	  gettext_noop("Specify -1 to use the display on which the center of the window lies in windowed mode."), MDFNST_INT, "-1",
	  "-1", "32767" },

	//{ "video.window.x", MDFNSF_NOFLAGS, gettext_noop("Window starting X position."), NULL, MDFNST_INT, "0x2FFF0000" },
	//{ "video.window.y", MDFNSF_NOFLAGS, gettext_noop("Window starting Y position."), NULL, MDFNST_INT, "0x2FFF0000" },

	{ "video.cursorvis", MDFNSF_NOFLAGS, gettext_noop("Preferred window manager cursor visibility."),
	  gettext_noop(
		  "The cursor will still be forcibly hidden in relative mouse mode(used automatically when emulating a mouse input device in fullscreen mode or in windowed mode and input grabbing is toggled on), and forcibly shown in the debugger."),
	  MDFNST_ENUM, "hidden", NULL, NULL, NULL, NULL, CursorVis_List },

	{ "video.glformat", MDFNSF_NOFLAGS, gettext_noop("Preferred source data pixel format for emulated video."),
	  gettext_noop(
		  "Using hicolor(RGB555/RGB565) formats may boost performance, but at the cost of color fidelity.  Noticeability of color degradation depends on the emulated system and features, such as custom palettes or NTSC blitter, being used.  When hicolor format support is not available for the emulation module or special scaler being used, Mednafen will automatically fall back to using a truecolor format.  Examine Mednafen's startup output to see the actual OpenGL texture formats being used."),
	  MDFNST_ENUM, "auto", NULL, NULL, NULL, NULL, GLFormat_List },

	{ "video.force_bbclear", MDFNSF_NOFLAGS, gettext_noop("Force backbuffer clear before drawing."),
	  gettext_noop(
		  "Enabling may result in a noticeable negative impact on performance with the \"softfb\" video driver, and with the \"opengl\" video driver on underpowered GPUs."),
	  MDFNST_BOOL, "0" },

	{ "video.glvsync", MDFNSF_NOFLAGS, gettext_noop("Attempt to synchronize OpenGL page flips to vertical retrace period."),
	  gettext_noop(
		  "Note: Additionally, if the environment variable \"__GL_SYNC_TO_VBLANK\" does not exist, then it will be created and set to the value specified for this setting.  This has the effect of forcibly enabling or disabling vblank synchronization when running under Linux with NVidia's drivers."),
	  MDFNST_BOOL, "1" },

	{ "video.disable_composition", MDFNSF_NOFLAGS, gettext_noop("Attempt to disable desktop composition."),
	  gettext_noop(
		  "Currently, this setting only has an effect on Windows Vista and Windows 7(and probably the equivalent server versions as well)."),
	  MDFNST_BOOL, "1" },

	{ NULL }
};

static const MDFNSetting_EnumList StretchMode_List[] = {
	{ "0", 0, gettext_noop("Disabled") },
	{ "off", 0 },
	{ "none", 0 },

	{ "1", 1 },
	{ "full", 1, gettext_noop("Full"), gettext_noop("Full-screen stretch, disregarding aspect ratio.") },

	{ "2", 2 },
	{ "aspect", 2, gettext_noop("Aspect Preserve"),
	  gettext_noop(
		  "Full-screen stretch as far as the aspect ratio(in this sense, the equivalent xscalefs == yscalefs) can be maintained.") },

	{ "aspect_int", 3, gettext_noop("Aspect Preserve + Integer Scale"),
	  gettext_noop(
		  "Full-screen stretch, same as \"aspect\" except that the equivalent xscalefs and yscalefs are rounded down to the nearest integer.") },
	{ "aspect_mult2", 4, gettext_noop("Aspect Preserve + Integer Multiple-of-2 Scale"),
	  gettext_noop("Full-screen stretch, same as \"aspect_int\", but rounds down to the nearest multiple of 2.") },

	{ NULL, 0 },
};

static const MDFNSetting_EnumList VideoIP_List[] = {
	{ "0", VIDEOIP_OFF, gettext_noop("Disabled") },

	{ "1", VIDEOIP_BILINEAR, gettext_noop("Bilinear") },

	// Disabled until a fix can be made for rotation.
	{ "x", VIDEOIP_LINEAR_X, gettext_noop("Linear (X)"), gettext_noop("Interpolation only on the X axis.") },
	{ "y", VIDEOIP_LINEAR_Y, gettext_noop("Linear (Y)"), gettext_noop("Interpolation only on the Y axis.") },

	{ NULL, 0 },
};

static const MDFNSetting_EnumList Special_List[] = {
	{ "0", NTVB_NONE },
	{ "none", NTVB_NONE, "None/Disabled" },

#ifdef WANT_FANCY_SCALERS
	{ "hq2x", NTVB_HQ2X, "hq2x" },
	{ "hq3x", NTVB_HQ3X, "hq3x" },
	{ "hq4x", NTVB_HQ4X, "hq4x" },
	{ "scale2x", NTVB_SCALE2X, "scale2x" },
	{ "scale3x", NTVB_SCALE3X, "scale3x" },
	{ "scale4x", NTVB_SCALE4X, "scale4x" },

	{ "2xsai", NTVB_2XSAI, "2xSaI" },
	{ "super2xsai", NTVB_SUPER2XSAI, "Super 2xSaI" },
	{ "supereagle", NTVB_SUPEREAGLE, "Super Eagle" },
#endif

	{ "nn2x", NTVB_NN2X, "Nearest-neighbor 2x" },
	{ "nn3x", NTVB_NN3X, "Nearest-neighbor 3x" },
	{ "nn4x", NTVB_NN4X, "Nearest-neighbor 4x" },
	{ "nny2x", NTVB_NNY2X, "Nearest-neighbor 2x, y axis only" },
	{ "nny3x", NTVB_NNY3X, "Nearest-neighbor 3x, y axis only" },
	{ "nny4x", NTVB_NNY4X, "Nearest-neighbor 4x, y axis only" },

	{ NULL, 0 },
};

#ifdef OPENGL
static const MDFNSetting_EnumList Shader_List[] = {
	{ "none", SHADER_NONE, gettext_noop("None/Disabled") },
	{ "autoip", SHADER_AUTOIP, gettext_noop("Auto Interpolation"),
	  gettext_noop(
		  "Will automatically interpolate on each axis if the corresponding effective scaling factor is not an integer.") },
	{ "autoipsharper", SHADER_AUTOIPSHARPER, gettext_noop("Sharper Auto Interpolation"),
	  gettext_noop(
		  "Same as \"autoip\", but when interpolation is done, it is done in a manner that will reduce blurriness if possible.") },
	{ "scale2x", SHADER_SCALE2X, "Scale2x" },
	{ "sabr", SHADER_SABR, "SABR v3.0", gettext_noop("GPU-intensive.") },
	{ "ipsharper", SHADER_IPSHARPER, gettext_noop("Sharper bilinear interpolation.") },
	{ "ipxnoty", SHADER_IPXNOTY, gettext_noop("Linear interpolation on X axis only.") },
	{ "ipynotx", SHADER_IPYNOTX, gettext_noop("Linear interpolation on Y axis only.") },
	{ "ipxnotysharper", SHADER_IPXNOTYSHARPER, gettext_noop("Sharper version of \"ipxnoty\".") },
	{ "ipynotxsharper", SHADER_IPYNOTXSHARPER, gettext_noop("Sharper version of \"ipynotx\".") },

	{ "goat", SHADER_GOAT, gettext_noop("Simple approximation of a color TV CRT look."),
	  gettext_noop(
		  "Intended for fullscreen modes with a vertical resolution of around 1000 to 1500 pixels.  Doesn't simulate halation and electron beam energy distribution nuances.") },

	{ NULL, 0 },
};

static const MDFNSetting_EnumList GoatPat_List[] = {
	{ "goatron", ShaderParams::GOAT_MASKPAT_GOATRON, gettext_noop("Goatron"), gettext_noop("Brightest.") },
	{ "goattron", ShaderParams::GOAT_MASKPAT_GOATRON },
	{ "goatronprime", ShaderParams::GOAT_MASKPAT_GOATRONPRIME },
	{ "goattronprime", ShaderParams::GOAT_MASKPAT_GOATRONPRIME },

	{ "borg", ShaderParams::GOAT_MASKPAT_BORG, gettext_noop("Borg"), gettext_noop("Darkest.") },
	{ "slenderman", ShaderParams::GOAT_MASKPAT_SLENDERMAN, gettext_noop("Slenderman"), gettext_noop("Spookiest?") },

	{ NULL, 0 },
};
#endif
void Video_MakeSettings(void)
{
	static const char *CSD_xres = gettext_noop("Full-screen horizontal resolution.");
	static const char *CSD_yres = gettext_noop("Full-screen vertical resolution.");
	static const char *CSDE_xres =
		gettext_noop("A value of \"0\" will cause the current desktop horizontal resolution to be used.");
	static const char *CSDE_yres =
		gettext_noop("A value of \"0\" will cause the current desktop vertical resolution to be used.");

	static const char *CSD_xscale = gettext_noop("Scaling factor for the X axis in windowed mode.");
	static const char *CSD_yscale = gettext_noop("Scaling factor for the Y axis in windowed mode.");

	static const char *CSD_xscalefs = gettext_noop("Scaling factor for the X axis in fullscreen mode.");
	static const char *CSD_yscalefs = gettext_noop("Scaling factor for the Y axis in fullscreen mode.");
	static const char *CSDE_xyscalefs =
		gettext_noop("For this setting to have any effect, the \"<system>.stretch\" setting must be set to \"0\".");

	static const char *CSD_scanlines = gettext_noop("Enable scanlines with specified opacity.");
	static const char *CSDE_scanlines = gettext_noop(
		"Opacity is specified in %; IE a value of \"100\" will give entirely black scanlines.\n\nNegative values are the same as positive values for non-interlaced video, but for interlaced video will cause the scanlines to be overlaid over the previous(if the video.deinterlacer setting is set to \"weave\", the default) field's lines.");

	static const char *CSD_stretch = gettext_noop("Stretch to fill screen.");
	static const char *CSDvideo_settingsip = gettext_noop("Enable (bi)linear interpolation.");

	static const char *CSD_special = gettext_noop("Enable specified special video scaler.");
	static const char *CSDE_special = gettext_noop(
		"The destination rectangle is NOT altered by this setting, so if you have xscale and yscale set to \"2\", and try to use a 3x scaling filter like hq3x, the image is not going to look that great. The nearest-neighbor scalers are intended for use with bilinear interpolation enabled, for a sharper image, though the \"autoipsharper\" shader may provide better results.");
#ifdef OPENGL
	static const char *CSD_shader = gettext_noop("Enable specified OpenGL shader.");
	static const char *CSDE_shader = gettext_noop(
		"Obviously, this will only work with the OpenGL \"video.driver\" setting, and only on cards and OpenGL implementations that support shaders, otherwise you will get a black screen, or Mednafen may display an error message when starting up. When a shader is enabled, the \"<system>.videoip\" setting is ignored.");
#endif
	for (unsigned int i = 0; i < MDFNSystems.size() + 1; i++) {
		int nominal_width;
		//int nominal_height;
		const char *default_videoip;
		const char *sysname;
		char default_value[256];
		const int default_xres = 0, default_yres = 0;
		const double default_scalefs = 1.0;
		double default_scale;

		if (i == MDFNSystems.size()) {
			nominal_width = 384;
			//nominal_height = 240;
			default_videoip = "0";
			sysname = "player";
		} else {
			const int mr = MDFNSystems[i]->multires;

			nominal_width = MDFNSystems[i]->nominal_width;
			//nominal_height = MDFNSystems[i]->nominal_height;

			if (mr < 0)
				default_videoip = "x";
			else if (!mr)
				default_videoip = "0";
			else
				default_videoip = "1";

			sysname = (const char *)MDFNSystems[i]->shortname;
		}

		default_scale = ceil(1024 / nominal_width);

		if (default_scale * nominal_width > 1024)
			default_scale--;

		if (!default_scale)
			default_scale = 1;

		trio_snprintf(default_value, 256, "%d", default_xres);
		AddSystemSetting(sysname, "xres", CSD_xres, CSDE_xres, MDFNST_UINT, strdup(default_value), "0", "65536", NULL, NULL,
				 NULL, MDFNSF_FREE_DEFAULT);

		trio_snprintf(default_value, 256, "%d", default_yres);
		AddSystemSetting(sysname, "yres", CSD_yres, CSDE_yres, MDFNST_UINT, strdup(default_value), "0", "65536", NULL, NULL,
				 NULL, MDFNSF_FREE_DEFAULT);

		trio_snprintf(default_value, 256, "%f", default_scale);
		AddSystemSetting(sysname, "xscale", CSD_xscale, NULL, MDFNST_FLOAT, strdup(default_value), "0.01", "256", NULL,
				 NULL, NULL, MDFNSF_FREE_DEFAULT);
		AddSystemSetting(sysname, "yscale", CSD_yscale, NULL, MDFNST_FLOAT, strdup(default_value), "0.01", "256", NULL,
				 NULL, NULL, MDFNSF_FREE_DEFAULT);

		trio_snprintf(default_value, 256, "%f", default_scalefs);
		AddSystemSetting(sysname, "xscalefs", CSD_xscalefs, CSDE_xyscalefs, MDFNST_FLOAT, strdup(default_value), "0.01",
				 "256", NULL, NULL, NULL, MDFNSF_FREE_DEFAULT);
		AddSystemSetting(sysname, "yscalefs", CSD_yscalefs, CSDE_xyscalefs, MDFNST_FLOAT, strdup(default_value), "0.01",
				 "256", NULL, NULL, NULL, MDFNSF_FREE_DEFAULT);

		AddSystemSetting(sysname, "scanlines", CSD_scanlines, CSDE_scanlines, MDFNST_INT, "0", "-100", "100");
		AddSystemSetting(sysname, "stretch", CSD_stretch, NULL, MDFNST_ENUM, "aspect_mult2", NULL, NULL, NULL, NULL,
				 StretchMode_List);
		AddSystemSetting(sysname, "videoip", CSDvideo_settingsip, NULL, MDFNST_ENUM, default_videoip, NULL, NULL, NULL,
				 NULL, VideoIP_List);
		AddSystemSetting(sysname, "special", CSD_special, CSDE_special, MDFNST_ENUM, "none", NULL, NULL, NULL, NULL,
				 Special_List);
#ifdef OPENGL
		AddSystemSetting(sysname, "shader", CSD_shader, CSDE_shader, MDFNST_ENUM, "none", NULL, NULL, NULL, NULL,
				 Shader_List);
		AddSystemSetting(sysname, "shader.goat.hdiv", gettext_noop("Constant RGB horizontal divergence."), nullptr,
				 MDFNST_FLOAT, "0.50", "-2.00", "2.00");
		AddSystemSetting(sysname, "shader.goat.vdiv", gettext_noop("Constant RGB vertical divergence."), nullptr,
				 MDFNST_FLOAT, "0.50", "-2.00", "2.00");
		AddSystemSetting(sysname, "shader.goat.pat", gettext_noop("Mask pattern."), nullptr, MDFNST_ENUM, "goatron", NULL,
				 NULL, NULL, NULL, GoatPat_List);
		AddSystemSetting(sysname, "shader.goat.tp", gettext_noop("Transparency of otherwise-opaque mask areas."), nullptr,
				 MDFNST_FLOAT, "0.50", "0.00", "1.00");
		AddSystemSetting(
			sysname, "shader.goat.fprog", gettext_noop("Force interlaced video to be treated as progressive."),
			gettext_noop(
				"When disabled, the default, the \"video.deinterlacer\" setting is effectively ignored with respect to what appears on the screen, unless it's set to \"blend\" or \"blend_rg\".  When enabled, it may be prudent to disable the scanlines effect controlled by the *.goat.slen setting, or else the scanline effect may look objectionable."),
			MDFNST_BOOL, "0");
		AddSystemSetting(sysname, "shader.goat.slen", gettext_noop("Enable scanlines effect."), nullptr, MDFNST_BOOL, "1");
#endif
	}

	MDFNI_MergeSettings(GlobalVideoSettings);
}

static struct {
	int fullscreen;
	int fs_display;
	int xres, yres;
	double xscale, xscalefs;
	double yscale, yscalefs;
	int videoip;
	int stretch;
	int special;
	int scanlines;
	bool force_bbclear;
#ifdef OPENGL
	ShaderType shader;
	ShaderParams shader_params;
#endif
	unsigned cursorvis;

	std::string special_str, shader_str, goat_pat_str;
} video_settings;

static unsigned vdriver;
static bool osd_alpha_blend;

static const struct ScalerDefinition {
	int id;
	int xscale;
	int yscale;
} Scalers[] = {
	{ NTVB_HQ2X, 2, 2 },	{ NTVB_HQ3X, 3, 3 },	   { NTVB_HQ4X, 4, 4 },

	{ NTVB_SCALE2X, 2, 2 }, { NTVB_SCALE3X, 3, 3 },	   { NTVB_SCALE4X, 4, 4 },

	{ NTVB_NN2X, 2, 2 },	{ NTVB_NN3X, 3, 3 },	   { NTVB_NN4X, 4, 4 },

	{ NTVB_NNY2X, 1, 2 },	{ NTVB_NNY3X, 1, 3 },	   { NTVB_NNY4X, 1, 4 },

	{ NTVB_2XSAI, 2, 2 },	{ NTVB_SUPER2XSAI, 2, 2 }, { NTVB_SUPEREAGLE, 2, 2 },
};

static MDFNGI *VideoGI;

static int winpos_x, winpos_y;
static bool winpos_applied;
//static SDL_Window *window = NULL;
#ifdef OPENGL
static SDL_GLContext glcontext = NULL;
static OpenGL_Blitter *ogl_blitter = NULL;
#endif
static SDL_Surface *screen = NULL;
//static SDL_Surface *IconSurface = NULL;

static int32 screen_w, screen_h;
static MDFN_Rect screen_dest_rect;

static MDFN_Surface *HelpSurface = NULL;
static MDFN_Rect HelpRect;

static MDFN_Surface *SMSurface = NULL;
static MDFN_Rect SMRect;
static MDFN_Rect SMDRect;

static double exs, eys;
static int evideoip;

static int NeedClear = 0;
static uint32 LastBBClearTime = 0;

static int rotated;

static MDFN_PixelFormat game_pf; // Pixel format for game texture/surface
static MDFN_PixelFormat osd_pf; // Pixel format for OSD textures/surfaces
static MDFN_PixelFormat emu_pf;

static INLINE void MarkNeedBBClear(void)
{
	NeedClear = 15;
}

static void SyncCleanup(void)
{
	if (SMSurface) {
		delete SMSurface;
		SMSurface = nullptr;
	}

	if (HelpSurface) {
		delete HelpSurface;
		HelpSurface = nullptr;
	}

	/*
*/
}

void Video_Kill(void)
{
	SyncCleanup();
	if (screen) {
		SDL_FreeSurface(screen);
	}
	/*
	if (window) {
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN)
			SDL_SetWindowFullscreen(window, 0);
		//
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_ShowCursor(SDL_TRUE);
		SDL_SetWindowGrab(window, SDL_FALSE);

		SDL_DestroyWindow(window);
		window = nullptr;
	}

	//
	//
	//
	if (IconSurface) {
		//SDL_FreeSurface(IconSurface);
		//IconSurface = nullptr;
	}
*/
	screen = nullptr;
	VideoGI = nullptr;
	screen_w = 0;
	screen_h = 0;
}

// May be called before Video_Init(), and after Video_Kill().
bool Video_ErrorPopup(bool warning, const char *title, const char *text)
{
	bool ret = false;

	return ret;
}

static void GenerateWindowedDestRect(void)
{
	exs = video_settings.xscale;
	eys = video_settings.yscale;

	screen_dest_rect.x = 0;
	screen_dest_rect.y = 0;
	screen_dest_rect.w = floor(0.5 + VideoGI->nominal_width * exs);
	screen_dest_rect.h = floor(0.5 + VideoGI->nominal_height * eys);

	if (rotated == MDFN_ROTATE90 || rotated == MDFN_ROTATE270)
		std::swap(screen_dest_rect.w, screen_dest_rect.h);
}

static bool GenerateFullscreenDestRect(void)
{
	if (video_settings.stretch) {
		int nom_width, nom_height;

		if (rotated) {
			nom_width = VideoGI->nominal_height;
			nom_height = VideoGI->nominal_width;
		} else {
			nom_width = VideoGI->nominal_width;
			nom_height = VideoGI->nominal_height;
		}

		if (video_settings.stretch == 2 || video_settings.stretch == 3 ||
		    video_settings.stretch == 4) // Aspect-preserve stretch
		{
			exs = (double)screen_w / nom_width;
			eys = (double)screen_h / nom_height;

			if (video_settings.stretch == 3 || video_settings.stretch == 4) // Round down to nearest int.
			{
				double floor_exs = floor(exs);
				double floor_eys = floor(eys);

				if (!floor_exs || !floor_eys) {
					MDFN_printf(_(
						"WARNING: Resolution is too low for stretch mode selected.  Falling back to \"aspect\" mode.\n"));
				} else {
					exs = floor_exs;
					eys = floor_eys;

					if (video_settings.stretch == 4) // Round down to nearest multiple of 2.
					{
						int even_exs = (int)exs & ~1;
						int even_eys = (int)eys & ~1;

						if (!even_exs || !even_eys) {
							MDFN_printf(_(
								"WARNING: Resolution is too low for stretch mode selected.  Falling back to \"aspect_int\" mode.\n"));
						} else {
							exs = even_exs;
							eys = even_eys;
						}
					}
				}
			}

			// Check if we are constrained horizontally or vertically
			if (exs > eys) {
				// Too tall for screen, fill vertically
				exs = eys;
			} else {
				// Too wide for screen, fill horizontally
				eys = exs;
			}

			//printf("%f %f\n", exs, eys);

			screen_dest_rect.w = floor(0.5 + exs * nom_width);
			screen_dest_rect.h = floor(0.5 + eys * nom_height);

			// Centering:
			int nx = (int)((screen_w - screen_dest_rect.w) / 2);
			if (nx < 0)
				nx = 0;
			screen_dest_rect.x = nx;

			int ny = (int)((screen_h - screen_dest_rect.h) / 2);
			if (ny < 0)
				ny = 0;
			screen_dest_rect.y = ny;
		} else // Full-stretch
		{
			screen_dest_rect.x = 0;
			screen_dest_rect.w = screen_w;

			screen_dest_rect.y = 0;
			screen_dest_rect.h = screen_h;

			exs = (double)screen_w / nom_width;
			eys = (double)screen_h / nom_height;
		}
	} else {
		exs = video_settings.xscalefs;
		eys = video_settings.yscalefs;

		screen_dest_rect.w = floor(0.5 + VideoGI->nominal_width * exs);
		screen_dest_rect.h = floor(0.5 + VideoGI->nominal_height * eys);

		if (rotated == MDFN_ROTATE90 || rotated == MDFN_ROTATE270)
			std::swap(screen_dest_rect.w, screen_dest_rect.h);

		screen_dest_rect.x = (screen_w - screen_dest_rect.w) / 2;
		screen_dest_rect.y = (screen_h - screen_dest_rect.h) / 2;
	}

	// Quick and dirty kludge for VB's "hli" and "vli" 3D modes.
	screen_dest_rect.x &= ~1;
	screen_dest_rect.y &= ~1;

	return screen_dest_rect.w < 16384 && screen_dest_rect.h < 16384;
}

static bool weset_glstvb = false;

void Video_Sync(MDFNGI *gi)
{
	MDFNI_printf(_("Initializing video...\n"));
	MDFN_AutoIndent aindv(1);
	/*
	if (window && winpos_applied && !(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN))
		SDL_GetWindowPosition(window, &winpos_x, &winpos_y);
    */
	winpos_x = 0;
	winpos_y = 0;
	//
	SyncCleanup();
	//Time::SleepMS(1000);

	VideoGI = gi;
	rotated = gi->rotated;
	//

	//
	osd_alpha_blend = MDFN_GetSettingB("osd.alpha_blend");

	std::string snp = std::string(gi->shortname) + ".";

	if (gi->GameType == GMT_PLAYER)
		snp = "player.";

	video_settings.xres = MDFN_GetSettingUI(snp + "xres");
	video_settings.yres = MDFN_GetSettingUI(snp + "yres");
	video_settings.xscale = 1; //MDFN_GetSettingF(snp + "xscale");
	video_settings.yscale = 1; // MDFN_GetSettingF(snp + "yscale");
	video_settings.xscalefs = 1; //MDFN_GetSettingF(snp + "xscalefs");
	video_settings.yscalefs = 1; //MDFN_GetSettingF(snp + "yscalefs");
	video_settings.videoip = MDFN_GetSettingI(snp + "videoip");
	video_settings.stretch = MDFN_GetSettingUI(snp + "stretch");
	video_settings.scanlines = MDFN_GetSettingI(snp + "scanlines");

	video_settings.special = MDFN_GetSettingUI(snp + "special");
	video_settings.special_str = MDFN_GetSettingS(snp + "special");

	video_settings.fullscreen = false; //MDFN_GetSettingB("video.fs");
	video_settings.fs_display = MDFN_GetSettingI("video.fs.display");
	vdriver = VDRIVER_SOFTSDL;
	video_settings.force_bbclear = MDFN_GetSettingB("video.force_bbclear");
	video_settings.cursorvis = MDFN_GetSettingUI("video.cursorvis");

	GenerateWindowedDestRect();
	screen_w = screen_dest_rect.w;
	screen_h = screen_dest_rect.h;
	//
	//

	if (screen_dest_rect.w > 16383 || screen_dest_rect.h > 16383)
		throw MDFN_Error(0, _("Window size(%dx%d) is too large!"), screen_dest_rect.w, screen_dest_rect.h);

	SDL_PumpEvents();
	//SDL_SetWindowSize(window, screen_dest_rect.w, screen_dest_rect.h);
	screen = SDL_SetVideoMode(screen_dest_rect.w, screen_dest_rect.h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL)
		throw MDFN_Error(0, _("Error SDL_SetVideoMode(%dx%d) is too large!"), screen_dest_rect.w, screen_dest_rect.h);
	winpos_applied = true;
	SDL_PumpEvents();

	evideoip = video_settings.videoip;

	//
	//
	//
	MDFN_printf(_("Driver: %s\n"), (vdriver == VDRIVER_OPENGL) ? _("OpenGL") : _("Software SDL"));
	//
	//
	//MDFN_PixelFormat pf;

	//screen = nullptr;

	{
		MDFN_printf(_("Display bpp:%d  (Window: %u x %u)\n"), screen->format->BitsPerPixel, screen_w, screen_h);

		if (vdriver != VDRIVER_OPENGL) {
			if (screen->format->BitsPerPixel != 32)
				throw MDFN_Error(0, _("Window surface bit depth(%ubpp) is not supported by Mednafen."),
						 screen->format->BitsPerPixel);

			int rs = screen->format->Rshift;
			int gs = screen->format->Gshift;
			int bs = screen->format->Bshift;

			int as = 0;
			while (as == rs || as == gs || as == bs) // Find unused 8-bits to use as our alpha channel
				as += 8;

			osd_pf = game_pf = emu_pf = MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 4, rs, gs, bs, as, 8, 8, 8, 8);
		}
	}

	MDFN_printf(_("Fullscreen: %s\n"), video_settings.fullscreen ? _("Yes") : _("No"));
	MDFN_printf(_("Special Scaler: %s\n"),
		    (video_settings.special == NTVB_NONE) ? _("None") : video_settings.special_str.c_str());

	if (!video_settings.scanlines)
		MDFN_printf(_("Scanlines: Off\n"));
	else
		MDFN_printf(_("Scanlines: %d%% opacity%s\n"), abs(video_settings.scanlines),
			    (video_settings.scanlines < 0) ? _(" (with interlace field obscure)") : "");

	MDFN_printf(_("Destination Rectangle: X=%d, Y=%d, W=%d, H=%d\n"), screen_dest_rect.x, screen_dest_rect.y,
		    screen_dest_rect.w, screen_dest_rect.h);
	if (screen_dest_rect.x < 0 || screen_dest_rect.y < 0 || (screen_dest_rect.x + screen_dest_rect.w) > screen_w ||
	    (screen_dest_rect.y + screen_dest_rect.h) > screen_h) {
		MDFN_AutoIndent ainddr;
		MDFN_printf(_(
			"Warning:  Destination rectangle exceeds screen dimensions.  This is ok if you really do want the clipping...\n"));
	}

	/* HQXX only supports this pixel format, sadly, and other pixel formats
    can't be easily supported without rewriting the filters.
    We do conversion to the real screen format in the blitting function.
 */

	{
		int xmu = std::max<int>(1, screen_w / 402);
		int ymu = std::max<int>(1, screen_h / 288);

		SMRect.h = 18 + 2;
		SMRect.x = 0;
		SMRect.y = 0;
		SMRect.w = screen_w;

		SMDRect.w = SMRect.w * xmu;
		SMDRect.h = SMRect.h * ymu;
		SMDRect.x = (screen_w - SMDRect.w) / 2;
		SMDRect.y = screen_h - SMDRect.h;

		if (SMDRect.x < 0) {
			SMRect.w += SMDRect.x * 2 / xmu;
			SMDRect.w = SMRect.w * xmu;
			SMDRect.x = 0;
		}
		SMSurface = new MDFN_Surface(NULL, SMRect.w, SMRect.h, SMRect.w, osd_pf);
	}

	SDL_FillRect(screen, NULL, 0);
	for (int i = 0; i < 2; i++)
		SDL_Flip(screen); //SDL_UpdateWindowSurface(window);

	MarkNeedBBClear();
	//
	//
	//
	SDL_ShowCursor(SDL_FALSE);
}

void Video_Init(void)
{
	winpos_x = 0; //MDFN_GetSettingI("video.window.x");
	winpos_y = 0; //MDFN_GetSettingI("video.window.y");
	winpos_applied = false;

	//
	/*
	for (unsigned i = 0; i < 2 && !window; i++) {
		static const uint32 try_flags[2] = { SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL, SDL_WINDOW_HIDDEN };
		window = SDL_CreateWindow("Mednafen", winpos_x, winpos_y, 64, 64, try_flags[i]);
	}

	if (!window)
		throw MDFN_Error(0, _("SDL_CreateWindow() failed: %s\n"), SDL_GetError());

		//

	IconSurface = SDL_CreateRGBSurfaceFrom((void *)icon_128x128, 128, 128, 32, 128 * 4, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	SDL_SetWindowIcon(window, IconSurface);
*/
	/*	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (screen == NULL) {
		throw MDFN_Error(0, _("SDL_CreateWindow() failed: %s\n"), SDL_GetError());
	}
  */
}

static uint32 howlong = 0;
static char *CurrentMessage = NULL;
static MDFN_NoticeType CurrentMessageType;

void Video_ShowNotice(MDFN_NoticeType t, char *s)
{
	howlong = Time::MonoMS() + MDFN_GetSettingUI("osd.message_display_time");

	if (CurrentMessage) {
		free(CurrentMessage);
		CurrentMessage = NULL;
	}

	CurrentMessage = s;
	CurrentMessageType = t;
}

void BlitOSD(MDFN_Surface *src, const MDFN_Rect *src_rect, const MDFN_Rect *dest_rect, int source_alpha)
{
	SDL_to_MDFN_Surface_Wrapper m_surface(screen);

	//MDFN_SrcAlphaBlitSurface(src, src_rect, &m_surface, dest_rect);
	MDFN_StretchBlitSurface(src, *src_rect, &m_surface, *dest_rect, (source_alpha > 0) && osd_alpha_blend);

	bool cond1 =
		(dest_rect->x < screen_dest_rect.x || (dest_rect->x + dest_rect->w) > (screen_dest_rect.x + screen_dest_rect.w));
	bool cond2 =
		(dest_rect->y < screen_dest_rect.y || (dest_rect->y + dest_rect->h) > (screen_dest_rect.y + screen_dest_rect.h));

	if (cond1 || cond2)
		MarkNeedBBClear();
}

static bool BlitInternalMessage(const uint32 curtime)
{
	if (curtime >= howlong) {
		if (CurrentMessage) {
			free(CurrentMessage);
			CurrentMessage = NULL;
		}
		return false;
	}

	if (CurrentMessage) {
		uint32 shad_color = SMSurface->MakeColor(0x00, 0x00, 0x00, 0xFF);
		uint32 text_color;
		uint8 fill_alpha = 0xC0;

		switch (CurrentMessageType) {
		default:
		case MDFN_NOTICE_STATUS:
			text_color = SMSurface->MakeColor(0xFF, 0xFF, 0xFF, 0xFF);
			break;

		case MDFN_NOTICE_WARNING:
			text_color = SMSurface->MakeColor(0xFF, 0xFF, 0x00, 0xFF);
			break;

		case MDFN_NOTICE_ERROR:
			text_color = SMSurface->MakeColor(0xFF, 0x00, 0x00, 0xFF);
			fill_alpha = 0xFF;
			break;
		}

		SMSurface->Fill(0x00, 0x00, 0x00, fill_alpha);

		DrawTextShadow(SMSurface, 0, 1, CurrentMessage, text_color, shad_color, MDFN_FONT_9x18_18x18, SMRect.w);
		free(CurrentMessage);
		CurrentMessage = NULL;
	}

	BlitOSD(SMSurface, &SMRect, &SMDRect);

	return true;
}

static void SubBlit(const MDFN_Surface *source_surface, const MDFN_Rect &src_rect, const MDFN_Rect &dest_rect,
		    const int InterlaceField)
{
	const MDFN_Surface *eff_source_surface = source_surface;
	MDFN_Rect eff_src_rect = src_rect;

	if (!(src_rect.w > 0 && src_rect.w <= 32767) || !(src_rect.h > 0 && src_rect.h <= 32767)) {
		//fprintf(stderr, "BUG: Source rect out of range; w=%d, h=%d\n", src_rect.w, src_rect.h);
		return;
	}
	//#if 0
	// assert(src_rect.w > 0 && src_rect.w <= 32767);
	// assert(src_rect.h > 0 && src_rect.h <= 32767);
	//#endif

	assert(dest_rect.w > 0);
	assert(dest_rect.h > 0);

	// No special scaler:
	{
		SDL_to_MDFN_Surface_Wrapper m_surface(screen);

		MDFN_StretchBlitSurface(eff_source_surface, eff_src_rect, &m_surface, dest_rect, false, video_settings.scanlines,
					&eff_src_rect, rotated, InterlaceField);
	}
}

// FIXME: JoyGunTranslate is kind of sloppy, not entirely thread safe, though the consequences in practice should be negligible...
static struct {
	float mul;
	float sub;
} volatile JoyGunTranslate[2] = { { 0, 0 }, { 0, 0 } };

void BlitScreen(MDFN_Surface *msurface, const MDFN_Rect *DisplayRect, const int32 *LineWidths, const int new_rotated,
		const int InterlaceField, const bool take_ssnapshot)
{
	//
	// Reduce CPU usage when minimized, and prevent OpenGL memory quasi-leaks on Windows(though I have the feeling there's a
	// cleaner less-racey way to prevent that memory leak problem).
	//
	//
	//
	//
	MDFN_Rect src_rect;

	if (rotated != new_rotated) {
		rotated = new_rotated;
		//
		/*		if (video_settings.fullscreen) {
			if (MDFN_UNLIKELY(!GenerateFullscreenDestRect()))
				Video_Sync(VideoGI);
			else
				MarkNeedBBClear();
		} else*/
		Video_Sync(VideoGI);
	}

#if 1
	{
		int sub[2] = { 0, 0 };
		//SDL_DisplayMode mode;

		screen_dest_rect.x = 0;
		screen_dest_rect.y = 0;
		sub[0] += screen_dest_rect.x;
		sub[1] += screen_dest_rect.y;
		/*
			if (SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &mode) >= 0) {
				JoyGunTranslate[0].mul = (float)mode.w / screen_dest_rect.w / 32768.0;
				JoyGunTranslate[0].sub = (float)sub[0] / screen_dest_rect.w;

				JoyGunTranslate[1].mul = (float)mode.h / screen_dest_rect.h / 32768.0;
				JoyGunTranslate[1].sub = (float)sub[1] / screen_dest_rect.h;
			}
      */
	}
#endif
	//
	//
	//
	const uint32 curtime = Time::MonoMS();

	if (NeedClear || video_settings.force_bbclear) {
		//printf("BBClear 0x%08x %d\n", curtime, NeedClear);

		if ((curtime - LastBBClearTime) >= 30) {
			LastBBClearTime = curtime;
			NeedClear -= (bool)NeedClear;
		}

		SDL_FillRect(screen, NULL, 0);
		NeedClear = 0;
	}

	msurface->SetFormat(emu_pf, true);

	src_rect.x = DisplayRect->x;
	src_rect.w = DisplayRect->w;
	src_rect.y = DisplayRect->y;
	src_rect.h = DisplayRect->h;

	if (LineWidths[0] == ~0) // Skip multi line widths code?
	{
		SubBlit(msurface, src_rect, screen_dest_rect, InterlaceField);
	} else {
		int y;
		int last_y = src_rect.y;
		int last_width = LineWidths[src_rect.y];

		MDFN_Rect sub_src_rect;
		MDFN_Rect sub_dest_rect;

		for (y = src_rect.y; y < (src_rect.y + src_rect.h + 1); y++) {
			if (y == (src_rect.y + src_rect.h) || LineWidths[y] != last_width) {
				sub_src_rect.x = src_rect.x;
				sub_src_rect.w = last_width;
				sub_src_rect.y = last_y;
				sub_src_rect.h = y - last_y;

				if (rotated == MDFN_ROTATE90) {
					sub_dest_rect.x =
						screen_dest_rect.x + (last_y - src_rect.y) * screen_dest_rect.w / src_rect.h;
					sub_dest_rect.y = screen_dest_rect.y;

					sub_dest_rect.w = sub_src_rect.h * screen_dest_rect.w / src_rect.h;
					sub_dest_rect.h = screen_dest_rect.h;
					//printf("sdr.x=%f, sdr.w=%f\n", (double)screen_dest_rect.x + (double)(last_y - src_rect.y) * screen_dest_rect.w / src_rect.h, (double)sub_src_rect.h * screen_dest_rect.w / src_rect.h);
				} else if (rotated == MDFN_ROTATE270) {
					sub_dest_rect.x = screen_dest_rect.x +
							  (src_rect.h - (y - src_rect.y)) * screen_dest_rect.w / src_rect.h;
					sub_dest_rect.y = screen_dest_rect.y;

					sub_dest_rect.w = sub_src_rect.h * screen_dest_rect.w / src_rect.h;
					sub_dest_rect.h = screen_dest_rect.h;
				} else {
					sub_dest_rect.x = screen_dest_rect.x;
					sub_dest_rect.w = screen_dest_rect.w;
					sub_dest_rect.y =
						screen_dest_rect.y + (last_y - src_rect.y) * screen_dest_rect.h / src_rect.h;
					sub_dest_rect.h = sub_src_rect.h * screen_dest_rect.h / src_rect.h;
				}

				if (!sub_dest_rect
					     .h) // May occur with small yscale values in certain cases, so prevent triggering an assert()
					sub_dest_rect.h = 1;

				// Blit here!
				SubBlit(msurface, sub_src_rect, sub_dest_rect, InterlaceField);

				last_y = y;

				if (y != (src_rect.y + src_rect.h)) {
					last_width = LineWidths[y];
				}
			}
		}
	}

	if (take_ssnapshot) {
		try {
			std::unique_ptr<MDFN_Surface> ib;
			MDFN_Rect sr;
			MDFN_Rect tr;

			sr = screen_dest_rect;
			if (sr.x < 0) {
				sr.w += sr.x;
				sr.x = 0;
			}
			if (sr.y < 0) {
				sr.h += sr.y;
				sr.y = 0;
			}
			if (sr.w < 0)
				sr.w = 0;
			if (sr.h < 0)
				sr.h = 0;
			if (sr.w > screen_w)
				sr.w = screen_w;
			if (sr.h > screen_h)
				sr.h = screen_h;

			ib.reset(new MDFN_Surface(NULL, sr.w, sr.h, sr.w, osd_pf));

			{
				if (SDL_MUSTLOCK(screen))
					SDL_LockSurface(screen);

				for (int y = 0; y < sr.h; y++) {
					for (int x = 0; x < sr.w; x++) {
						ib->pixels[y * ib->pitchinpix + x] = ((
							uint32 *)((uint8 *)screen->pixels + (sr.y + y) * screen->pitch))[sr.x + x];
					}
				}

				if (SDL_MUSTLOCK(screen))
					SDL_UnlockSurface(screen);
			}

			tr.x = tr.y = 0;
			tr.w = ib->w;
			tr.h = ib->h;
			MDFNI_SaveSnapshot(ib.get(), &tr, NULL);
		} catch (std::exception &e) {
			MDFND_OutputNotice(MDFN_NOTICE_ERROR, e.what());
		}
	}

	Debugger_MT_DrawToScreen(osd_pf, screen_w, screen_h);

	try {
		DrawSaveStates(screen_w, screen_h, exs, eys, osd_pf);

		CheatIF_MT_Draw(osd_pf, screen_w, screen_h);
		Netplay_MT_Draw(osd_pf, screen_w, screen_h);

		if (Help_IsActive()) {
			if (!HelpSurface) {
				HelpRect.x = 0;
				HelpRect.y = 0;
				HelpRect.w = std::min<int>(512, screen_w);
				HelpRect.h = std::min<int>(408, screen_h);

				HelpSurface = new MDFN_Surface(NULL, 512, 408, 512, osd_pf);
				Help_Draw(HelpSurface, HelpRect);
			}

			MDFN_Rect zederect;
			int32 sfx = screen_w / HelpRect.w;
			int32 sfy = screen_h / HelpRect.h;

			if (sfy > sfx)
				sfy = sfx + 1;

			if (sfx > sfy)
				sfx = sfy + 1;

			zederect.w = HelpRect.w * sfx;
			zederect.h = HelpRect.h * sfy;

			zederect.x = (screen_w - zederect.w) / 2;
			zederect.y = (screen_h - zederect.h) / 2;

			BlitOSD(HelpSurface, &HelpRect, &zederect, 0);
		} else if (HelpSurface) {
			delete HelpSurface;
			HelpSurface = NULL;
		}
	} catch (std::exception &e) {
		MDFND_OutputNotice(MDFN_NOTICE_ERROR, e.what());
	}

	BlitInternalMessage(curtime);

	//
	{
		int32 p[4] = { 0, 0, screen_w, screen_h };
		MDFN_Rect cr;

		// When using soft-SDL, position the FPS display so we won't incur a potentially large(on older/slower hardware) penalty due
		// to a requisite backbuffer clear(we could avoid this with some sort of dirty-rects system so only parts of the backbuffer are cleared,
		// but that gets awfully complicated and prone to bugs when dealing with double/triple-buffered video...).
		//
		// std::max so we don't position it offscreen if the user has selected xscalefs or yscalefs values that are too large.
		if (vdriver != VDRIVER_OPENGL) {
			p[0] = std::max<int32>(std::min<int32>(screen_w, screen_dest_rect.x), 0);
			p[1] = std::max<int32>(std::min<int32>(screen_h, screen_dest_rect.y), 0);

			p[2] = std::max<int32>(std::min<int32>(screen_w, screen_dest_rect.x + screen_dest_rect.w), 0);
			p[3] = std::max<int32>(std::min<int32>(screen_h, screen_dest_rect.y + screen_dest_rect.h), 0);
		}

		cr = { p[0], p[1], p[2] - p[0], p[3] - p[1] };
		FPS_DrawToScreen(osd_pf, cr, std::min(screen_w, screen_h));
	}
	//

	//SDL_UpdateWindowSurface(window);
	SDL_Flip(screen);
}

void Video_Exposed(void)
{
	MarkNeedBBClear();
}

void Video_PtoV(const int in_x, const int in_y, float *out_x, float *out_y)
{
	assert(VideoGI);
	//
	int32 tmp_x = in_x - screen_dest_rect.x;
	int32 tmp_y = in_y - screen_dest_rect.y;
	int32 div_x = screen_dest_rect.w;
	int32 div_y = screen_dest_rect.h;
	/*
 switch(rotated)
 {
  case MDFN_ROTATE0:
	break;

  case MDFN_ROTATE90:
	std::swap(tmp_x, tmp_y);
	std::swap(div_x, div_y);
	tmp_x = screen_dest_rect.h - 1 - tmp_x;
	break;

  case MDFN_ROTATE270:
	std::swap(tmp_x, tmp_y);
	std::swap(div_x, div_y);
	tmp_y = screen_dest_rect.w - 1 - tmp_y;
	break;
 }
*/
	*out_x = (float)tmp_x / div_x;
	*out_y = (float)tmp_y / div_y;
}

float Video_PtoV_J(const int32 inv, const bool axis, const bool scr_scale)
{
	assert(VideoGI);

	if (!scr_scale)
		return 0.5 + (inv / 32768.0 - 0.5) * std::max<int32>(VideoGI->nominal_width, VideoGI->nominal_height) /
				     (axis ? VideoGI->nominal_height : VideoGI->nominal_width);
	else
		return inv * JoyGunTranslate[axis].mul - JoyGunTranslate[axis].sub;
}
