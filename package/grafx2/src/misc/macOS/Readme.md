# GrafX2 for macOS

This build uses Xcode to create a complete standalone (portable) macOS app bundle for macOS 10.13 or higher (Intel and Apple Silicon, 64 Bit).

Some remarks:

* this build uses Frameworks wherever possible, so you need those (SDL, SDL_ttf, SDL_image, Lua, ...). Dylibs provided via homebrew or MacPorts are not supported. For SDL1 support you can use the SDL1_compat build on [GitHub](https://github.com/Alexco500/libsdl12-compat)
* You need to change the include path if your frameworks are not located in ~/Library/Frameworks
* libpng and libtiff are linked statically, but you can change that easily in Xcode
* you need to run `make 6502` and `make recoil`in the 3rdparty folder to get the necessary patched 6502 and recoil files
* SDL1 API is currently used, if you want SDL2 API, you need to change the build settings in Xcode (Project/Preprocessor Macros)
* `version.c` file is created during build, using git revision. Check `build-scripts/update_bundle_from_git.sh` for details
* These symbols are used
	* `__SDL_WRAPPER__`
	* `__macosx__`
	* `__ENABLE_LUA__`
	* `CPU_6502_DEPENDENCIES_H=\"6502types.h\" `
	* `CPU_6502_STATIC`
	* `USE_SDL`
	* `ENABLE_FILENAMES_ICONV`
