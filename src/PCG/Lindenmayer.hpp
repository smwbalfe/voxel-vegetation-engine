//
// Created by shriller44 on 12/28/23.
//

#ifndef VOXEL_LINDENMAYER_HPP
#define VOXEL_LINDENMAYER_HPP

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
#include "turtle.hpp"
#include "../Engine/Vector3.h"
#include "../Engine/Chunk/Chunk.h"

namespace vv_engine {

  class lindenmayer {
   public:
    struct rule {
      std::string successor_;
      double probability_;
    };

    struct lindenmayer_config {
      std::string axiom_;
      uint32_t branch_length_;
      double branching_angle_;
    };

    using RuleMap = std::unordered_map<char, std::vector<rule>>;
    enum class lsystem_type { kStochastic, kBasic, kTest };
    enum class Axis { kX, kY, kZ };

    lindenmayer(lindenmayer_config  lconfig, lsystem_type type,uint32_t chunk_size);

    std::string execute_productions(uint32_t production_count);
    void process_string(uint32_t chunk_index);
    void set_functions();
    void rotate(Axis axis, float sign);
    std::shared_ptr<Chunk> get_plant_chunk() { return chunk_; }
    void update_config(std::string_view axiom, uint32_t branch_length_, double branching_angle_){
      config_.axiom_ = axiom;
      config_.branch_length_ = branch_length_;
      config_.branching_angle_ = branching_angle_;
    }

   private:
    void add_rule(char c, rule&& rule) { rules_[c].emplace_back(rule); }
    std::unique_ptr<turtle> turtle_;
    RuleMap rules_;
    lindenmayer_config config_;
    std::string result_;
    std::array<std::function<void()>, 256> symbol_functions_;
    std::shared_ptr<Chunk> chunk_;
    std::vector<std::vector<double>> model;
  };


  inline std::string select_stochastic_rule(std::vector<lindenmayer::rule>& rules){
    double cum_prob = 0.0;
    std::mt19937_64 eng(std::random_device{}());
    std::uniform_real_distribution<double> distr(0, 1);
    auto random_number_0_1 = distr(eng);
    for (const auto& r: rules){
      cum_prob += r.probability_;
      if (random_number_0_1 <= cum_prob){
        return r.successor_;
      }
    }
    return std::string{""};
  }
  inline void place_cube(Vector3& cube_center, uint32_t cube_size,
                         Chunk* plant_chunk, bool placing_leaf) {
    if (cube_size == 1){
      plant_chunk->AddVoxel(cube_center, placing_leaf);
      return;
    }
    double half_size = cube_size / 2.0;
    double remainder = std::fmod(half_size, 1.0);
    if (remainder == 0.5){floor(half_size);}
    double startX = cube_center.x_ - half_size;
    double endX = startX + cube_size;
    double startY = cube_center.y_ - half_size;
    double endY = startY + cube_size;
    double startZ = cube_center.z_ - half_size;
    double endZ = startZ + cube_size;
    for(int i = static_cast<int>(startX); i <= endX; i++) {
      for(int j = static_cast<int>(startY); j <= endY; j++) {
        for(int k = static_cast<int>(startZ); k <= endZ; k++) {
          Vector3 cube_voxel(i, j, k);
          plant_chunk->AddVoxel(cube_voxel, placing_leaf);
        }
      }
    }
  }
}

#endif  // VOXEL_LINDENMAYER_HPP
