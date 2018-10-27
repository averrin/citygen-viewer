#ifndef __DRAWABLEMAP_H_
#define __DRAWABLEMAP_H_
#include <citygen/map.hpp>
#include <memory>

#include <SFML/Graphics.hpp>
#include <Thor/Shapes/ConcaveShape.hpp>

class DrawableMap : public sf::Drawable {
  float scale;
  std::shared_ptr<CityGen::Map> map;

public:
  std::vector<std::shared_ptr<sf::Drawable>> shapes;
  DrawableMap(std::shared_ptr<CityGen::Map>);
  void setScale(float s) {
    scale = s;
    update();
  }
  void update();

private:
  virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif // __DRAWABLEMAP_H_
