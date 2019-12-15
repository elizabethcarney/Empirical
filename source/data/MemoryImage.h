/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2018-2019.
 *
 *  @file  MemoryImage.h
 *  @brief A series of bytes that can store a collection of arbitrary objects.
 * 
 *  There MemoryImage forms a base class that maintains a series of bytes.  The derived class can
 *  either have those bytes as a dynamic size (MemoryVector) or static size (MemoryArray).
 */

#ifndef EMP_MEMORY_IMAGE_H
#define EMP_MEMORY_IMAGE_H

#include "../base/assert.h"
#include "../base/Ptr.h"
#include "../base/array.h"
#include "../base/vector.h"

namespace emp {

  /// A MemoryImage is a full set of variable values, linked together.  They can represent
  /// all of the variables in a scope or in a class.
  template <typename MEM_T>
  class MemoryImage {
  protected:
    using byte_t = unsigned char;
    MEM_T memory;      ///< The specific memory values.

  public:
    MemoryImage() : memory() { ; }
    MemoryImage(MemoryImage && _in) : memory(std::move(_in.memory)) { _in.memory.resize(0); }
    // Note: No copy constructor since each object needs to be copied independently.

    ~MemoryImage() { emp_assert(memory.size() == 0, "Must manually delete memory before destructing."); }

    /// Get a typed pointer to a specific position in this image.
    template <typename T> emp::Ptr<T> GetPtr(size_t pos) {
      emp_assert(pos + sizeof(T) <= GetSize(), pos, sizeof(T), GetSize());
      return reinterpret_cast<T*>(&memory[pos]);
    }

    /// Get a proper reference to an object represented in this image.
    template <typename T> T & GetRef(size_t pos) {
      emp_assert(pos + sizeof(T) <= GetSize(), pos, sizeof(T), GetSize());
      return *(reinterpret_cast<T*>(&memory[pos]));
    }

    /// Get a const reference to an object represented in this image.
    template <typename T> const T & GetRef(size_t pos) const {
      emp_assert(pos + sizeof(T) <= GetSize(), pos, sizeof(T), GetSize());
      return *(reinterpret_cast<const T*>(&memory[pos]));
    }

    byte_t & operator[](size_t pos) { return memory[pos]; }
    const byte_t & operator[](size_t pos) const { return memory[pos]; }
    size_t size() const { return memory.size(); }
    size_t GetSize() const { return memory.size(); }

    /// Build a new object of the provided type at the memory position indicated.
    template <typename T, typename... ARGS>
    void Construct(size_t pos, ARGS &&... args) {
      emp_assert(pos + sizeof(T) <= GetSize(), pos, sizeof(T), GetSize());
      new (GetPtr<T>(pos).Raw( std::forward<ARGS>(args)... )) T;
    }

    /// Destruct an object of the provided type at the memory position indicated; don't release memory!
    template <typename T>
    void Destruct(size_t pos) {
      emp_assert(pos + sizeof(T) <= GetSize(), pos, sizeof(T), GetSize());
    	GetPtr<T>(pos)->~T();
    }

    /// Copy an object from another MemoryImage with an identical layout.
    template<typename T>
    void CopyObj(size_t pos, MemoryImage & image2) {
      Construct<T, const T &>(pos, image2.GetRef<T>(pos));
    }
  };

  template <unsigned int SIZE>
  class MemoryArray : MemoryImage< emp::array<std::byte, SIZE> > {
  protected:
    unsigned int free_pos = 0;

  public:
    ~MemoryArray() { }

    /// Add a new object to this memory, just after allocated space.
    template <typename T, typename... ARGS>
    size_t AddObject(ARGS &&... args) {
      const size_t obj_pos = free_pos;
      free_pos += sizeof(T);
      Construct<T>(obj_pos, std::forward<ARGS>(args)...);
      return obj_pos;
    }
  };

  class MemoryVector : MemoryImage< emp::vector<std::byte> > {
  protected:

  public:
    MemoryVector(size_t num_bytes=0) {
      memory.resize(num_bytes);
    }
    ~MemoryVector() { }

    void resize(size_t new_size) { memory.resize(new_size); }

    /// Increase the size of this memory to add a new object inside it.
    template <typename T, typename... ARGS>
    size_t AddObject(ARGS &&... args) {
      const size_t obj_pos = memory.size();
      memory.resize(obj_pos + sizeof(T));
      Construct<T>(obj_pos, std::forward<ARGS>(args)...);
      return obj_pos;
    }
  };

}

#endif
