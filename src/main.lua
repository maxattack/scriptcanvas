
local root = scene.createNode()
scene.setPosition(root, vec(300, 300))

function addVert(angle)
	local result = scene.createNode(root)
	scene.setPosition(result, vec.polar(100, angle))
	return result
end

local noodle = spline.addMaterial(8, 0xaaaa77)
local tau = 2 * 3.14156
local p0 = addVert(0)
local p1 = addVert(tau/3.0)
local p2 = addVert(2.0*tau/3.0)
spline.addCubicSegment(p0, p1, noodle)
spline.addCubicSegment(p1, p2, noodle)
spline.addCubicSegment(p2, p0, noodle)

circle.create(p0, 0xaaaa77, 4/2)
circle.create(p1, 0xaaaa77, 4/2)
circle.create(p2, 0xaaaa77, 4/2)

while not input.quit() do 
	scene.setPosition(root, input.mousePosition())
	--scene.setRotation(root, 100.0 * input.seconds())
	scene.setDirection(root, vec.polar(2, input.seconds()))
	script.yield() 
end

-- function import(ns)
-- 	for k,v in pairs(ns) do
-- 		_G[k] = v
-- 	end
-- end

-- function clmp(x, lo, hi)
-- 	if x < lo then
-- 		return lo 
-- 	elseif x > hi then
-- 		return hi 
-- 	else 
-- 		return x 
-- 	end
-- end

-- import(scene)
-- import(math)

-- -- create the planets

-- function createPlanet(x, y, color, radius)
-- 	local result = createNode()
-- 	setPosition(result, vec(x, y))
-- 	circle.create(result, color, radius)
-- 	return result
-- end

-- local planetx = createPlanet(100, 100, 0x444488, 48)
-- local sun = createPlanet(400, 400, 0xffff00, 32)
-- local urth = createPlanet(128, 0, 0x8888ff, 24)
-- local moon = createPlanet(48, 0, 0x888888, 12)	
-- attachTo(sun, urth)
-- attachTo(urth, moon)

-- -- create the noodle

-- function addNode(x, y)
-- 	local result = createNode()
-- 	setPosition(result, vec(x, y))
-- 	return result
-- end

-- local p0 = addNode(0, -50)
-- local p1 = addNode(200, 100)
-- local p2 = addNode(200, 600)
-- local p3 = addNode(300, 850)
-- setDirection(p0, vec(50, 50))
-- setDirection(p3, vec(50, 50))
-- local noodle = spline.addMaterial(8, 0x999955)
-- spline.addSegment(p0, p1, noodle)
-- spline.addSegment(p1, p2, noodle);
-- spline.addSegment(p2, p3, noodle);
-- spline.setTaper(p0, 0)
-- spline.setTaper(p3, 0)

-- -- game loop

-- while not input.quit() do
-- 	local tau = 2 * 3.14156
-- 	local t = input.seconds()
-- 	local mp = input.mousePosition()
	
-- 	-- planets
-- 	circle.setRadius(sun, 32 + 4*sin(2.1*tau*t))
-- 	setPosition(sun, mp)
-- 	setRotation(sun, -0.11*360*t)
-- 	setRotation(urth, 360*t)
-- 	local p = 0.1 * mp + 0.9 * position(planetx)
-- 	setPosition(planetx, p)

-- 	-- noodle dance
-- 	p = p - vec(400,400)
-- 	local len = clmp(vec.length(p), 0, 200)
-- 	p = vec.polar(len, vec.angle(p)) + vec(400,400)
-- 	setDirection(p1, p - vec(200, 100))
-- 	setDirection(p2, vec(200,600) - p)
-- 	spline.setWeight(noodle, 8  + 32 * (0.5 + 0.5 * sin(tau*t)))

-- 	script.yield()
-- end
