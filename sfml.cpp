//
//  sfml.cpp
//  snakes
//
//  Created by Ivan Dmitrievsky on 19/04/14.
//  Copyright (c) 2014 Ivan Dmitrievsky. All rights reserved.
//

#include "sfml.h"

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

void sfml_loop(Snake &snake) {
  sf::Texture texture;
  texture.loadFromMemory(snake.get_raw_img(), snake.get_raw_img_size());
  sf::RenderWindow window({ texture.getSize().x, texture.getSize().y },
                          "Snakes & Flies",
                          sf::Style::Titlebar | sf::Style::Close);
  window.setVerticalSyncEnabled(true);

  sf::Sprite sprite;
  sprite.setTexture(texture);

  unsigned long snake_size = 0;
  std::vector<double> xs(snake_size), ys(snake_size);

  bool inited = false, play = false;
  std::vector<sf::Vertex> snake_scheme;
  snake_scheme.push_back(sf::Vertex({ 0, 0 }, sf::Color::Red));

  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::MouseMoved) {
        if (!inited) {
          snake_scheme.back().position =
              sf::Vector2f(sf::Mouse::getPosition(window));
        }
      }

      if (event.type == sf::Event::MouseButtonPressed) {
        if (!inited) {
          snake_scheme.insert(
              snake_scheme.end() - 1,
              sf::Vertex(sf::Vector2f(sf::Mouse::getPosition(window)),
                         sf::Color::Red));
        }
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          window.close();
        }
        if (event.key.code == sf::Keyboard::Space) {
          play = !play;
        }
        if (event.key.code == sf::Keyboard::Left ||
            event.key.code == sf::Keyboard::Right) {
          bool direction = event.key.code == sf::Keyboard::Right;
          if (!snake.shift_frame(direction)) {
            window.close();
          }
          texture.loadFromMemory(snake.get_raw_img(), snake.get_raw_img_size());
        }
      }

      if (event.type == sf::Event::KeyPressed && snake_scheme.size() > 2) {
        if (!inited) {
          if (snake.is_closed()) {
            snake_scheme.back() = snake_scheme.front();
          } else {
            snake_scheme.pop_back();
          }

          int implicit_joints = snake.get_implicit();

          if (implicit_joints > 0) {
            snake_size = implicit_joints * (snake_scheme.size() - 1) + 1;
          } else {
            snake_size = snake_scheme.size();
          }
          xs.resize(snake_size);
          ys.resize(snake_size);

          if (implicit_joints > 0) {
            for (int k = 0; k < snake_scheme.size() - 1; ++k) {
              auto x_diff = (snake_scheme[k + 1].position.x -
                             snake_scheme[k].position.x) /
                            implicit_joints;
              auto y_diff = (snake_scheme[k + 1].position.y -
                             snake_scheme[k].position.y) /
                            implicit_joints;
              for (int j = 0; j < implicit_joints; ++j) {
                xs[k * snake.get_implicit() + j] =
                    snake_scheme[k].position.x + j * x_diff;
                ys[k * snake.get_implicit() + j] =
                    snake_scheme[k].position.y + j * y_diff;
              }
            }
          }
          xs.back() = snake_scheme[snake_scheme.size() - 1].position.x;
          ys.back() = snake_scheme[snake_scheme.size() - 1].position.y;
          snake_scheme.resize(snake_size, { { 0, 0 }, sf::Color::Red });
          snake.set_xs(xs);
          snake.set_ys(ys);
          snake.set_pentamat();
        }
        inited = true;
      }
    }

    if (inited && play) {
      bool local_minima = snake.update();
      if (local_minima) {
        window.close();
      }
      xs = snake.get_xs();
      ys = snake.get_ys();
      for (int k = 0; k < snake_size; ++k) {
        snake_scheme[k].position.x = xs[k];
        snake_scheme[k].position.y = ys[k];
      }
    }

    window.draw(sprite);
    window.draw(&snake_scheme[0], (unsigned)snake_scheme.size(),
                sf::LinesStrip);

    window.display();
  }
}
