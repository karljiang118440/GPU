/*
 * OpenGL ES 2.0 Tutorial 1
 *
 * Draws a simple triangle with basic vertex and pixel shaders.
 */

#include <GLES2/gl2.h>
#include <gc_vdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TUTORIAL_NAME "OpenGL ES 2.0 Tutorial 1"

// to hold vdk information.
vdkEGL egl;
int width  = 640;
int height = 480;
int posX   = -1;
int posY   = -1;
int samples = 0;
int frames = 0;

// Global Variables, attribute and uniform
GLint locVertices     = 0;
GLint locColors       = 0;
GLint locTransformMat = 0;

// Global Variables, shader handle and program handle
GLuint vertShaderNum  = 0;
GLuint pixelShaderNum = 0;
GLuint programHandle  = 0;

// Triangle Vertex positions.
const GLfloat vertices[3][2] = {
	{ -0.5f, -0.5f},
	{  0.0f,  0.5f},
	{  0.5f, -0.5f}
};

// Triangle Vertex colors.
const GLfloat color[3][3] = {
	{1.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 1.0f}
};

// Start with an identity matrix.
GLfloat transformMatrix[16] = 
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

/***************************************************************************************
***************************************************************************************/

void RenderInit()
{
	// Grab location of shader attributes.
	locVertices = glGetAttribLocation(programHandle, "my_Vertex");
	locColors   = glGetAttribLocation(programHandle, "my_Color");
	// Transform Matrix is uniform for all vertices here.
	locTransformMat = glGetUniformLocation(programHandle, "my_TransformMatrix");

	// enable vertex arrays to push the data.
	glEnableVertexAttribArray(locVertices);
	glEnableVertexAttribArray(locColors);

	// set data in the arrays.
	glVertexAttribPointer(locVertices, 2, GL_FLOAT, GL_FALSE, 0, &vertices[0][0]);
	glVertexAttribPointer(locColors, 3, GL_FLOAT, GL_FALSE, 0, &color[0][0]);
	glUniformMatrix4fv(locTransformMat, 1, GL_FALSE, transformMatrix);
}

// Actual rendering here.
void Render()
{
	static float angle = 0;

	// Clear background.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set up rotation matrix rotating by angle around y axis.
	transformMatrix[0] = transformMatrix[10] = (GLfloat)cos(angle);
	transformMatrix[2] = (GLfloat)sin(angle);
	transformMatrix[8] = -transformMatrix[2];
	angle += 0.1f;
	glUniformMatrix4fv(locTransformMat, 1, GL_FALSE, transformMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	// flush all commands.
	glFlush ();
	// swap display with drawn surface.
	vdkSwapEGL(&egl);
}

void RenderCleanup()
{
	// cleanup
	glDisableVertexAttribArray(locVertices);
	glDisableVertexAttribArray(locColors);
}

/***************************************************************************************
***************************************************************************************/

// Compile a vertex or pixel shader.
// returns 0: fail
//         1: success
int CompileShader(const char * FName, GLuint ShaderNum)
{
	FILE * fptr = NULL;
#ifdef UNDER_CE
	static wchar_t buffer[MAX_PATH + 1];
	int i = GetModuleFileName(NULL, buffer, MAX_PATH);
	while (buffer[i - 1] != L'\\') i--;
	while (*FName != '\0') buffer[i++] = (wchar_t)(*FName++);
	buffer[i] = L'\0';
	fptr = _wfopen(buffer, L"rb");
#else
	fptr = fopen(FName, "rb");
#endif
	if (fptr == NULL)
	{
		return 0;
	}

	int length;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	fseek(fptr, 0 ,SEEK_SET);

	char * shaderSource = (char*)malloc(sizeof (char) * length);
	if (shaderSource == NULL)
	{
		fprintf(stderr, "Out of memory.\n");
		return 0;
	}

	fread(shaderSource, length, 1, fptr);

	glShaderSource(ShaderNum, 1, (const char**)&shaderSource, &length);
	glCompileShader(ShaderNum);

	free(shaderSource);
	fclose(fptr);

	GLint compiled = 0;
	glGetShaderiv(ShaderNum, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		// Retrieve error buffer size.
		GLint errorBufSize, errorLength;
		glGetShaderiv(ShaderNum, GL_INFO_LOG_LENGTH, &errorBufSize);

		char * infoLog = (char*)malloc(errorBufSize * sizeof(char) + 1);
		if (!infoLog)
		{
			// Retrieve error.
			glGetShaderInfoLog(ShaderNum, errorBufSize, &errorLength, infoLog);
			infoLog[errorBufSize + 1] = '\0';
			fprintf(stderr, "%s\n", infoLog);

			free(infoLog);
		}
		return 0;
	}

	return 1;
}

/***************************************************************************************
***************************************************************************************/

// Wrapper to load vetex and pixel shader.
void LoadShaders(const char * vShaderFName, const char * pShaderFName)
{
	vertShaderNum = glCreateShader(GL_VERTEX_SHADER);
	pixelShaderNum = glCreateShader(GL_FRAGMENT_SHADER);

	if (CompileShader(vShaderFName, vertShaderNum) == 0)
	{
		return;
	}

	if (CompileShader(pShaderFName, pixelShaderNum) == 0)
	{
		return;
	}

	programHandle = glCreateProgram();

	glAttachShader(programHandle, vertShaderNum);
	glAttachShader(programHandle, pixelShaderNum);

	glLinkProgram(programHandle);
	// Check if linking succeeded.
	GLint linked = false;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		// Retrieve error buffer size.
		GLint errorBufSize, errorLength;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &errorBufSize);

		char * infoLog = (char*)malloc(errorBufSize * sizeof (char) + 1);
		if (!infoLog)
		{
			// Retrieve error.
			glGetProgramInfoLog(programHandle, errorBufSize, &errorLength, infoLog);
			infoLog[errorBufSize + 1] = '\0';
			fprintf(stderr, "%s", infoLog);

			free(infoLog);
		}

		return;
	}
	glUseProgram(programHandle);
}

