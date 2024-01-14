//
// Created by shriller44 on 12/28/23.
//

#ifndef VOXEL_HELPERS_H
#define VOXEL_HELPERS_H

#include <vector>
#include <fstream>
#include <sstream>
#include "../Engine/Renderer.h"

namespace helpers {
  std::vector<std::vector<double>> ReadVoxFile(const std::string& filename){
    std::string pos;
    std::fstream fs {filename};
    std::vector<std::vector<double>> offsets;
    while(std::getline(fs, pos)){
      std::stringstream ss{ pos};
      std::string c;
      std::vector<double> values;
      while(std::getline(ss, c, ',')){
        int val;
        if (c.size() == 1) {
          val = c[0] - '0';
        }
        else {
          val = std::stoi(c);
        }
        values.emplace_back(val);
      }
      offsets.emplace_back(values);
    }
    return offsets;
  }

  void HandleSDLEvent(Renderer& renderer, SDL_Event& event, bool& should_run, bool& escaped){
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
      case SDL_QUIT: {
        should_run = false;
        SDL_Quit();
        break;
      }
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          escaped = !escaped;
        }
      }
      case SDL_MOUSEMOTION: {
        if (!escaped) {
          int x, y;
          SDL_GetMouseState(&x, &y);
          renderer.ProcessMouse(static_cast<float>(x),static_cast<float>(y));
        }
      }
    }
  }
}
#endif  // VOXEL_HELPERS_H
