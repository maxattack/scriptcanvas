function init()
	root = create_node()
	--root:set_position(100 + 32 * math.cos(get_time()), 100 + 32 * math.sin(get_time()))
	root:set_position(100,100)
	root:add_circle():init(48, 0.33, 0.25, 0.65)
end

function update()
	x,y = root:get_position()
	mx,my = get_mouse_position()
	x = 0.1 * mx + 0.9 * x
	y = 0.1 * my + 0.9 * y
	root:set_position(x, y)
end