/**
 * OpenGL 3 - Example 17
 *
 * @author	Norbert Nopper norbert@nopper.tv
 *
 * Homepage: http://nopper.tv
 *
 * Copyright Norbert Nopper
 */

#include <stdio.h>

#include "GL/glus.h"

/**
 * Properties of the light.
 */
struct LightProperties
{
    GLfloat direction[3];
    GLfloat ambientColor[4];
    GLfloat diffuseColor[4];
    GLfloat specularColor[4];
};

/**
 * Properties of the material, basically all the color factors without the emissive color component.
 */
struct MaterialProperties
{
    GLfloat ambientColor[4];
    GLfloat diffuseColor[4];
    GLfloat specularColor[4];
    GLfloat specularExponent;
};

/**
 * Locations for the light properties.
 */
struct LightLocations
{
    GLint directionLocation;
    GLint ambientColorLocation;
    GLint diffuseColorLocation;
    GLint specularColorLocation;
};

/**
 * Locations for the material properties.
 */
struct MaterialLocations
{
    GLint ambientColorLocation;
    GLint diffuseColorLocation;
    GLint specularColorLocation;
    GLint specularExponentLocation;
};

static GLfloat g_viewMatrix[16];

static GLUSshaderprogram g_program;

/**
 * The location of the projection matrix.
 */
static GLint g_projectionMatrixLocation;

/**
 * The location of the view matrix.
 */
static GLint g_viewMatrixLocation;

/**
 * The location of the model matrix.
 */
static GLint g_modelMatrixLocation;

static GLint g_normalMatrixLocation;

static GLint g_vertexLocation;

static GLint g_normalLocation;

/**
 * The location of the clipping plane in the shader.
 */
static GLint g_planeLocation;

/**
 * The locations for the light properties.
 */
static struct LightLocations g_light;

/**
 * The locations for the material properties.
 */
static struct MaterialLocations g_material;


static GLuint g_verticesVBO;

static GLuint g_normalsVBO;

static GLuint g_vao;

static GLuint g_numberVertices;

