import(scene)
import(circle)
import(math)

function createPlanet(x, y, radius, r, g, b)
	local result = createNode()
	setPosition(result, x, y)
	addComponent(result, component.circle)
	setMaterial(result, createMaterial(r, g, b))
	setGeometry(result, createGeometry(radius))
	return result
end

-- test
local n1 = createNode()
local n2 = createNode(n1)
local n3 = createNode(n2)
local n4 = createNode(n3)
local n5 = createNode()
attachTo(n5, n1)
destroyNode(n5)

n1 = createNode()
local planetx = createPlanet(100, 100, 48, 0.33, 0.25, 0.65)
n2 = createNode()
local sun = createPlanet(400, 400, 32, 1, 1, 0.25)
n3 = createNode()
local urth = createPlanet(128, 0, 24, 0.5, 0.5, 0.8)
n4 = createNode()
local moon = createPlanet(48, 0, 12, 0.4, 0.4, 0.4)	
n5 = createNode()
attachTo(sun, urth)
attachTo(urth, moon)

-- test
attachTo(n5, n1)
destroyNode(n5)

local isDone = false

while not isDone do
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mx, my = input.mousePosition()
	setGeometryRadius(geometry(sun), 32 + 4*sin(2.1*tau*t))
	--print("setPosition sun")
	setPosition(sun, mx, my)
	--print("setRotation sun")
	setRotation(sun, -0.11*360*t)
	--print("setRotation urth")
	setRotation(urth, 360*t)
	local x,y = positionOf(planetx)
	--print("setPosition planetX")
	setPosition(planetx, 0.1*mx + 0.9*x, 0.1*my + 0.9*y)
	paint()
end
