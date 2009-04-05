#!/usr/bin/env ruby

require 'osc'


RED	= 0xFF0000
GREEN	= 0x00FF00
BLUE = 0x0000FF

def getRainbowColor(cyclePoint)
	
	# We blend between each of these points
	blendPoints = [
		RED,			    # red
		RED | GREEN,	# yellow
		GREEN,			  # green
		GREEN | BLUE,	# cyan
		BLUE,			    # blue
		RED | BLUE		# magenta
	]
	
	# Convert a point between 0.0 and 1.0 to a point between 0.0 and 6.0
	fullCyclePoint = cyclePoint * 6.0
	
	# Find which of the points we need to blend between
	blendIdx1  = fullCyclePoint.to_i
	blendIdx2 = blendIdx1+1
	blendIdx2 = 0 if  blendIdx2 >= 6
	
	# Find the RGB values of the points we're blending between, so that we
	# can blend between each of the compontents individually
	red1   = (blendPoints[blendIdx1] & RED)   >> 16
	green1 = (blendPoints[blendIdx1] & GREEN) >> 8
	blue1  = (blendPoints[blendIdx1] & BLUE)  >> 0
	
	red2   = (blendPoints[blendIdx2] & RED)   >> 16
	green2 = (blendPoints[blendIdx2] & GREEN) >> 8
	blue2  = (blendPoints[blendIdx2] & BLUE)  >> 0
	
	# Find the blend point (0.0 to 1.0) between two of the six points
	lerpValue = (fullCyclePoint - blendIdx1)
	
	# Do the blend, and convert RGB components from 0-255 to 0.0-1.0
	red   = ( (1.0-lerpValue)*red1   + lerpValue*red2   ) / 255.0
	green = ( (1.0-lerpValue)*green1 + lerpValue*green2 ) / 255.0
	blue  = ( (1.0-lerpValue)*blue1  + lerpValue*blue2  ) / 255.0
	
	return red,green,blue
end


sock = OSC::UDPSocket.new
sock.connect('localhost', 7770)

cycle=0
loop do
  cycle -= 1 if cycle>1
  
  r,g,b = getRainbowColor(cycle)
  p [cycle,r,g,b]
  
  sock.send OSC::Message.new('/dmx/0/set', 'f', r), 0
  sock.send OSC::Message.new('/dmx/1/set', 'f', g), 0
  sock.send OSC::Message.new('/dmx/2/set', 'f', b), 0

  sleep 0.01
  
  cycle += 0.001
end
