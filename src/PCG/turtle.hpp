//
// Created by shriller44 on 12/28/23.
//

#ifndef VOXEL_TURTLE_HPP
#define VOXEL_TURTLE_HPP

#include <stack>
#include "../Engine/Vector3.h"

struct turtle {

  turtle(){
    turtle_states_ = std::make_unique<std::stack<TurtleState>>();
  }
  struct TurtleState {
    Vector3 direction_;
    Vector3 chunk_voxel_position_;
    uint32_t core_size_;
    uint32_t branch_size_ {4};
  };

  std::unique_ptr<std::stack<TurtleState>> turtle_states_;
  TurtleState state_;

};

#endif  // VOXEL_TURTLE_HPP
