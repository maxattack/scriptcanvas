print("Bootstrap Script")
root = create_node()
root:add_component(0)
root:set_position(100,100)
root:init_circle(48.0, 0.33, 0.25, 0.65)
x,y = root:get_position()
print (x..", "..y)