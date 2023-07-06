--
-- font.lua -- show SDL_ttf module with UTF-8 string
--

local SDL	= require "SDL1"
local font	= require "SDL1.ttf"

local ret, err = SDL.init { SDL.flags.Video }
if not ret then
	error(err)
end

local ret, err = font.init()
if not ret then
	error(err)
end

local surface, err = SDL.setVideoMode {
    bpp	= 32,
    height	= 600,
    width	= 800,
    flags = { SDL.video.HwSurface }
}

if not surface then
	error(err)
end

local f, err = font.open("DejaVuSans.ttf", 40)
if not f then
	error(err)
end

local s, err = f:renderUtf8("Gérard! Hello!", "blended", { r = 255, g = 255, b = 255 })
if not s then
	error(err)
end

s:blit(surface)

surface:flip()

SDL.delay(5000)