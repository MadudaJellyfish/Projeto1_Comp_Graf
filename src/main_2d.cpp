
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "scene.h"
#include "state.h"
#include "camera2d.h"
#include "color.h"
#include "transform.h"
#include "error.h"
#include "shader.h"
#include "shape.h"
#include "quad.h"
#include "disk.h"

#include <iostream>

static ScenePtr scene;
static CameraPtr camera;

class MoveAstro;
using MoveAstroPtr = std::shared_ptr<MoveAstro>;
class MoveAstro : public Engine
{
  TransformPtr m_trf;
  float m_angular_velocity;
  float m_radius;
  float m_angle;

protected:
    MoveAstro(TransformPtr trf, float velocity, float radius)
        : m_trf(trf),
        m_angular_velocity(velocity),
        m_radius(radius),
        m_angle(0.0f)
  {
  }
public:
  static MoveAstroPtr Make (TransformPtr trf, float velocity, float radius)
  {
    return MoveAstroPtr(new MoveAstro(trf, velocity, radius));
  }
  virtual void Update (float dt)
  {
	  m_angle += -dt * m_angular_velocity;

      m_trf->LoadIdentity();
      m_trf->Rotate(m_angle,0,0,1);
	  m_trf->Translate(m_radius, 0.0f, 0.0f);
  }

};

void rgb_scale(float r, float g, float b, float& r_out, float& g_out, float& b_out) 
{
	r_out = r / 255.0f;
	g_out = g / 255.0f;
	b_out = b / 255.0f;
    return;
}

static void initialize (void)
{
  // set background color: white 
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);

  // create objects
  camera = Camera2D::Make(-8,8,-8,8);

  //sol
  auto sun_trf = Transform::Make();


  //terra
  auto earth_orbit_trf = Transform::Make();


  auto earth_trf = Transform::Make();
  earth_trf->Scale(0.4f, 0.4f, 1.0f);
  //earth_trf->Translate(10.0f, 0.0f, 0.0f);
  
  

  //lua
  auto moon_orbit_trf = Transform::Make();

  auto moon_trf = Transform::Make();
  moon_trf->Scale(0.3f, 0.3f, 1.0f);
  //moon_trf->Translate(3.0f, 0.0f, 0.0f);
  
  
  //hierarquia da cena
  //Sol->Terra->Lua

  float r, g, b = 0;
  rgb_scale(173, 199, 239, r, g, b);
  auto moon = Node::Make(moon_trf, { Color::Make(r, g, b) }, { Disk::Make(3600) });
  auto moon_orbit = Node::Make(moon_orbit_trf, {}, {}, { moon });

  rgb_scale(74, 118, 183, r, g, b);

  auto earth = Node::Make(earth_trf, { Color::Make(r,g,b) }, { Disk::Make(3600) }, { moon_orbit });
  auto earth_orbit = Node::Make(earth_orbit_trf, {}, {}, { earth });

  rgb_scale(255, 237, 120, r, g, b);
  auto sun = Node::Make(sun_trf, { Color::Make(r,g,b) }, { Disk::Make(3600) }, { earth_orbit });

  auto shader = Shader::Make();
  shader->AttachVertexShader("shaders/2d/vertex.glsl");
  shader->AttachFragmentShader("shaders/2d/fragment.glsl");
  shader->Link();

  // build scene
  auto root = Node::Make(shader, {sun});
  scene = Scene::Make(root);
  scene->AddEngine(MoveAstro::Make(earth_orbit_trf, 7.27f, 5));
  scene->AddEngine(MoveAstro::Make(moon_orbit_trf, 14.0f, 2));
  

}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(600, 400, "Window title", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback

    glfwMakeContextCurrent(win);
#ifdef _WIN32
    if (!gladLoadGL()) {
        printf("Failed to initialize GLAD OpenGL context\n");
        exit(1);
    }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

