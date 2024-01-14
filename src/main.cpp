//
// Created by shriller44 on 12/26/22.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Engine/Renderer.h"
#include "PCG/world.hpp"
#include "fmt/format.h"
#include "utility/helpers.h"


namespace vv_engine {
  void execute_lsystem(uint32_t lsystem_id, uint32_t production_count, world& current_world, Renderer& renderer) {

    auto l_system = current_world.get_lsystem(lsystem_id);
    /* generate the string, return output for debugging purposes*/
    auto output_string = l_system->execute_productions(production_count);
    std::cout << fmt::format("chunk: {} | output: {}\n", lsystem_id,
                             output_string);

    /* process it which fills our chunk */
    l_system->process_string(lsystem_id);

    auto vec = l_system->get_plant_chunk()->GetRenderData();

    assert(!vec.empty());

    /* store the chunk data with the associated index in the renderer */
    renderer.AddChunk(l_system->get_plant_chunk(), lsystem_id);

    /* update the buffers of each voxel in the chunk data ready to be rendered*/
    renderer.ProcessChunkData(lsystem_id);
  }
}

int main(){
  uint32_t chunk_size = 32768;

  auto renderer = std::make_unique<Renderer>(1000, 600, chunk_size);

  auto current_world = std::make_unique<world>();

  auto lsystem_0 = current_world->create_lsystem(vv_engine::lindenmayer::lsystem_type::kTest,
                                                 vv_engine::lindenmayer::lindenmayer_config {
                                                      .axiom_ = "p",
                                                      .branch_length_ = 4,
                                                      .branching_angle_ = 19,
                                                 },
                                                 chunk_size);

  vv_engine::execute_lsystem(lsystem_0, 4, *current_world ,*renderer);

  GUI& gui = renderer->GetGui();

  bool escaped {};

  uint32_t current_lsystem = lsystem_0;

  for (bool should_run = true; should_run;){
    if (gui.FlagSet(Interface::Flags::kProcessAgain)){
      fmt::print("processing again\n");
      gui.SetFlag(Interface::Flags::kProcessAgain);
      auto [branch_length, production_count, _, branching_angle, axiom, _0] = gui.GetState();

      vv_engine::lindenmayer* cur_lsystem = current_world->get_lsystem(current_lsystem);

      /* update the specified chunks state with our new values */
      cur_lsystem->update_config(axiom, branch_length, branching_angle);

      /* clear the chunk of all its voxels */
      cur_lsystem->get_plant_chunk()->WipeVoxels();

      vv_engine::execute_lsystem(current_lsystem, production_count, *current_world, *renderer);
    }

    if (gui.FlagSet(Interface::Flags::kSwapChunks)){
      gui.SetFlag(Interface::Flags::kSwapChunks);
      auto new_lsystem = gui.GetChunkIndex();
      current_lsystem = new_lsystem;
    }

//    if (gui.FlagSet(Interface::Flags::kLoadCustomFile)){
//      gui.SetFlag(Interface::Flags::kLoadCustomFile);
//      l_system->model = helpers::ReadVoxFile("../data/Models/"+gui.GetFileName());
//      gui.SetFlag(Interface::Flags::kProcessAgain);
//    }
//    if (gui.FlagSet(Interface::Flags::kSwapMaps)){
//      gui.SetFlag(Interface::Flags::kSwapMaps);
//      l_system->SetActiveMap(gui.GetMapIndex());
//      gui.SetFlag(Interface::Flags::kProcessAgain);
//    }

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      helpers::HandleSDLEvent(*renderer, event, should_run, escaped);
    }
    renderer->Render(current_lsystem);
  }
}
