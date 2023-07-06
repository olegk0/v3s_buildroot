--
-- keyboard.lua -- process keyboard states
--

local SDL	= require "SDL1"

SDL.init {
	SDL.flags.Video
}

local surface, err = SDL.setVideoMode {
    bpp	= 32,
    height	= 50,
    width	= 50,
    --flags = { SDL.video.HwSurface }
}

local keys = SDL.getKeyboardState()

while true do
	SDL.pumpEvents()
	if keys[SDL.key.Return] then
		print("Return pressed")
	end
	if keys[SDL.key.Escape] then
		print("Exiting!")
		break
	end

	SDL.delay(1000)
end
