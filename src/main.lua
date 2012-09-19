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

n1 = createNode()
n2 = createNode(n1)
n3 = createNode(n2)
n4 = createNode()
attachTo(n4, n1)

local planetx = createPlanet(100, 100, 48, 0.33, 0.25, 0.65)
local sun = createPlanet(400, 400, 32, 1, 1, 0.25)
local urth = createPlanet(128, 0, 24, 0.5, 0.5, 0.8)
local moon = createPlanet(48, 0, 12, 0.4, 0.4, 0.4)	
attachTo(sun, urth)
attachTo(urth, moon)

destroyNode(n4)

local isDone = false

while not isDone do
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mx, my = input.mousePosition()
	setGeometryRadius(geometry(sun), 32 + 4*sin(2.1*tau*t))
	setPosition(sun, mx, my)
	setRotation(sun, -0.11*360*t)
	setRotation(urth, 360*t)
	local x,y = positionOf(planetx)
	setPosition(planetx, 0.1*mx + 0.9*x, 0.1*my + 0.9*y)
	paint()
end
