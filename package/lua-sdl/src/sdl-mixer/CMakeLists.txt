#
# CMakeLists.txt -- build system for LuaSDL (SDL_mixer module)
#
# Copyright (c) 2013, 2014 David Demelier <markand@malikania.fr>
# Copyright (c) 2014 Joseph Wallace <tangent128@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

set(
	MIXER_SOURCES
	src/mixer.c
)

add_library(
	mixer
	MODULE
	${MIXER_SOURCES}
)

set_target_properties(
	mixer
	PROPERTIES
	PREFIX ""
)

target_link_libraries(
	mixer
	common
	SDL::mixer
)

install(
	TARGETS mixer
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/lua/${Lua_VERSION}/SDL1
)
