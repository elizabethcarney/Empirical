/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2018
 *
 *  @file  ModuleBase.h
 *  @brief Base class for high-level Module types in MABE (Environments, OrganismTypes, etc.)
 *
 *  This class provides a base class for ALL modules in MABE, specifying common functionality.
 * 
 *  The author of a new MABE module *must* override:
 *
 *    const std::string & GetClassName() const
 *      Provide a unique name for module class, usually the same as in code (used for debugging).
 * 
 *  The author of a new MABE module *may* also choose to override:
 * 
 *    The distructor.
 *    
 */

#ifndef MABE_MODULE_BASE_H
#define MABE_MODULE_BASE_H

#include <string>

namespace mabe {

  /// Track the specific type associated with a module.
  enum class ModuleType {
    NONE=0, BASE, ENVIRONMENT, ORGANISM_TYPE, SCHEMA, WATCHER, UNKNOWN
  };

  /// Base class for all major MABE modules that can receive names.
  class ModuleBase {
  private:
    std::string name;

  public:
    ModuleBase(const std::string & in_name) : name(in_name) { ; }
    virtual ~ModuleBase() { ; }  ///< Make sure all modules can be deleted properly.

    /// Identify the broad type of this module at compile time; ModuleType is enumerated above.
    static constexpr mabe::ModuleType GetModuleType() { return ModuleType::BASE; }

    /// Every module must have a unique name to identify its section of config files.
    const std::string & GetName() const { return name; }

    /// Every module type needs to specify its derived class name as a string.
    virtual std::string GetClassName() const = 0;

  };

}

#endif

