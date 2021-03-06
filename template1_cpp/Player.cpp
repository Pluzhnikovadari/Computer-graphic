#include "Player.h"


bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

int movement = 0;

bool Player::nearWall(MovementDir dir, std::vector<std::string> map, int i, int j) const {
  bool flag = true;
  switch(dir) {
    case MovementDir::UP:
      if (map[i + 1][j] == '#' || map[i + 1][j + 1] == '#') {
        flag = false;
      }
      break;
    case MovementDir::DOWN:
      if (map[i][j] == '#' || map[i][j + 1] == '#') {
        flag = false;
      }
      break;
    case MovementDir::LEFT:
      if (map[i][j] == '#' || map[i + 1][j] == '#') {
        flag = false;
      }
      break;
    case MovementDir::RIGHT:
      if (map[i][j + 1] == '#' || map[i + 1][j + 1] == '#') {
        flag = false;
      }
      break;
    default:
      break;
  }
  return flag;
}

void Player::ProcessInput(MovementDir dir, std::vector<std::string> map, GLfloat frame)
{
  int move_dist = move_speed * 1;
  switch(dir) {
    case MovementDir::UP:
      movement = 1;
      old_coords.y = coords.y;
      if (nearWall(dir, map, (coords.y + move_dist) / tileSize, coords.x / tileSize)) {
        coords.y += move_dist;
      }
      break;
    case MovementDir::DOWN:
      movement = 0;
      old_coords.y = coords.y;
      if (nearWall(dir, map, (coords.y - move_dist) / tileSize, coords.x / tileSize)) {
        coords.y -= move_dist;
      }
      break;
    case MovementDir::LEFT:
      movement = 2;
      old_coords.x = coords.x;
      if (nearWall(dir, map, coords.y / tileSize, (coords.x - move_dist) / tileSize)) {
        coords.x -= move_dist;
      }
      break;
    case MovementDir::RIGHT:
      movement = 3;
      old_coords.x = coords.x;
      if (nearWall(dir, map, coords.y / tileSize, (coords.x + move_dist) / tileSize)) {
        coords.x += move_dist;
      }
      break;
    default:
      break;
  }
  if (frame - past_frames > frames_active) {
    past_frames = frame;
    cur_position = (cur_position + 1) % 4;
  }
}


void Player::Draw(Image &screen)
{
  int img_size = tileSize / 2;
  if(Moved())
  {
    for(int y = 0; y <= img_size; ++y)
    {
      for(int x = 0; x <= img_size; ++x)
      {
        screen.PutPixel(x * 2 + old_coords.x, y * 2 + old_coords.y, screen.GetBufPixel(x * 2 + old_coords.x, y * 2 + old_coords.y));
        screen.PutPixel(x * 2 + 1 + old_coords.x, y * 2 + old_coords.y, screen.GetBufPixel(x * 2 + 1 + old_coords.x, y * 2 + old_coords.y));
        screen.PutPixel(x * 2 + old_coords.x, y * 2 + 1 + old_coords.y, screen.GetBufPixel(x * 2 + old_coords.x, y * 2 + 1 + old_coords.y));
        screen.PutPixel(x * 2 + 1 + old_coords.x, y * 2 + 1 + old_coords.y, screen.GetBufPixel(x * 2 + 1 + old_coords.x, y * 2 + 1 + old_coords.y));
      }
    }
    old_coords = coords;
  }

  for(int y = 0; y <= img_size; ++y)
  {
    for(int x = 0; x <= img_size; ++x)
    {
      if (skin.GetPixel(x + img_size * movement, img_size - y + img_size * cur_position).a > 0.1) {
        screen.PutPixel(x * 2 + coords.x, y * 2 + coords.y, skin.GetPixel(x + img_size * movement, img_size - y + img_size * cur_position));
        screen.PutPixel(x * 2 + 1 + coords.x, y * 2 + coords.y, skin.GetPixel(x + img_size * movement, img_size - y + img_size * cur_position));
        screen.PutPixel(x * 2 + coords.x, y * 2 + 1 + coords.y, skin.GetPixel(x + img_size * movement, img_size - y + img_size * cur_position));
        screen.PutPixel(x * 2 + 1 + coords.x, y * 2 + 1 + coords.y, skin.GetPixel(x + img_size * movement, img_size - y + img_size * cur_position));
      }
    }
  }
}

void Player::ChangePosition(Point pos) {
  coords = pos;
  old_coords = coords;
}

int ti = 0;
bool Trap::checkStatus(GLfloat frame) {
  if (frame - past_frames > frames_active) {
    if (activated == 0) {
      if (ti == 4) {
        activated = 1;
        ti = 0;
      } else {
        ti++;
        activated = 0;
      }
    } else if (activated == 1) {
      activated = 2;
    } else if (activated == 2) {
      activated = 3;
    } else if (activated == 3) {
      activated = 0;
    }
    past_frames = frame;
    return true;
  }
  return false;
}

int Trap::isActivated() {
  return activated;
}