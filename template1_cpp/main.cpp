#include "common.h"
#include "Image.h"
#include "Player.h"
#include <unistd.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <vector>

constexpr GLsizei WINDOW_WIDTH = tileSize * 55, WINDOW_HEIGHT = tileSize * 30;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  switch (key)
  {
  case GLFW_KEY_ESCAPE:
    if (action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
  default:
    if (action == GLFW_PRESS)
      Input.keys[key] = true;
    else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
  }
}

void processPlayerMovement(Player &player, std::vector<std::string> map, GLfloat frame) {
  if (Input.keys[GLFW_KEY_W]) {
    player.ProcessInput(MovementDir::UP, map, frame);
  } else if (Input.keys[GLFW_KEY_S]) {
    player.ProcessInput(MovementDir::DOWN, map, frame);
  }
  if (Input.keys[GLFW_KEY_A]) {
    player.ProcessInput(MovementDir::LEFT, map, frame);
  } else if (Input.keys[GLFW_KEY_D]) {
    player.ProcessInput(MovementDir::RIGHT, map, frame);
  }
}

int lifes = 3;
bool tileWithTrap(Player &player, std::vector<std::string> &map, Trap &trap, Image &screen, Image &wall) {
  Point coords = player.Place();
  int x = (coords.x + tileSize / 2) / tileSize;
  int y = (coords.y + tileSize / 2) / tileSize;
  static int wait_lifes = 5;
  if (map[y][x] == ' ' || trap.isActivated() && map[y][x] == 'T') {
    if (lifes == 0) {
      return true;
    } else {
      if (wait_lifes == 0) {
        lifes--;
        if (lifes == 2) {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screen.PutPixel(WINDOW_WIDTH - tileSize * 4 + k, WINDOW_HEIGHT - tileSize + l, wall.GetPixel(k + 13 * tileSize, l));
            }
          }
        } else if (lifes == 1) {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screen.PutPixel(WINDOW_WIDTH - tileSize * 3 + k, WINDOW_HEIGHT - tileSize + l, wall.GetPixel(k + 13 * tileSize, l));
            }
          }
        }
        wait_lifes = 5;
      } else {
        wait_lifes--;
      }
      return false;
    }
  }
  return false;
}

void drawWinPicture(Image &screen) {
  Image win("./resources/win2.png");
  Pixel pix = {0, 0, 0, 0};
  for (int j = 0; j < WINDOW_HEIGHT; ++j) {
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
      screen.PutPixel(i, j, pix);
    }
  }
  for (int j = 0; j < win.Height(); ++j) {
    for (int i = 0; i < win.Width(); ++i) {
      Point shift = {.x = 780, .y = 310};
      screen.PutPixel(shift.x + i, shift.y + j, win.GetPixel(i, win.Height() - j));
    }
  }
}

bool nextLevel(Player &player, std::vector<std::string> &map) {
  Point coords = player.Place();
  int x = (coords.x + tileSize / 2) / tileSize;
  int y = (coords.y + tileSize / 2) / tileSize;
  if (map[y][x] == 'X') {
    return true;
  }
  return false;
}

void drawLosePicture(Image &screen) {
  Image lose("./resources/lose.png");
  for (int j = 0; j < WINDOW_HEIGHT; ++j) {
    for (int i = 0; i < WINDOW_WIDTH; ++i) {
      Point shift = {.x = 80, .y = 60};
      screen.PutPixel(i, j, lose.GetPixel(shift.x + i, shift.y + WINDOW_HEIGHT - j));
    }
  }
}

void changeTrapPic(Image &screen, Trap &trap, std::vector<Point> coords, Image &fon, Image &wall1, Image &dungeon) {
  if (trap.isActivated() == 1) {
    for (Point coord : coords) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (dungeon.GetPixel(k + 4 * tileSize, l + tileSize * 15).a > 0.1) {
            screen.PutPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 5 * tileSize, l + tileSize * 15));
            screen.PutBufPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 5 * tileSize, l + tileSize * 15));
          }
        }
      }
    } 
  } else if (trap.isActivated() == 2) {
    for (Point coord : coords) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (dungeon.GetPixel(k + 4 * tileSize, l + tileSize * 15).a > 0.1) {
            screen.PutPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 4 * tileSize, l + tileSize * 15));
            screen.PutBufPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 4 * tileSize, l + tileSize * 15));
          }
        }
      }
    } 
  } else if (trap.isActivated() == 3){
    for (Point coord : coords) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (dungeon.GetPixel(k + 3 * tileSize, l + tileSize * 15).a > 0.1) {
            screen.PutPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 3 * tileSize, l + tileSize * 15));
            screen.PutBufPixel(coord.x * tileSize + k, coord.y * tileSize + l, dungeon.GetPixel(k + 3 * tileSize, l + tileSize * 15));
          }
        }
      }
    } 
  } else {
    for (Point coord : coords) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          screen.PutPixel(coord.x * tileSize + k, coord.y * tileSize + l, fon.GetPixel(k, l));
          screen.PutBufPixel(coord.x * tileSize + k, coord.y * tileSize + l, fon.GetPixel(k, l));
        }
      }
    }
  }
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }

  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

  return 0;
}

