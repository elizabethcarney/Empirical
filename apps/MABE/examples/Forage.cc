/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2018
 *
 *  @file  Forage.cc
 *  @brief An example of building a MABE world where AvidaGP organisms evovle to forage.
 */

#include "mabe.h"
#include "controlers/AvidaGP.h"
#include "genomes/LinearGenome.h"
#include "environments/ResourceGrid.h"
#include "schemas/TournamentSelect.h"
#include "watchers/TrackLineage.h"

int main(int argc, char * argv[]) {
  // Build the world using these types.
  mabe::World world;

  using org_t = mabe::OrganismType<mabe::AvidaGP, mabe::LinearGenome>;
  auto & org_type        = world.BuildModule<org_t>("Organisms");
  auto & resource_grid   = world.BuildModule<mabe::ResourceGrid>("ResourceGrid");
  auto & tourny_schema   = world.BuildModule<mabe::TournamentSelect>("TournamentSelect");
  auto & lineage_tracker = world.BuildModule<mabe::TrackLineage>("LineageTracker");
  
  // Configure the world using the "farm.cfg" file and command-line overrides.
  world.Config("forage.cfg", argv, argc);
  
  // Running world will go for full configured duration; can also manually Update() it.
  return world.Run();
}

