#
# FindSDL.cmake -- find SDL library and addons
#
# Copyright (c) 2020 David Demelier <markand@malikania.fr>
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
#
# Find SDL library and components, this modules defines:
#
#  SDL_LIBRARY, the name of the library to link against.
#  SDL_MAIN_LIBRARY, for SDLmain (if present).
#  SDL_LIBRARIES, alias to SDL_LIBRARY.
#  SDL_FOUND, true if found.
#  SDL_INCLUDE_DIR, where to find SDL.h.
#
# The following imported targets will be defined:
#
# SDL::SDL
# SDL::SDLmain (if present)
#
# This module also handle the following official SDL addons:
#
# - image
# - mixer
# - net
# - ttf
#
# And thus, variables SDL_<C>_LIBRARY, SDL_<C>_INCLUDE_DIRS and SDL::<C>
# imported targets will be defined if they are found.
#

include(FindPackageHandleStandardArgs)

# The official include convention is <SDL.h> not <SDL/SDL.h>.
find_path(
	SDL_INCLUDE_DIR
	NAMES SDL.h
	PATH_SUFFIXES include/SDL include
)

find_library(SDL_LIBRARY NAMES SDL libSDL)
find_library(SDL_MAIN_LIBRARY NAMES SDLmain libSDLmain)

# Standard components.
foreach (c ${SDL_FIND_COMPONENTS})
	message(comp="${c}")
	find_path(
		SDL_${c}_INCLUDE_DIR
		NAMES SDL.h
		PATH_SUFFIXES include/SDL include
	)

message(lib="SDL_${c} libSDL_${c}")
	find_library(
		
		SDL_${c}_LIBRARY
		NAMES SDL_${c} libSDL_${c}
	)

	if (NOT TARGET SDL::${c} AND SDL_${c}_LIBRARY)
		set(SDL_${c}_FOUND TRUE)
		add_library(SDL::${c} UNKNOWN IMPORTED)
		set_target_properties(
			SDL::${c}
			PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "C"
				IMPORTED_LOCATION "${SDL_${c}_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${SDL_${c}_INCLUDE_DIRS}"
		)
	endif ()

	mark_as_advanced(SDL_${c}_INCLUDE_DIR SDL_${c}_LIBRARY)
endforeach ()

find_package_handle_standard_args(
	SDL
	REQUIRED_VARS SDL_LIBRARY SDL_INCLUDE_DIR
	HANDLE_COMPONENTS
)

if (SDL_FOUND)
	set(SDL_LIBRARIES ${SDL_LIBRARY})
	set(SDL_INCLUDE_DIRS ${SDL_INCLUDE_DIR})

	if (NOT TARGET SDL::SDL)
		add_library(SDL::SDL UNKNOWN IMPORTED)
		set_target_properties(
			SDL::SDL
			PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "C"
				IMPORTED_LOCATION "${SDL_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${SDL_INCLUDE_DIRS}"
		)
	endif ()

	if (NOT TARGET SDL::main AND SDL_MAIN_LIBRARY)
		add_library(SDL::main UNKNOWN IMPORTED)
		set_target_properties(
			SDL::main
			PROPERTIES
				IMPORTED_LINK_INTERFACE_LANGUAGES "C"
				IMPORTED_LOCATION "${SDL_MAIN_LIBRARY}"
				INTERFACE_INCLUDE_DIRECTORIES "${SDL_INCLUDE_DIRS}"
		)
	endif ()
endif ()

mark_as_advanced(SDL_INCLUDE_DIR SDL_LIBRARY)
