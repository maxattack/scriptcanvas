
function createPlanet(x, y, radius, r, g, b)
	result = scene.createNode()
	scene.setPosition(result, x, y)
	scene.addComponent(result, component.circle)
	circle.setMaterial(result, circle.createMaterial(r, g, b))
	circle.setGeometry(result, circle.createGeometry(radius))
	return result
end

function init()
	planetx = createPlanet(100, 100, 48, 0.33, 0.25, 0.65)
	sun = createPlanet(400, 400, 32, 1, 1, 0.25)
	urth = createPlanet(128, 0, 24, 0.5, 0.5, 0.8)
	moon = createPlanet(48, 0, 12, 0.4, 0.4, 0.4)	
	scene.attach(sun, urth)
	scene.attach(urth, moon)
end

function update()
	local tau = 2 * 3.14156
	local t = input.seconds()
	local mx, my = input.mousePosition()
	circle.setGeometryRadius(circle.geometry(sun), 32 + 4*math.sin(2.1*tau*t))
	scene.setPosition(sun, mx, my)
	scene.setRotation(sun, -0.11*360*t)
	scene.setRotation(urth, 360*t)
	x,y = scene.positionOf(planetx)
	scene.setPosition(planetx, 0.1*mx + 0.9*x, 0.1*my + 0.9*y)
end


