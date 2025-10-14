
#ifdef _WIN32
#include <glad/glad.h>

#elif __APPLE__
#include <OpenGL/gl3.h>

#elif __linux__
#include <glad/glad.h>
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
#include "texture.h"

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

class AxisRotation;
using AxisRotationPtr = std::shared_ptr<AxisRotation>;
class AxisRotation : public Engine
{
    TransformPtr m_trf;
    float m_angular_velocity;
    float m_scale;
    float m_angle;

protected:
    AxisRotation(TransformPtr trf, float velocity, float scale)
        : m_trf(trf),
        m_angular_velocity(velocity),
        m_scale(scale),
        m_angle(0.0f)
    {
    }
public:
    static AxisRotationPtr Make(TransformPtr trf, float velocity, float scale)
    {
        return AxisRotationPtr(new AxisRotation(trf, velocity, scale));
    }
    virtual void Update(float dt)
    {
        m_angle += -dt * m_angular_velocity;

		m_trf->LoadIdentity();
		m_trf->Scale(m_scale, m_scale, 1.0f);
        m_trf->Rotate(m_angle, 0, 0, 1);
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
  // enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // create objects 
  camera = Camera2D::Make(-1,1,-1,1);
  TexturePtr texture_space = Texture::Make("face", "images/space.jpg");


  ////espaco
  auto espaco_trf = Transform::Make();
  espaco_trf->Scale(1.7f, 1.0f, 1.0f);

  ////sol
  auto sun_trf = Transform::Make();
  sun_trf->Translate(0.0f, 0.0f, 1.0f);
  sun_trf->Scale(0.5f, 0.5f, 0.0f);

  //Mercury
  auto mercury_orbit_trf = Transform::Make();
  auto mercury_trf = Transform::Make();
  mercury_trf->Scale(0.15f, 0.15f, 1.0f);

  ////terra
  auto earth_orbit_trf = Transform::Make();
  auto earth_rotation_trf = Transform::Make();
  auto earth_trf = Transform::Make();
 
 

  ////lua
  auto moon_orbit_trf = Transform::Make();

  auto moon_trf = Transform::Make();
  moon_trf->Scale(0.25f, 0.25f, 1.0f);
  //
  
  ////texturas
  TexturePtr sun_text = Texture::Make("face", "images/sun.jpg");
  TexturePtr earth_text = Texture::Make("face", "images/earth.jpg");
  TexturePtr mercury_text = Texture::Make("face", "images/mercury.jpg");
  TexturePtr moon_text = Texture::Make("face", "images/moon.jpeg");

  //hierarquia da cena
  //Espa�o->Sol->Terra->Lua

  DiskPtr disk_moon = Disk::Make(5000);


  disk_moon->set_text_name("face");
  auto moon = Node::Make(moon_trf, { moon_text }, { disk_moon });

  auto moon_orbit = Node::Make(moon_orbit_trf, {}, {}, { moon });

  DiskPtr disk_earth = Disk::Make(5000);
  disk_earth->set_text_name("face");
  auto earth = Node::Make(earth_trf, { earth_text }, { disk_earth }, { });
  auto earth_rotation = Node::Make(earth_rotation_trf, {}, {}, { earth,moon_orbit });

  auto earth_orbit = Node::Make(earth_orbit_trf, {}, {}, { earth_rotation});

  DiskPtr disk_mercury = Disk::Make(5000);
  disk_mercury->set_text_name("face");
  auto mercury = Node::Make(mercury_trf, { mercury_text }, { disk_mercury }, {});
  auto mercury_orbit = Node::Make(mercury_orbit_trf, {}, {}, { mercury });

  DiskPtr disk_sun = Disk::Make(5000);
  disk_sun->set_text_name("face");
  auto sun = Node::Make(sun_trf, { sun_text }, { disk_sun }, { earth_orbit, mercury_orbit }); //quadrado num intervalo de [0-1] em x e y
  
  int vp[ 4 ] ;
  glGetIntegerv (GL_VIEWPORT, vp);
  float w = (float) vp [2];
  float h = (float) vp [3];
  float xmax = 1, ymax = 1, xmin = -1, ymin = -1;

  auto dx = xmax - xmin;
  auto dy = ymax - ymin;
  if (w/h > dx/dy)
  {
    auto xc = (xmin + xmax)/2;
    xmin = xc - dx/2 * w/h;
    xmax = xc + dx/2 * w/h;
  }
  else
  {
    auto yc = (ymin + ymax)/2;
    ymin = yc - dy/2 * h/w;
    ymax = yc + dy/2 * h/w;
  }
  std::vector<glm::vec2> coord = {
    {xmin, ymin},
    {xmax ,ymin},
    {xmax , ymax},
    {xmin, ymax}
  };

  std::vector<glm::vec2> coordText = {
    {0.0f, 0.0f},
    {1.0f ,0.0f},
    {1.0f , 1.0f},
    {0.0f, 1.0f}
  };

  auto espaco = Node::Make(espaco_trf, { Color::Make(1.0f, 1.0f, 1.0f), texture_space }, { Quad::Make(coord, coordText) }); //quadrado num intervalo de [-1 - 0] em x e y
  auto shader = Shader::Make();
  shader->AttachVertexShader("shaders/2d/vertex.glsl");
  shader->AttachFragmentShader("shaders/2d/fragment.glsl");
  shader->Link();

  // build scene
  auto root = Node::Make(shader, {espaco, sun});
  scene = Scene::Make(root);
  scene->AddEngine(MoveAstro::Make(earth_orbit_trf, 7.27f, 1.5f));
  scene->AddEngine(MoveAstro::Make(moon_orbit_trf, 14.0f, 1.0f));
  scene->AddEngine(MoveAstro::Make(mercury_orbit_trf, 3.5f, 0.8f));
  scene->AddEngine(AxisRotation::Make(earth_rotation_trf, 20.f, 0.3f));
  
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
  #if defined(__linux__) && defined(__glad_h_)
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
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