GLUSboolean init(GLUSvoid)
{
    // This is a white light.
    struct LightProperties light = { { 1.0f, 1.0f, 1.0f }, { 0.3f, 0.3f, 0.3f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };

    // Blue color material with white specular color.
    struct MaterialProperties material = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 20.0f };

    // The point and the normal define the plane, which is calculated later in the code:
    // This point is just below the top of the cap.
    GLfloat pointOnPlane[4] = {0.0f, 2.5f, 0.0f, 1.0f};
    // The vector directs to the bottom, which will be later visible.
    GLfloat normalOfPLane[3] = {0.0f, -1.0f, 0.0f};
    GLfloat plane[4];

    GLUStextfile vertexSource;
    GLUStextfile fragmentSource;

    GLUSshape wavefrontObj;

    glusLoadTextFile("../Example17/shader/phong_clip.vert.glsl", &vertexSource);
    glusLoadTextFile("../Example17/shader/phong_clip.frag.glsl", &fragmentSource);

    glusBuildProgramFromSource(&g_program, (const GLUSchar**) &vertexSource.text, 0, 0, 0, (const GLUSchar**) &fragmentSource.text);

    glusDestroyTextFile(&vertexSource);
    glusDestroyTextFile(&fragmentSource);

    //

    g_projectionMatrixLocation = glGetUniformLocation(g_program.program, "u_projectionMatrix");
    g_viewMatrixLocation = glGetUniformLocation(g_program.program, "u_viewMatrix");
    g_modelMatrixLocation = glGetUniformLocation(g_program.program, "u_modelMatrix");
    g_normalMatrixLocation = glGetUniformLocation(g_program.program, "u_normalMatrix");

    g_light.directionLocation = glGetUniformLocation(g_program.program, "u_light.direction");
    g_light.ambientColorLocation = glGetUniformLocation(g_program.program, "u_light.ambientColor");
    g_light.diffuseColorLocation = glGetUniformLocation(g_program.program, "u_light.diffuseColor");
    g_light.specularColorLocation = glGetUniformLocation(g_program.program, "u_light.specularColor");

    g_material.ambientColorLocation = glGetUniformLocation(g_program.program, "u_material.ambientColor");
    g_material.diffuseColorLocation = glGetUniformLocation(g_program.program, "u_material.diffuseColor");
    g_material.specularColorLocation = glGetUniformLocation(g_program.program, "u_material.specularColor");
    g_material.specularExponentLocation = glGetUniformLocation(g_program.program, "u_material.specularExponent");

    g_planeLocation = glGetUniformLocation(g_program.program, "u_plane");

    g_vertexLocation = glGetAttribLocation(g_program.program, "a_vertex");
    g_normalLocation = glGetAttribLocation(g_program.program, "a_normal");

    //

    // Use a helper function to load an wavefront object file.
    glusLoadObjFile("teapot.obj", &wavefrontObj);

    g_numberVertices = wavefrontObj.numberVertices;

    glGenBuffers(1, &g_verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, wavefrontObj.numberVertices * 4 * sizeof(GLfloat), (GLfloat*) wavefrontObj.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_normalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_normalsVBO);
    glBufferData(GL_ARRAY_BUFFER, wavefrontObj.numberVertices * 3 * sizeof(GLfloat), (GLfloat*) wavefrontObj.normals, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glusDestroyShapef(&wavefrontObj);

    //

    glUseProgram(g_program.program);

    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
    glVertexAttribPointer(g_vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(g_vertexLocation);

    glBindBuffer(GL_ARRAY_BUFFER, g_normalsVBO);
    glVertexAttribPointer(g_normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(g_normalLocation);

    //

    glusLookAtf(g_viewMatrix, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glUniformMatrix4fv(g_viewMatrixLocation, 1, GL_FALSE, g_viewMatrix);

    //

    // Calculate the plane ...
    glusPlaneCreatef(plane, pointOnPlane, normalOfPLane);

    // ... and pass to the shader.
    glUniform4fv(g_planeLocation, 1, plane);

    //

    glusVector3Normalizef(light.direction);

    // Transform light to camera space, as it is currently in world space.
    glusMatrix4x4MultiplyVector3f(light.direction, g_viewMatrix, light.direction);

    // Set up light ...
    glUniform3fv(g_light.directionLocation, 1, light.direction);
    glUniform4fv(g_light.ambientColorLocation, 1, light.ambientColor);
    glUniform4fv(g_light.diffuseColorLocation, 1, light.diffuseColor);
    glUniform4fv(g_light.specularColorLocation, 1, light.specularColor);

    // ... and material values.
    glUniform4fv(g_material.ambientColorLocation, 1, material.ambientColor);
    glUniform4fv(g_material.diffuseColorLocation, 1, material.diffuseColor);
    glUniform4fv(g_material.specularColorLocation, 1, material.specularColor);
    glUniform1f(g_material.specularExponentLocation, material.specularExponent);

    //

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CLIP_DISTANCE0);

    return GLUS_TRUE;
}

GLUSvoid reshape(GLUSint width, GLUSint height)
{
    GLfloat projectionMatrix[16];

    glViewport(0, 0, width, height);

    glusPerspectivef(projectionMatrix, 40.0f, (GLfloat) width / (GLfloat) height, 1.0f, 100.0f);

    // Just pass the projection matrix. The final matrix is calculated in the shader.
    glUniformMatrix4fv(g_projectionMatrixLocation, 1, GL_FALSE, projectionMatrix);
}

GLUSboolean update(GLUSfloat time)
{
    static GLfloat angle = 0.0f;

    GLfloat modelMatrix[16];
    GLfloat modelViewMatrix[16];
    GLfloat normalMatrix[9];

    glusMatrix4x4Identityf(modelMatrix);

    glusMatrix4x4RotateRyf(modelMatrix, angle);

    glusMatrix4x4Multiplyf(modelViewMatrix, g_viewMatrix, modelMatrix);
    glusMatrix4x4ExtractMatrix3x3f(normalMatrix, modelViewMatrix);

    glUniformMatrix4fv(g_modelMatrixLocation, 1, GL_FALSE, modelMatrix);
    glUniformMatrix3fv(g_normalMatrixLocation, 1, GL_FALSE, normalMatrix);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, g_numberVertices);

    angle += 30.0f * time;

    return GLUS_TRUE;
}

GLUSvoid terminate(GLUSvoid)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (g_verticesVBO)
    {
        glDeleteBuffers(1, &g_verticesVBO);

        g_verticesVBO = 0;
    }

    if (g_normalsVBO)
    {
        glDeleteBuffers(1, &g_normalsVBO);

        g_normalsVBO = 0;
    }

    glBindVertexArray(0);

    if (g_vao)
    {
        glDeleteVertexArrays(1, &g_vao);

        g_vao = 0;
    }

    glUseProgram(0);

    glusDestroyProgram(&g_program);
}

int main(int argc, char* argv[])
{
    glusInitFunc(init);

    glusReshapeFunc(reshape);

    glusUpdateFunc(update);

    glusTerminateFunc(terminate);

    glusPrepareContext(3, 2, GLUS_FORWARD_COMPATIBLE_BIT);

    glusPrepareMSAA(8);

    if (!glusCreateWindow("GLUS Example Window", 640, 480, 24, 0, GLUS_FALSE))
    {
        printf("Could not create window!\n");
        return -1;
    }

    glusRun();

    return 0;
}