--
-- image.lua -- shows SDL_image module for Lua
--

local SDL	= require "SDL1"
local image	= require "SDL1.image"

local ret, err = SDL.init { SDL.flags.Video }
if not ret then
	error(err)
end

local formats, ret, err = image.init { image.flags.PNG }
if not ret then
	error(err)
end

local surface, err = SDL.setVideoMode {
    bpp	= 32,
    height	= 250,
    width	= 250,
    flags = { SDL.video.HwSurface }
}
if not surface then
    error(err)
end

local img, ret = image.load("Lua-SDL2.png")
if not img then
	error(err)
end

for i = 1, 50 do
    img:blit(surface)
    surface:flip()

    SDL.delay(100)
end
