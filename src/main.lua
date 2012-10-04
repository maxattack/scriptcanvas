local tau = 2 * 3.14156
local root = node.create(0, vec(300, 300))

-- create ring of circles

local num_circles = 50
local cm = material.create(0x555555, 6)
local rotor = node.create(root)
for i=0,50 do
	circle.create(node.create(rotor, vec.polar(300, i*tau/50)), cm)
end

-- create blue "arm"

local center = node.create(0, vec(150,150))
local bm = material.create(0x445588, 16)
spline.create(center, root, bm, stroke.create(0.2, 1), 0.1)
local cbm = material.create(0x445588, 48)
circle.create(root, cbm)
circle.create(center, cbm)

-- create blobby triangle

local m = material.create(0xCCCC77, 8)
local p0 = node.create(root, vec.polar(180, 0))
local p1 = node.create(root, vec.polar(180, tau/3.0))
local p2 = node.create(root, vec.polar(180, 2.0*tau/3.0))
local t = stroke.create(1, 1, -0.7)
local e0 = spline.create(p0, p1, m, t)
local e1 = spline.create(p1, p2, m, t)
local e2 = spline.create(p2, p0, m, t)
circle.create(p0, m)
circle.create(p1, m)
circle.create(p2, m)

-- create little triangles

function addTriangle(mat, angle)
	local result = node.create(root, vec.polar(150, angle), vec.polar(1, angle))
	local p0 = node.create(result, vec.polar(50, 0))
	local p1 = node.create(result, vec.polar(50, tau/3.0))
	local p2 = node.create(result, vec.polar(50, 2*tau/3.0))
	local e0 = spline.create(p0, p1, mat, 0, 0)
	local e1 = spline.create(p1, p2, mat, 0, 0)
	local e2 = spline.create(p2, p0, mat, 0, 0)
	circle.create(p0, mat)
	circle.create(p1, mat)
	circle.create(p2, mat)
	return result
end

local m0 = material.create(0xAAAA66, 4)
local t0 = addTriangle(m0, tau/6.0)
local t1 = addTriangle(m0, tau/6.0 + tau/3.0)
local t2 = addTriangle(m0, tau/6.0 + 2*tau/3.0)

-- interaction loop

function lerp(u, lo, hi)
	return lo + u * (hi - lo)
end

while not input.quit() do 
	node.setPosition(root, 0.25 * input.mousePosition() + 0.75 * node.position(root))
	node.setRotation(root, 0.25 * tau * input.seconds())
	node.setRotation(rotor, -0.18 * tau * input.seconds())
	local eccen = lerp(0.5 + 0.5 * math.sin(0.5 * tau * input.seconds()), -0.14, 0.2)
	spline.setEccentricity(e0, eccen)
	spline.setEccentricity(e1, eccen)
	spline.setEccentricity(e2, eccen)
	script.yield() 
end
