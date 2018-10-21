#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

#include <liblog/liblog.hpp>
#include <librandom/random.hpp>

#include <citygen/building.hpp>

class Application {
public:
  Application(std::string, std::string);
  ~Application() = default;

  std::string APP_NAME;
  std::string VERSION;

  sf::RenderWindow *window;
  void processEvent(sf::Event event);
  int serve();
  void drawMinimap(sf::Sprite);
  std::shared_ptr<sf::RenderTexture> drawMap();
                // void updateMinimap();
  Logger &log = Logger::getInstance();

  sf::View minimap;
  sf::View fixed;
  float scale = 1.f;
  bool damaged = true;
std::shared_ptr<sf::RenderTexture> cache;
std::shared_ptr<R::Generator> gen;
std::shared_ptr<BuildingGenerator> buildingGenerator;

};

#endif // __APPLICATION_H_
