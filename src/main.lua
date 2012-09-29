function import(ns)
	for k,v in pairs(ns) do
		_G[k] = v
	end
end

import(scene)
import(math)

-- create the planets

function createPlanet(x, y, color, radius)
	local result = createNode()
	setPosition(result, vec(x, y))
	circle.create(result, color, radius)
	return result
end

local planetx = createPlanet(100, 100, 0x444488, 48)
local sun = createPlanet(400, 400, 0xffff00, 32)
local urth = createPlanet(128, 0, 0x8888ff, 24)
local moon = createPlanet(48, 0, 0x888888, 12)	
attachTo(sun, urth)
attachTo(urth, moon)

-- create the noodle

function addNode(x, y)
	local result = createNode()
	setPosition(result, vec(x, y))
	return result
end

local p0 = addNode(0, -50)
local p1 = addNode(200, 100)
local p2 = addNode(200, 600)
local p3 = addNode(300, 850)
setDirection(p0, vec(50, 50))
setDirection(p3, vec(50, 50))
local noodle = spline.addMaterial(8, 0x777755)
spline.addSegment(p0, p1, noodle)
spline.addSegment(p1, p2, noodle);
spline.addSegment(p2, p3, noodle);

-- game loop

local isDone = false
while not isDone do
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mp = input.mousePosition()
	
	-- planets
	circle.setRadius(sun, 32 + 4*sin(2.1*tau*t))
	setPosition(sun, mp)
	setRotation(sun, -0.11*360*t)
	setRotation(urth, 360*t)
	local p = position(planetx)
	setPosition(planetx, 0.1*mp + 0.9*p)

	-- noodle dance
	setDirection(p1, mp - vec(200, 100))
	setDirection(p2, vec(200,600) - mp)
	spline.setWeight(noodle, 8  + 32 * (0.5 + 0.5 * sin(tau*t)))

	script.yield()
end

