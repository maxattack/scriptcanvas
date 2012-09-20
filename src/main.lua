import(scene)
import(math)

function createPlanet(x, y, color, radius)
	local result = createNode()
	setPosition(result, x, y)
	circle.create(result, color, radius)
	return result
end

local planetx = createPlanet(100, 100, 0x444488, 48)
local sun = createPlanet(400, 400, 0xffff00, 32)
local urth = createPlanet(128, 0, 0x8888ff, 24)
local moon = createPlanet(48, 0, 0x888888, 12)	
attachTo(sun, urth)
attachTo(urth, moon)

local isDone = false

while not isDone do
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mx, my = input.mousePosition()
	circle.setRadius(sun, 32 + 4*sin(2.1*tau*t))
	setPosition(sun, mx, my)
	setRotation(sun, -0.11*360*t)
	setRotation(urth, 360*t)
	local x,y = positionOf(planetx)
	setPosition(planetx, 0.1*mx + 0.9*x, 0.1*my + 0.9*y)
	paint()
end
