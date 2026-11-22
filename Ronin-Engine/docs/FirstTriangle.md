# OpenGL: Rendering My First Triangle

## 1. Understanding the CPU and GPU

The CPU and GPU are separate processing units that work together.

**CPU (Central Processing Unit)**
- Runs your C++ program.
- Prepares vertex data.
- Sends rendering commands through OpenGL.

**GPU (Graphics Processing Unit)**
- Processes vertex data.
- Executes shaders.
- Performs graphics calculations.
- Produces the image that gets displayed on the screen.

### How do they communicate?

The CPU initially stores your vertex data in CPU-accessible memory.

The GPU needs access to that data to perform graphics operations.

OpenGL provides an interface for requesting these operations through the graphics driver.

On a dedicated GPU, such as my RTX 5060, the GPU has its own dedicated VRAM.

**Important:** OpenGL is an API, not the GPU itself.

- OpenGL defines graphics functions and their behavior.
- The graphics driver implements OpenGL for the hardware.
- The GPU executes the graphics work.

---

## 2. Libraries Used in Ronin Engine

### GLFW (Graphics Library Framework)

GLFW handles window management and input.

It helps you:
- Create a window.
- Create an OpenGL context.
- Handle keyboard and mouse input.
- Manage the window's lifecycle.

**What is an OpenGL context?**

An OpenGL context provides the environment and state needed for an application to issue OpenGL commands.

It connects your application to an OpenGL implementation through the graphics driver.

### GLAD

GLAD is an OpenGL function loader.

It helps you:
- Retrieve OpenGL function pointers.
- Load the OpenGL functions supported by your graphics driver.
- Make those functions accessible to your C++ program.

Example:

```cpp
gladLoadGL(glfwGetProcAddress);
```

GLFW provides the function used to retrieve OpenGL function addresses. GLAD uses those addresses to make the OpenGL functions available.

**Remember:**
- GLFW handles the window and context.
- GLAD loads OpenGL functions.
- OpenGL handles graphics operations.

---

## 3. Understanding Vertex Data

A vertex represents a point in space.

In a 3D coordinate system, each vertex has three coordinates:

- X: Horizontal position.
- Y: Vertical position.
- Z: Depth position.

Example:

```cpp
float vertex[] = {
    0.5f, 0.2f, 0.0f
};
```

This represents one vertex at:

`(0.5, 0.2, 0.0)`

### Why does a triangle have nine values?

A triangle needs three vertices.

Each vertex has three coordinates.

3 vertices × 3 coordinates = 9 values.

Example:

```cpp
float vertices[] = {
    // Vertex 1
    -0.5f, -0.5f, 0.0f,

    // Vertex 2
     0.5f, -0.5f, 0.0f,

    // Vertex 3
     0.0f,  0.5f, 0.0f
};
```

All three Z coordinates are zero because the triangle lies on the XY plane.

---

## 4. Normalized Device Coordinates (NDC)

OpenGL uses Normalized Device Coordinates to represent positions in the visible region after vertex processing.

The coordinate ranges are:

- X: -1.0 to +1.0
- Y: -1.0 to +1.0
- Z: -1.0 to +1.0

The center is `(0, 0, 0)`.

The edges of the visible region reach -1.0 and +1.0 along each axis.

This means that coordinates such as `(-0.5, -0.5, 0.0)` describe positions within the visible region.

---

## 5. Vertex Buffer Objects (VBOs)

A VBO is a buffer object managed by OpenGL that stores vertex data.

Our original C++ array lives in CPU-accessible memory.

We use a VBO to provide OpenGL with a buffer containing that data.

### Step 1: Generate a buffer

```cpp
unsigned int VBO;

glGenBuffers(1, &VBO);
```

- `unsigned int VBO` declares a variable that stores the buffer's ID.
- `glGenBuffers()` generates buffer object IDs.
- `1` means generate one buffer.
- `&VBO` gives OpenGL the address where it should write the ID.

### Step 2: Bind the buffer

```cpp
glBindBuffer(GL_ARRAY_BUFFER, VBO);
```

Binding tells OpenGL which buffer object you want to work with.

`GL_ARRAY_BUFFER` specifies the buffer target.

### Step 3: Upload the vertex data

```cpp
glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertices),
    vertices,
    GL_STATIC_DRAW
);
```

This allocates storage for the currently bound buffer and copies the supplied vertex data into it.

