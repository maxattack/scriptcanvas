import glob
import sys
import os
import os.path

# TODO: Integrate Unity's GLSL->GLSL Optimizer (https://github.com/aras-p/glsl-optimizer)

HEADER = '''// GENERATED AUTOMATICALLY, DO NOT EDIT BY HAND
#pragma once
#include "RenderSystem.h"

namespace Shader {

'''

def main():
	global write_depth
	path = sys.argv[1]
	src_dir = os.path.split(path)[0]
	if not os.path.isdir(src_dir): fail("Source Directory Not Found: " + src_dir)

	with open(path, 'w') as output:
		output.write(HEADER)
		for shader_path in glob.glob(os.path.join(src_dir, "*.glsl")):
			shader_name = os.path.split(shader_path)[1][:-5]
			with open(shader_path, 'r') as input:
				src_lines = list(shader_lines(input))
				attribs = []
				uniforms = []
				for line in src_lines:
					if line.startswith('attribute'):
						tokens = line.split()
						attribs.append((tokens[1], tokens[2][:-1]))
					elif line.startswith('uniform'):
						tokens = line.split()
						uniforms.append((tokens[1], tokens[2][:-1]))
			writeln(output, "struct %s_t {" % shader_name)
			write_depth += 1
			writeln(output, 'GLuint handle;')
			for _,name in attribs:
				writeln(output, 'GLuint %s;' % name)
			for _,name in uniforms:
				writeln(output, 'GLuint %s;' % name)
			writelb(output)
			writeln(output, 'void Initialize() {')
			write_depth += 1
			writeln(output, 'handle = RenderSystem::LoadShaderProgramFromLiteral(')
			write_depth += 1
			for line in src_lines:
				writeln(output, '"%s\\n"' % line)
			write_depth -= 1
			writeln(output, ');')
			writeln(output, 'glUseProgram(handle);')
			for _,name in attribs:
				writeln(output, '%s = glGetAttribLocation(handle, "%s");' % (name,name))
			for _,name in uniforms:
				writeln(output, '%s = glGetUniformLocation(handle, "%s");' % (name,name))
			write_depth -= 1
			writeln(output, '}')
			writelb(output)
			writeln(output, 'void Destroy() {')
			write_depth += 1
			# TODO!  Do we need to save vert/frag handles, too?
			writeln(output, '// TODO')
			write_depth -= 1
			writeln(output, '}')
			write_depth -= 1
			writelb(output)
			writeln(output, "};")
			writelb(output)

		writeln(output, '}')
		writelb(output)

write_depth = 0

def writeln(file, line):
	for x in xrange(write_depth):
		file.write('\t')
	file.write(line)
	file.write('\n')

def writelb(file):
	for x in xrange(write_depth):
		file.write('\t')
	file.write('\n')

def shader_lines(reader):
	# doesn't handle multiline comments :P
	stripped = (strip_line(line) for line in reader)
	return (line for line in stripped if line)

def strip_line(line):
	i = line.find('//')
	return line.strip() if i == -1 else line[0:i].strip()

def fail(msg):
	print 'ERROR: ' + msg
	exit(-1)



if __name__ == '__main__':
	if len(sys.argv) < 2:
		print 'usage write_glsl_bindings path/to/src'
		exit(-1)
	main()