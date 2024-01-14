//
// Created by shriller44 on 12/28/23.
//

#ifndef VOXEL_CANVAS_HPP
#define VOXEL_CANVAS_HPP

#include "../Engine/Chunk/Chunk.h"
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_map>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include "Lindenmayer.hpp"

class world {

 public:

  world(): lsystem_counter_{0}{}


  uint32_t create_lsystem(vv_engine::lindenmayer::lsystem_type l_type,
                           const vv_engine::lindenmayer::lindenmayer_config& l_config,
                          uint32_t chunk_size) {

      lsystem_map_[lsystem_counter_] = std::make_unique<vv_engine::lindenmayer>(
          l_config, l_type, chunk_size
      );

      return lsystem_counter_++;
  }

  vv_engine::lindenmayer* get_lsystem(uint32_t lsystem){ return lsystem_map_[lsystem].get(); }

 private:
  uint32_t lsystem_counter_;
  uint32_t active_lsystem {};
  std::unordered_map<uint32_t, std::unique_ptr<vv_engine::lindenmayer>> lsystem_map_;
};

#endif  // VOXEL_CANVAS_HPP
