#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

#include <liblog/liblog.hpp>

class Application {
public:
  Application(std::string, std::string);
  ~Application() = default;

  std::string APP_NAME;
  std::string VERSION;

  sf::RenderWindow *window;
  void processEvent(sf::Event event);
  int serve();
  void drawMainWindow();
  Logger &log = Logger::getInstance();

  sf::View minimap;
  sf::View fixed;
  sf::RectangleShape miniback;
  float zoom = 10.f;
};

#endif // __APPLICATION_H_
