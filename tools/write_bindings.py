import os, os.path, sys, traceback

# GIFT IDEAS
#	- support multiple return values (expressed in C++ as pointer "out" arguments)
#	- precompile "bootstrap" script and plunk opcode binary into source file
#	- check ID types in DEBUG builds

MODULES = {

	'script': {
		'yield':				'void ScriptSystem::Yield()'
	},

	'scene': {
		'nodeCount': 			'int SceneSystem::NodeCount()',
		'createNode': 			'ID SceneSystem::CreateNode(ID parent=0, vec2_t position=VZERO, vec2_t attitude=VUNITY, float depth=0.f)',
		'attachTo': 			'void SceneSystem::AttachNode(ID parent, ID child)',
		'detach': 				'void SceneSystem::DetachNode(ID child)',
		'parent': 				'ID SceneSystem::Parent(ID node)',
		'destroyNode':			'void SceneSystem::DestroyNode(ID node)',
		'position': 			'vec2_t SceneSystem::Position(ID node)',
		'rotation': 			'float SceneSystem::Rotation(ID node)',
		'direction': 			'vec2_t SceneSystem::Direction(ID node)',
		'setPosition': 			'void SceneSystem::SetPosition(ID node, vec2_t pos)',
		'setRotation': 			'void SceneSystem::SetRotation(ID node, float radians)',
		'setDirection': 		'void SceneSystem::SetDirection(ID node, vec2_t dir)',
		'setName': 				'void NameSystem::SetName(ID node, std::string name)',
		'clearName':			'void NameSystem::ClearName(ID node)',
		'name':					'std::string NameSystem::Name(ID node)',
		'find':					'ID NameSystem::FindNode(std::string name)'
	},

	'material': {
		'create':				'ID MaterialSystem::CreateMaterial(color_t color, float weight)',
		'destroy':				'void MaterialSystem::DestroyMaterial(ID mid)',
		'weight':				'float MaterialSystem::Weight(ID mid)',
		'color':				'color_t MaterialSystem::Color(ID mid)',
		'setWeight':			'void MaterialSystem::SetWeight(ID mid, float weight)',
		'setColor':				'void MaterialSystem::SetColor(ID mid, color_t c)'
	},

	'circle': {
		'create':				'void CircleSystem::Create(ID node, ID mid)',
		'destroy': 				'void CircleSystem::Destroy(ID node)',
		'fill': 				'color_t CircleSystem::Fill(ID node)',
		'radius': 				'float CircleSystem::Radius(float radius)',
		'setFill': 				'void CircleSystem::SetFill(ID node, color_t fill)',
		'setRadius': 			'void CircleSystem::SetRadius(ID node, float radius)'
	},

	'input': {
		'mousePosition': 		'vec2_t InputSystem::MousePosition()',
		'seconds': 				'double InputSystem::Time()',
		'quit':					'bool InputSystem::Quit()'
	},

	'spline': {
		'addCubicSegment':		'ID SplineSystem::CreateCubicSegment(ID start, ID end, ID mat)',

		'addEccentricSegment':	'ID SplineSystem::CreateEccentricSegment(ID start, ID end, ID mat, float eccentricity=0.f)',
		'eccentricity':			'float SplineSystem::Eccentricity(ID esid)',
		'setEccentricity':		'void SplineSystem::SetEccentricity(ID esid, float e)',
		
		'taper':				'float SplineSystem::Taper(ID node)',
		'setTaper':				'void SplineSystem::SetTaper(ID node, float taper)'

	},

}

HEADER = '''
#include "ScriptSystem.h"
// GENERATED BY "tools/write_bindings.py"
// DO NOT MODIFY BY HAND

#define VZERO   LVec(0,0)
#define VUNITY  LVec(1,0)

namespace ScriptSystem {
namespace Internal {

static void LoadModule(lua_State* L, const char* name, const luaL_Reg *p, int nfuncs) {
	// I think this is a dup of a luaauxlib function, but I figure,
	// don't fix it if it ain't broke!
	lua_createtable(L, 0, nfuncs);
	while(p->name) {
		lua_pushstring(L, p->name);
		lua_pushcfunction(L, p->func);
		lua_settable(L, -3);
		++p;
	}
	lua_setglobal(L, name);
}

inline vec2_t Vec2(lua_Vector v) {
	return *(vec2_t*)(&v);
}

inline lua_Vector LVec(float x, float y) {
	lua_Vector result = { x, y };
	return result;
}

inline std::string GetString(lua_State* L, int arg) {
	size_t length;
	const char* result = luaL_checklstring(L, arg, &length);
	return std::string(result, length);
}

inline std::string OptString(lua_State* L, int arg) {
	size_t length;
	const char* result = luaL_optlstring(L, arg, "", &length);
	return std::string(result, length);
}

'''

def main():
	with open(sys.argv[1], 'w') as src:
		src.write(HEADER)
		for module, signatures in MODULES.iteritems():
			for name,sig in signatures.iteritems():
				write_function_implementation(src, module, name, sig)
			src.write("static const luaL_Reg _%s[] = {\n" % module)
			for name in signatures:
				write_line(src, '{ "%s", _%s_%s  },' % (name, module, name))
			write_line(src, '{ NULL, NULL }')
			src.write("};\n\n")
		src.write("}\n\n")
		src.write('void Bind(lua_State* L) {\n')
		for module, signatures in MODULES.iteritems():
			write_line(src, 'Internal::LoadModule(L, "%s", Internal::_%s, %d);' % (module, module, len(signatures)))
		src.write('}\n\n}\n\n')


