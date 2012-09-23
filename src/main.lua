import(scene)
import(math)

-- create the planets

function createPlanet(x, y, color, radius)
	local result = addNode()
	setPosition(result, x, y)
	circle.add(result, color, radius)
	return result
end

local planetx = createPlanet(100, 100, 0x444488, 48)
local sun = createPlanet(400, 400, 0xffff00, 32)
local urth = createPlanet(128, 0, 0x8888ff, 24)
local moon = createPlanet(48, 0, 0x888888, 12)	
attachTo(sun, urth)
attachTo(urth, moon)

-- create the noodle

function createNode(x, y)
	local result = addNode()
	setPosition(result, x, y)
	return result
end

local p0 = createNode(0, -50)
local p1 = createNode(200, 100)
local p2 = createNode(200, 600)
local p3 = createNode(300, 850)
setDirection(p0, 50, 50)
setDirection(p3, 50, 50)
local noodle = spline.addMaterial(8, 0x777755)
spline.addSegment(p0, p1, noodle)
spline.addSegment(p1, p2, noodle);
spline.addSegment(p2, p3, noodle);

-- game loop

local isDone = false
while not isDone do
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mx, my = input.mousePosition()
	
	-- planets
	circle.setRadius(sun, 32 + 4*sin(2.1*tau*t))
	setPosition(sun, mx, my)
	setRotation(sun, -0.11*360*t)
	setRotation(urth, 360*t)
	local x,y = position(planetx)
	setPosition(planetx, 0.1*mx + 0.9*x, 0.1*my + 0.9*y)

	-- noodle dance
	setDirection(p1, (mx - 200), (my - 100))
	setDirection(p2, (200 - mx), (600 - my))
	spline.setWeight(noodle, 8  + 32 * (0.5 + 0.5 * sin(tau*t)))

	paint()
end

