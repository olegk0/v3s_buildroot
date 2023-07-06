Lua-SDL (SDL1.2 binding for Lua)
==================================

Redesigned for SDL 1.2 (not all features)

the examples have also been redone and mostly work




The following is the text of the original readme:



Lua-SDL (SDL binding for Lua)
==================================

Lua-SDL is a pure C binding of SDL to Lua 5.1, Lua 5.2, Lua 5.3, Lua 5.4, and
LuaJIT.

The current version is 2.1, compatible with SDL 2.0.1 - 2.0.5.

Lua-SDL follows the [SemVer](https://semver.org) standard with regards to project
versioning.

Features
========

Lua-SDL is a portable binding of SDL, written in pure C for efficiency.
It tries to stay as close to SDL as possible, acting as a simple binding
rather than attempting to re-design the interaction between the programmer
and SDL.

Lua-SDL takes full advantage of Lua's object-oriented capabilities wherever
possible, allowing the programmer to fully leverage SDL's inherent
object-oriented design.

Lua-SDL is very well documented, with copious source-code comments, and a full
API reference available at [the wiki](https://github.com/Tangent128/LuaSDL/wiki/).

Compatibility
=============

Lua-SDL is designed to be as compatible as possible.  The library has support
for all the latest features of SDL, while still compiling with SDL 2.0.1.

If the library is compiled with a newer point release of SDL than what is
listed here, the library should still function, simply without access to the
newer features.

Lua-SDL is compatible with:

	* Lua 5.1, 5.2, 5.3, 5.4 or LuaJIT
	* SDL 1.0.6 and greater

Installing
==========

If you have LuaRocks installed,

    $ luarocks install lua-sdl

Otherwise, read `INSTALL.md` for instructions to build it yourself.


Author and Maintainer
=====================

The Lua-SDL2 library was written by [David Demelier](mailto:markand@malikania.fr).

It is currently being maintained by