int main(int argc, char** argv)
{
  if(!glfwInit())
    return -1;

//  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
  if (window == nullptr)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window); 

  glfwSetKeyCallback        (window, OnKeyboardPressed);  
  glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
  glfwSetScrollCallback     (window, OnMouseScroll);

  if(initGL() != 0) 
    return -1;
  
  //Reset any OpenGL errors which could be present for some reason
  GLenum gl_error = glGetError();
  while (gl_error != GL_NO_ERROR)
    gl_error = glGetError();


  Image fon("./resources/fon.png");
  Image wall("./resources/wall.png");
  Image wall1("./resources/wall1.png");
  Image life("./resources/lifes.png");
  Image dungeon("./resources/Dungeon_Tileset.png");

  Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
    
  bool game_end = false;


  for (int z = 0; z < 2; ++z) {
    std::string path;
    if (z == 0) {
      path = "./resources/map.txt";
    } else if (z == 1) {
      drawWinPicture(screenBuffer);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
      glfwSwapBuffers(window);
      sleep(1.5);
      path = "./resources/mapwithot.txt";
    }
    std::ifstream file(path);
    std::vector<std::string> map(WINDOW_HEIGHT / tileSize);
    for (int i = 0; i < WINDOW_HEIGHT / tileSize; ++i) {
      for (int j = 0; j < WINDOW_WIDTH/ tileSize; ++j) {
        char temp = file.get();
        if (temp == '\n') {
          temp = file.get();
        }
        map[WINDOW_HEIGHT / tileSize - i - 1] += temp;
      }
    }
    
    Trap trap;
    std::vector<Point> trap_coord;
    Point start;

    for (int j = 0; j < WINDOW_HEIGHT / tileSize; ++j) {
      for (int i = 0; i < WINDOW_WIDTH / tileSize; ++i) {
        if (map[j][i] == '.' or map[j][i] == 'X') {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, fon.GetPixel(k, l));
            }
          }
        } else if (map[j][i] == '#') {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, wall.GetPixel(k + 13 * tileSize, l));
            }
          }
        } else if (map[j][i] == 'T') {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, fon.GetPixel(k, l));
            }
          }
          trap_coord.push_back({.x = i, .y = j});
        } else if (map[j][i] == '@') {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, fon.GetPixel(k, l));
            }
          }
          start = {.x = i * tileSize, .y = j * tileSize};
        } else if (map[j][i] == ' ') {
          for (int l = 0; l < tileSize; l++) {
            for (int k = 0; k < tileSize; k++) {
              screenBuffer.PutPixel(i * tileSize + k, j * tileSize + l, wall.GetPixel(k + 10 * tileSize, l + 5 * tileSize));
            }
          }
        } else {
          std::cerr << "wrong symbol in map: " << map[j][i] << std::endl;
        }
      }
    }
    Player player{start};
    if (lifes == 3) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (life.GetPixel(k + tileSize * 14, l + tileSize * 17).a > 0.1) {
            screenBuffer.PutPixel(WINDOW_WIDTH - tileSize * 2 + k, WINDOW_HEIGHT - tileSize + l, life.GetPixel(k + tileSize * 14, l + tileSize * 17));
          }
        }
      }
    }
    if (lifes >= 2) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (life.GetPixel(k + tileSize * 14, l + tileSize * 17).a > 0.1) {
            screenBuffer.PutPixel(WINDOW_WIDTH - tileSize * 3 + k, WINDOW_HEIGHT - tileSize + l, life.GetPixel(k + tileSize * 14, l + tileSize * 17));
          }
        }
      }
    }
    if (lifes >= 1) {
      for (int l = 0; l < tileSize; l++) {
        for (int k = 0; k < tileSize; k++) {
          if (life.GetPixel(k + tileSize * 14, l + tileSize * 17).a > 0.1) {
            screenBuffer.PutPixel(WINDOW_WIDTH - tileSize * 4 + k, WINDOW_HEIGHT - tileSize + l, life.GetPixel(k + tileSize * 14, l + tileSize * 17));
          }
        }
      }
    }
    screenBuffer.BufUpdate();
    player.Draw(screenBuffer);

    bool window_close = glfwWindowShouldClose(window);
    //game loop
    while (!window_close)
    {
      
      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
      glfwPollEvents();
      if (!game_end) {
        processPlayerMovement(player, map, lastFrame);
        game_end = tileWithTrap(player, map, trap, screenBuffer, wall);
        if (trap.checkStatus(lastFrame)) {
          changeTrapPic(screenBuffer, trap, trap_coord, fon, wall1, dungeon);
        }
        player.Draw(screenBuffer);
        if (game_end) {
          drawLosePicture(screenBuffer);
        }
      }
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

      glfwSwapBuffers(window);
      if (!game_end) {
        if (nextLevel(player, map)) {
          break;
        }
      }
      window_close = glfwWindowShouldClose(window);
    }
    if (game_end) {
      break;
    }
  }
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (!game_end) {
      drawWinPicture(screenBuffer);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
