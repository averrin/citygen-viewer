#ifndef __APPLICATION_H_
#define __APPLICATION_H_
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

#include <liblog/liblog.hpp>
#include <librandom/random.hpp>

#include <citygen/generator.hpp>

#include <app/drawableMap.hpp>

class Application {
public:
  Application(std::string, std::string);
  ~Application() = default;

  std::string APP_NAME;
  std::string VERSION;

  int seed;

  sf::RenderWindow *window;
  sf::ContextSettings settings;

  void processEvent(sf::Event event);
  int serve();
  void drawMinimap(sf::Sprite);
  std::shared_ptr<sf::RenderTexture> drawMap();

  LibLog::Logger &log = LibLog::Logger::getInstance();

  sf::View minimap;
  sf::View fixed;
  float scale = 1.f;
  float rotation = 0.f;
  bool damaged = true;
  std::shared_ptr<sf::RenderTexture> cache;
  std::shared_ptr<sf::RenderTexture> overview;
  std::shared_ptr<R::Generator> gen;
  std::shared_ptr<CityGen::Generator> cityGenerator;
  std::shared_ptr<CityGen::Map> map;
  std::shared_ptr<DrawableMap> drawableMap;

  bool moving = false;
  sf::Vector2f oldPos;
};

#endif // __APPLICATION_H_