// Cleanup the shaders.
void DestroyShaders()
{
	glDeleteShader(vertShaderNum);
	glDeleteShader(pixelShaderNum);
	glDeleteProgram(programHandle);
	glUseProgram(0);
}

/***************************************************************************************
***************************************************************************************/

void ParseCommandLine(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'x':
				posX   = atoi(argv[++i]);
				break;
			case 'y':
				posY   = atoi(argv[++i]);
				break;
			case 'w':
				width  = atoi(argv[++i]);
				break;
			case 'h':
				height = atoi(argv[++i]);
				break;
			case 's':
				samples = atoi(argv[++i]);
				break;
			case 'f':
				frames = atoi(argv[++i]);
				break;
			}
		}
	}
}

/***************************************************************************************
***************************************************************************************/

// Program entry.
int main(int argc, char** argv)
{
	bool pause = false;

	// EGL configuration - we use 24-bpp render target and a 16-bit Z buffer.
	EGLint configAttribs[] =
	{
		EGL_SAMPLES,      0,
		EGL_RED_SIZE,     8,
		EGL_GREEN_SIZE,   8,
		EGL_BLUE_SIZE,    8,
		EGL_ALPHA_SIZE,   EGL_DONT_CARE,
		EGL_DEPTH_SIZE,   16,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE,
	};

	EGLint attribListContext[] = 
	{
		// Needs to be set for es2.0 as default client version is es1.1.
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE 
	};

	// Parse the command line.
	ParseCommandLine(argc, argv);

	// Set multi-sampling.
	configAttribs[1] = samples;

	// Initialize VDK, EGL, and GLES.
	if (!vdkSetupEGL(posX, posY, width, height, configAttribs, NULL, attribListContext, &egl))
	{
		return 1;
	}

	// Set window title and show the window.
	vdkSetWindowTitle(egl.window, TUTORIAL_NAME);
	vdkShowWindow(egl.window);

	// load and compiler vertex/fragment shaders.
	LoadShaders("vs_es20t1.vert", "ps_es20t1.frag");

	if (programHandle != 0)
	{
		RenderInit();

		int frameCount = 0;
		unsigned int start = vdkGetTicks();

		// Main loop
		for (bool done = false; !done;)
		{
			// Get an event.
			vdkEvent event;
			if (vdkGetEvent(egl.window, &event))
			{
				// Test for Keyboard event.
				if ((event.type == VDK_KEYBOARD)
				&& event.data.keyboard.pressed
				)
				{
					// Test for key.
					switch (event.data.keyboard.scancode)
					{
					case VDK_SPACE:
						// Use SPACE to pause.
						pause = !pause;
						break;

					case VDK_ESCAPE:
						// Use ESCAPE to quit.
						done = true;
						break;
					default:
						break;
					}
				}

				// Test for Close event.
				else if (event.type == VDK_CLOSE)
				{
					done = true;
				}
			}
			else if (!pause)
			{
				// Render one frame if there is no event.
				Render();
				++ frameCount;

				if ((frames > 0) && (--frames == 0)) {
					done = true;
				}
			}
		}

		glFinish();
		unsigned int end = vdkGetTicks();
		float fps = frameCount / ((end - start) / 1000.0f);
		printf("%d frames in %d ticks -> %.3f fps\n", frameCount, end - start, fps);

		RenderCleanup();
	}

	// cleanup
	DestroyShaders();
	vdkFinishEGL(&egl);

	return 0;
}

