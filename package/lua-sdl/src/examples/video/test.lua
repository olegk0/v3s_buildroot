--
-- image.lua -- shows SDL_image module for Lua
--

local SDL	= require "SDL1"

local ret, err = SDL.init { SDL.flags.Video }
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

local sw,sh=surface:getSize()
print("w:",sw,"h:",sh)

le =true
while le do
	x=math.random(sw-1)
	y=math.random(sh-1)
	w=math.random(sw - x+1)
	h=math.random(sw - y+1)
	
	surface:fillRect({	x=x,
			y=y,
			w=w,
			h=h }, {r=math.random(255),g=math.random(255),b=math.random(255)} )

	surface:flip()
	SDL.delay(100)
	
	for e in SDL.pollEvent() do
		print("type:",e.type)
		print("state:",e.state)
		if e.keysym then
			print("scancode:",e.keysym.scancode)
			print("sym:",e.keysym.sym)
			print("mod:",e.keysym.mod)
		--if e.type == SDL.event.Quit then		
		if e.keysym.sym == SDL.key.Escape then		
			le=false		
		end
		end
	end
end

