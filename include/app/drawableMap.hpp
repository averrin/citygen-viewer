#ifndef __DRAWABLEMAP_H_
#define __DRAWABLEMAP_H_
#include <citygen/map.hpp>
#include <memory>

#include <SFML/Graphics.hpp>
#include <Thor/Shapes/ConcaveShape.hpp>

class DrawableMap : public sf::Drawable {
  float scale;
  float rotation;
  sf::Vector2f origin;
  std::shared_ptr<CityGen::Map> map;

public:
  std::vector<std::shared_ptr<sf::Drawable>> shapes;
  DrawableMap(std::shared_ptr<CityGen::Map>);
  void setScale(float s) {
    scale = s;
  }
  void setRotation(float r) {
    rotation = r;
  }
  void setOrigin(sf::Vector2f o) {
    origin = o;
  }
  void update();

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif // __DRAWABLEMAP_H_
