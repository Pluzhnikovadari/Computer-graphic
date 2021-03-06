#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <vector>
#include <iostream>

#define GLFW_DLL
#include <GLFW/glfw3.h>

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Trap {
  explicit Trap(GLfloat frames = 0.5)
      : frames_active(frames), past_frames(0), activated(false) {};
  bool checkStatus(GLfloat frame);
  int isActivated();
private:
  int activated = 0;
  GLfloat past_frames = 0;
  GLfloat frames_active = 0;
};

struct Player
{
  explicit Player(Point pos) :
                 coords(pos), old_coords(pos) {};

  Point Place() const { return coords; }
  bool Moved() const;
  void ChangePosition(Point pos);
  bool nearWall(MovementDir dir, std::vector<std::string> map, int i, int j) const;
  void ProcessInput(MovementDir dir, std::vector<std::string> map, GLfloat frame);
  void Draw(Image &screen);

  Point coords;
private:
  Point old_coords;
  Image skin = Image("./resources/1.png");
  int move_speed = 4;
  GLfloat past_frames = 0;
  GLfloat frames_active = 0.15;
  int cur_position = 0;

};

#endif //MAIN_PLAYER_H
