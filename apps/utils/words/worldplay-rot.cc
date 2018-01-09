// Test the rotation of letters through an alphabet to see if new words are formed.

//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2016-2017.
//  Released under the MIT Software license; see doc/LICENSE
//
//
//  Build graphs of various types in the standard format.
//  NOTE: All questions can be answered by providing command-line arguements.

#include <iostream>
#include <set>

#include "../../../source/base/assert.h"
#include "../../../source/config/command_line.h"
#include "../../../source/tools/File.h"
#include "../../../source/tools/math.h"
#include "../../../source/tools/set_utils.h"
#include "../../../source/tools/string_utils.h"

void Rot(std::string & str, int rot) {
  for (char & c : str) {
    if (c >= 'A' && c <= 'Z') c = emp::Mod(c - 'A' + rot, 26) + 'A';
    else if (c >= 'a' && c <= 'z') c = emp::Mod(c - 'a' + rot, 26) + 'a';
  }
}

int main(int argc, char* argv[])
{
  emp::vector<std::string> args = emp::cl::args_to_strings(argc, argv);

  if (args.size() != 3) {
    std::cout << "Expected: " << args[0] << " [dict file] [rotation]" << std::endl;
    exit(0);
  }

  int rot = emp::from_string<int>(args[2]);

  emp::File file(args[1]);   // Load the file.
  file.CompressWhitespace(); // Remove unneeded whitespace.
  
  // Transfer the file into a set.
  std::set<std::string> words;
  for (size_t i = 0; i < file.size(); i++) {
    words.insert(file[i]);
  }

  for (size_t i = 0; i < file.size(); i++) {
    std::string test = file[i];
    Rot(test, rot);
    if (emp::Has(words, test)) {
      std::cout << file[i] << " -> " << test << std::endl;
    }
  }
}