| Argument | Purpose |
|---|---|
| `GL_ARRAY_BUFFER` | Specifies the buffer target |
| `sizeof(vertices)` | Specifies the data size in bytes |
| `vertices` | Provides the source data |
| `GL_STATIC_DRAW` | Indicates the intended usage pattern |

**Important distinction:**

- `vertices` contains the original C++ data.
- `VBO` identifies the OpenGL buffer.
- `glBufferData()` copies the data into the buffer.

---

## 6. Vertex Array Objects (VAOs)

A VAO stores vertex input configuration.

It remembers how OpenGL should interpret the vertex data associated with vertex attributes.

For example, it remembers:
- Which vertex attributes are enabled.
- Which buffers provide the vertex data.
- How the vertex data is arranged.

### Creating a VAO

```cpp
unsigned int VAO;

glGenVertexArrays(1, &VAO);

glBindVertexArray(VAO);
```

### VBO vs VAO

**VBO:**
Stores the vertex data.

**VAO:**
Stores the configuration describing how OpenGL should read that vertex data.

Think of them as two separate responsibilities.

---

## 7. Describing Vertex Attributes

OpenGL needs to know how your vertex data is arranged.

We use `glVertexAttribPointer()` to describe that layout.

```cpp
glVertexAttribPointer(
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    3 * sizeof(float),
    (void*)0
);

glEnableVertexAttribArray(0);
```

### Understanding the arguments

| Argument | Meaning |
|---|---|
| `0` | Vertex attribute location |
| `3` | Number of components per vertex |
| `GL_FLOAT` | Data type |
| `GL_FALSE` | Do not normalize the values |
| `3 * sizeof(float)` | Stride, distance between consecutive vertices |
| `(void*)0` | Offset, start reading at the beginning of the buffer |

### What is a stride?

Stride tells OpenGL how many bytes to move forward to find the next vertex's data.

Our vertices contain three floats.

Therefore:

`3 * sizeof(float)`

### What does glEnableVertexAttribArray() do?

It enables the vertex attribute at the specified location.

Without enabling it, OpenGL won't use that vertex attribute during vertex processing.

---

## 8. The Graphics Pipeline

The graphics pipeline is the sequence of operations used to turn geometry into an image.

For our first triangle, we'll focus on these stages:

1. **Vertex Data:** Supplies the triangle's coordinates.
2. **Vertex Shader:** Processes each vertex.
3. **Primitive Assembly:** Groups vertices into a triangle.
4. **Rasterization:** Determines which screen locations the triangle covers.
5. **Fragment Shader:** Determines the output color of fragments.
6. **Framebuffer:** Receives the resulting pixels for display.

### Vertex Shader

A vertex shader is a small program written in GLSL (OpenGL Shading Language).

It processes individual vertices and produces their positions for the next stages of the pipeline.

### Fragment Shader

A fragment shader determines the output values, commonly including color, for fragments produced during rasterization.

### Shader Program

A shader program is created by compiling shader source code and linking the compiled shaders together.

Our first triangle will use:
- One vertex shader.
- One fragment shader.

---

## 9. Drawing the Triangle

Once our vertex data and shader program are ready, we can issue a drawing command.

```cpp
glUseProgram(shaderProgram);

glBindVertexArray(VAO);

glDrawArrays(GL_TRIANGLES, 0, 3);
```

### Understanding glDrawArrays()

| Argument | Meaning |
|---|---|
| `GL_TRIANGLES` | Interpret every group of three vertices as a triangle |
| `0` | Start with the first vertex |
| `3` | Draw three vertices |

This tells OpenGL to process the three vertices as one triangle.

---

## 10. First Triangle: Learning Roadmap

Learn these concepts in order.

1. **Vertex Data:** Understand how vertices define geometry.
2. **VBO:** Understand how vertex data gets stored in an OpenGL buffer.
3. **VAO:** Understand how OpenGL remembers vertex input configuration.
4. **Vertex Attributes:** Learn how OpenGL interprets the data.
5. **Vertex Shaders:** Learn GLSL and vertex processing.
6. **Fragment Shaders:** Learn how fragments receive their output colors.
7. **Shader Programs:** Learn how shaders are compiled and linked.
8. **Drawing Commands:** Use `glDrawArrays()` to render the triangle.

**Goal:** Render your first triangle in Ronin Engine and understand what every major part of the process does.