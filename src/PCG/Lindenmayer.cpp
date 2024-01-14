//
// Created by shriller44 on 12/28/23.
//
#include "Lindenmayer.hpp"

#include <utility>


vv_engine::lindenmayer::lindenmayer(lindenmayer_config  lconfig, lsystem_type type,
            uint32_t chunk_size)
    : config_(std::move(lconfig)), chunk_{std::make_shared<Chunk>(chunk_size)}, turtle_ {std::make_unique<turtle>()} {
  set_functions();
  switch (type) {
    case lsystem_type::kTest:
      add_rule('p', {"I+[p+]--//[--]I[++]-[p]++p", 1.0});
      add_rule('I', {">S[//&&#][//^^]>S", 1.0});
      add_rule('S', {"S>S", 1.0});
      break;
  }
}

void vv_engine::lindenmayer::set_functions() {
  /* place assets */
  symbol_functions_['#'] = [&](){

    chunk_->AddVoxel(turtle_->state_.chunk_voxel_position_, true);
    if (!model.empty()) {
      for (const auto& vox : model) {
        auto turtle_pos = turtle_->state_.chunk_voxel_position_;
        turtle_pos.x_ += vox[0];
        turtle_pos.y_ += vox[1];
        turtle_pos.z_ += vox[2];
        chunk_->AddVoxel(turtle_pos, true);
      }
    }
  };

  /* branching */
  symbol_functions_[']'] = [&](){
    std::cout << "branching\n";
    turtle_->state_.branch_size_++;
    if (turtle_->state_.branch_size_ > turtle_->state_.core_size_) turtle_->state_.branch_size_ = turtle_->state_.core_size_;
    turtle_->state_ = turtle_->turtle_states_->top();
    turtle_->turtle_states_->pop();
  };

  symbol_functions_['['] = [&](){

    turtle_->turtle_states_->push(turtle_->state_);
    turtle_->state_.branch_size_--;
    if (turtle_->state_.branch_size_ < 1) turtle_->state_.branch_size_ = 1;
  };

  /* rotations */
  symbol_functions_['^'] = [&](){  rotate(Axis::kX,-1);};
  symbol_functions_['\\'] = [&](){ rotate(Axis::kY, 1); };
  symbol_functions_['/'] = [&](){  rotate(Axis::kY,-1); };
  symbol_functions_['&'] = [&](){  rotate(Axis::kX,1); };
  symbol_functions_['+'] = [&](){  rotate(Axis::kZ, 1); };
  symbol_functions_['-'] = [&](){  rotate(Axis::kZ, -1); };

  symbol_functions_['>'] = [&]() {
    fmt::print("forward\n");
    Vector3 initial_position = turtle_->state_.chunk_voxel_position_;
    Vector3 current_position = initial_position;
    for (int i = 0; i < config_.branch_length_ ; i++){
//      chunk_->AddVoxel(turtle_->state_.chunk_voxel_position_, false);
      vv_engine::place_cube(turtle_->state_.chunk_voxel_position_, turtle_->state_.branch_size_, chunk_.get(), false);
      Vector3 new_voxel{};
      while (chunk_->GetVoxel(turtle_->state_.chunk_voxel_position_) != nullptr) {
        for (;;) {
          new_voxel = current_position.Update(0.1, turtle_->state_.direction_, initial_position);
          initial_position = new_voxel;
          if (new_voxel.Floor(1) !=  turtle_->state_.chunk_voxel_position_.Floor(1)) {
            break;
          }
        }
        turtle_->state_.chunk_voxel_position_ = new_voxel;
      }
    }
  };
}


std::string vv_engine::lindenmayer::execute_productions(uint32_t production_count) {

  fmt::print("production count: {}\n", production_count);

  result_ = config_.axiom_;
  fmt::print("result: {}\n", result_);
  while (production_count-- != 0) {
    std::string intermediate_string;
    for (const auto& c: result_) {

      auto rule = rules_.find(c);
      if (rule != rules_.end()){
        auto [_, rule_vector] = *rule;
        intermediate_string += vv_engine::select_stochastic_rule(rule_vector);
      }
      else {
        intermediate_string += c;
      }
    }
    result_ = intermediate_string;
  }
  return result_;
}

void vv_engine::lindenmayer::process_string(uint32_t chunk_index) {

  /* reset position and direction */

  turtle_->state_.chunk_voxel_position_ = Vector3{chunk_->GetSize() / 2.0,
                                                 3,
                                                 chunk_->GetSize() / 2.0};

  turtle_->state_.direction_ = Vector3{0.0,1.0,0.0};

  for (const auto& c: result_){
    if (symbol_functions_[c]) {
      symbol_functions_[c]();
    } else {
      std::cout << fmt::format("no function for symbol: {}\n", c);
    }
  }

  while (!turtle_->turtle_states_->empty()){
    turtle_->turtle_states_->pop();
  }
}

void vv_engine::lindenmayer::rotate(lindenmayer::Axis axis, float sign) {
  auto branching_angle = static_cast<float>(config_.branching_angle_);
  glm::vec3 axis_vec;
  switch(axis){
    case Axis::kX: {
      axis_vec = {1,0,0};
      break;
    }
    case Axis::kY: {
      axis_vec = {0,1,0};
      break;
    }
    case Axis::kZ : {
      axis_vec = {0,0,1};
      break;
    }
  }
  glm::dmat4 rotation_matrix = glm::rotate(glm::mat4(1.0), sign * branching_angle, axis_vec);
  turtle_->state_.direction_ = rotation_matrix * glm::vec4(glm::dvec3(turtle_->state_.direction_),1.0f);
}