def write_function_implementation(src, module, name, sig):
	result_type, command, args = unpack_signature(sig)
	src.write('static int _%s_%s(lua_State* L) {\n' % (module, name))
	# collect args
	write_line(src, "int nargs = lua_gettop(L);")
	if args:
		total_arg_count = len(args)
		opt_arg_count = len([def_val for _,_,def_val in args if def_val is not None])
		req_arg_count = total_arg_count - opt_arg_count
		if opt_arg_count:
			write_line(src, "ASSERT(%d <= nargs && nargs <= %d);" % (req_arg_count, total_arg_count))
		else:
			write_line(src, "ASSERT(nargs == %d);" % total_arg_count)

		def required_arg_str(stack_index, type):
			if type in ('int'):
				return 'luaL_checkinteger(L, %d)' % stack_index
			elif type in ('ID', 'uint32_t'):
				return 'luaL_checkunsigned(L, %d)' % stack_index
			elif type in ('float', 'double'):
				return 'luaL_checknumber(L, %d)' % stack_index
			elif type in ('vec2_t'):
				return 'Vec2(luaL_checkvec(L, %d))' % stack_index
			elif type in ('color_t'):
				return 'RGB(luaL_checkunsigned(L, %d))' % stack_index
			elif type in ('bool'):
				return '(luaL_checkunsigned(L, %d) != 0)' % stack_index
			elif type in ('std::string'):
				return 'GetString(L, %d)' % stack_index
			else:
				dofail()

		def opt_arg_str(stack_index, type, def_value):
			if type in ('int'):
				return 'luaL_optinteger(L, %d, %s)' % (stack_index, def_value)
			elif type in ('ID', 'uint32_t'):
				return 'luaL_optunsigned(L, %d, %s)' % (stack_index, def_value)
			elif type in ('float', 'double'):
				return 'luaL_optnumber(L, %d, %s)' % (stack_index, def_value)
			elif type in ('vec2_t'):
				return 'Vec2(luaL_optvec(L, %d, %s))' % (stack_index, def_value)
			elif type in ('color_t'):
				return 'RGB(luaL_optunsigned(L, %d, %s))' % (stack_index, def_value)
			elif type in ('bool'):
				return '(luaL_optunsigned(L, %d) != 0)' % stack_index
			elif type in ('std::string'):
				return 'OptString(L, %d)' % stack_index
			else:
				dofail()

		for i, (arg_type, arg_name, arg_def_value) in enumerate(args):
			write_line(src, "%s arg_%s = %s;" % (
				arg_type, arg_name, 
				required_arg_str(i+1, arg_type) if i < req_arg_count else opt_arg_str(i+1, arg_type, arg_def_value)
			))

	else:
		write_line(src, "ASSERT(nargs == 0);")
	# invoke function
	src.write("\t")
	if result_type != 'void':
		src.write("%s result = " % result_type)
	src.write("%s(%s);\n" % (command, ','.join("arg_"+arg_name for _,arg_name,_ in args)))
	# push results
	num_results = 0
	if result_type in ('int'):
		num_results = 1
		write_line(src, "lua_pushinteger(L, result);")
	elif result_type in ('uint32_t', 'ID'):
		num_results = 1
		write_line(src, "lua_pushunsigned(L, result);")
	elif result_type in ('bool'):
		num_results = 1
		write_line(src, "lua_pushboolean(L, result);")
	elif result_type in ('float', 'double'):
		num_results = 1
		write_line(src, "lua_pushnumber(L, result);")
	elif result_type in ("vec2_t"):
		num_results = 1
		write_line(src, "lua_pushvec(L, result.x, result.y);")
	elif result_type in ("color_t"):
		num_results = 1
		write_line(src, "lua_pushunsigned(L, result.RGB());")
	elif result_type in ('std::string'):
		num_results = 1
		write_line(src, 'lua_pushstring(L, result.c_str());')
	elif result_type !='void':
		fail("Unknown return type: %s" % result_type)
	write_line(src, "return %d;"  % num_results)
	src.write("}\n\n")

def write_line(src, line):
	src.write('\t')
	src.write(line)
	src.write('\n')


def unpack_signature(sig):
	# parse the signature string
	def dofail():
		fail("Bad Signature: %s" % sig)
	first_space = sig.find(' ')
	first_paren = sig.find('(')
	last_paren = sig.rfind(')')
	if first_space == -1 or first_paren == -1 or last_paren == -1:
		dofail()
	if first_space >= first_paren or first_paren >= last_paren:
		dofail()
	return_value = sig[0:first_space]
	name = sig[first_space+1:first_paren]
	arg_sig = sig[first_paren+1:last_paren].strip()
	if not return_value or not name:
		dofail()
	tokens = ( arg.strip() for arg in arg_sig.split(',') if arg.strip() )
	def unpack_arg_token(sig):
		first_space = sig.find(' ')
		if first_space == -1:
			dofail()
		type = sig[0:first_space].strip()
		name = sig[first_space+1:].strip()
		if not type or not name:
			dofail()
		if '=' in name:
			idx = name.find('=')
			actual_name = name[0:idx].strip()
			default_value = name[idx+1:].strip()
			if not actual_name or not default_value:
				dofail()
			return (type,actual_name,default_value)
		else:
			return (type,name,None)
	args = tuple( unpack_arg_token(token) for token in tokens )
	# validate that opt args are on the end
	seen_opt_arg = False
	for _,_,def_val in args:
		if def_val is not None:
			seen_opt_arg = True
		elif seen_opt_arg:
			dofail()
	return (return_value, name, args)

def fail(msg):
	print msg
	traceback.print_stack()
	exit(-1)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print "usage: write_bindings [output.cpp]"
		exit(-1)
	main()


