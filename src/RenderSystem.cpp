// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "RenderSystem.h"
#include <algorithm>
#include <functional>

void RenderSystem::Initialize() {
    // Initialize Display
    glfwInit();
    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    if (glfwOpenWindow(800, 800,8, 8, 8, 8, 8, 0, GLFW_WINDOW) != GL_TRUE) {
        printf("[ERROR] Window Failed to Open.\n");
        // TODO: return error code
        exit(-1);
    }
    glfwSetWindowTitle("Bubbles!?");
    int w,h;
    glfwGetWindowSize(&w, &h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::Render(CommandBuffer *vbuf) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::Destroy() {
    glfwTerminate();
}

GLuint RenderSystem::LoadShaderProgramFromFile(const char* filename) {
    GLint cnt = 0;
    GLchar* buf = 0;
    {
        FILE* file = fopen(filename, "r");
        if (!file) { 
            return 0; 
        }
        fseek(file, 0, SEEK_END);
        cnt = ftell(file);
        rewind(file);
        buf = new char[cnt+1];
        if (buf) {
          unsigned acnt = fread(buf, 1, cnt, file);
          fclose(file);
          if (cnt != acnt) {
            delete[] buf;
            return 0;
          } else {
            buf[cnt] = 0;
          }
        }
    }
    GLuint result = LoadShaderProgramFromLiteral(buf);
    delete[] buf;
    return result;

}

GLuint RenderSystem::LoadShaderProgramFromLiteral(const char* buf, int cnt) {
    if (cnt == 0) {
        cnt = strlen(buf);
    }  
    GLuint prog = glCreateProgram();
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar sCondVert[] = "#define VERTEX\n";
    const GLchar sCondFrag[] = "#define FRAGMENT\n";
    const GLchar *vsrc[] = { sCondVert, buf };
    const GLchar *fsrc[] = { sCondFrag, buf };
    GLint vcnt[] = { static_cast<GLint>(strlen(sCondVert)), cnt };
    GLint fcnt[] = { static_cast<GLint>(strlen(sCondFrag)), cnt };
    glShaderSource(vert, 2, vsrc, vcnt);
    glShaderSource(frag, 2, fsrc, fcnt);
    glCompileShader(vert);
    glCompileShader(frag);
    GLint result;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLchar buf[256];
        int len;
        glGetShaderInfoLog(vert, 256, &len, buf);
        printf("%s\n", buf);
        return 0;
    }

    glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        GLchar buf[256];
        int len;
        glGetShaderInfoLog(frag, 256, &len, buf);
        printf("%s\n", buf);
        return 0;
    }

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    return prog;
}