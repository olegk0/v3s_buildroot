--
-- server.lua -- UDP server
--

local net	= require "SDL1.net"

-- Init
net.init()

-- Create a server socket
local s = net.openUdp(9898)

while true do
	local v, num = s:recv(32)

	if v then
		print(v)
	end
end
