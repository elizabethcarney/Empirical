//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2016.
//  Released under the MIT Software license; see doc/LICENSE
//
//  A bunch of C++ Template Meta-programming tricks.

#ifndef EMP_META_H
#define EMP_META_H

#include <functional>
#include <tuple>
#include <utility>

namespace emp {

  // Effectively create a function (via constructor) where all args are computed, then ignored.
  struct run_and_ignore { template <typename... T> run_and_ignore(T&&...) {} };

  // Trim off a specific type position from a pack.
  template <typename T1, typename... Ts> using first_type = T1;
  template <typename T1, typename T2, typename... Ts> using second_type = T2;
  template <typename T1, typename T2, typename T3, typename... Ts> using third_type = T3;

  // Index into a template parameter pack to grab a specific type.
  namespace {
    template <int ID, typename T, typename... Ts>
    struct pack_id_impl { using type = typename pack_id_impl<ID-1,Ts...>::type; };

    template <typename T, typename... Ts>
    struct pack_id_impl<0,T,Ts...> { using type = T; };
  }

  template <int ID, typename... Ts>
  using pack_id = typename pack_id_impl<ID,Ts...>::type;

  // Trim off the last type from a pack.
  template <typename... Ts> using last_type = pack_id<sizeof...(Ts)-1,Ts...>;

  // Trick to call a function using each entry in a parameter pack.
#define EMP_EXPAND_PPACK(PPACK) ::emp::run_and_ignore{ 0, ((PPACK), void(), 0)... }

  // Check to see if a specified type is part of a set of types.
  template <typename TEST> constexpr bool has_type() { return false; }
  template <typename TEST, typename FIRST, typename... OTHERS>
  constexpr bool has_type() { return std::is_same<TEST, FIRST>() || has_type<TEST,OTHERS...>(); }

  // Count how many times a specified type appears in a set of types.
  template <typename TEST> constexpr int count_type() { return 0; }
  template <typename TEST, typename FIRST, typename... OTHERS>
  constexpr int count_type() { return count_type<TEST,OTHERS...>() + (std::is_same<TEST, FIRST>()?1:0); }

  // Return the index of a test type in a set of types.
  namespace {
    template <typename TEST_T> constexpr int get_type_index_impl() { return -1; } // Not found!
    template <typename TEST_T, typename T1, typename... Ts>
    constexpr int get_type_index_impl() {
      if (std::is_same<TEST_T, T1>()) return 0;                     // Found here!
      constexpr int next_id = get_type_index_impl<TEST_T,Ts...>();  // Keep looking...
      if (next_id < 0) return -1;                                   // Not found!
      return next_id + 1;                                           // Found later!
    }
  }
  template <typename TEST_T, typename... Ts>
  constexpr int get_type_index() { return get_type_index_impl<TEST_T, Ts...>(); }


  // These functions can be used to test if a type-set has all unique types or not.

  // Base cases...
  template <typename TYPE1> constexpr bool has_unique_first_type() { return true; }
  template <typename TYPE1> constexpr bool has_unique_types() { return true; }

  template <typename TYPE1, typename TYPE2, typename... TYPE_LIST>
  constexpr bool has_unique_first_type() {
    return (!std::is_same<TYPE1, TYPE2>()) && emp::has_unique_first_type<TYPE1, TYPE_LIST...>();
  }

  template <typename TYPE1, typename TYPE2, typename... TYPE_LIST>
  constexpr bool has_unique_types() {
    return has_unique_first_type<TYPE1, TYPE2, TYPE_LIST...>()  // Check first against all others...
      && has_unique_types<TYPE2, TYPE_LIST...>();               // Recurse through other types.
  }


  // sfinae_decoy<X,Y> will always evaluate to X no matter what Y is.
  // X is type you want it to be; Y is a decoy trigger potential substituion failue.
  template <typename REAL_TYPE, typename EVAL_TYPE> using sfinae_decoy = REAL_TYPE;
  template <typename EVAL_TYPE>                     using bool_decoy = bool;
  template <typename EVAL_TYPE>                     using int_decoy = int;

  // Deprecated macros
#define emp_bool_decoy(TEST) emp::sfinae_decoy<bool, decltype(TEST)>
#define emp_int_decoy(TEST) emp::sfinae_decoy<int, decltype(TEST)>


  // constexpr bool test_type<TEST,T>() returns true if T passes the TEST, false otherwise.
  //
  // TEST is a template.  TEST will fail if TEST<T> fails to resolve (substitution failure) -OR-
  // if TEST<T> does resolve, but TEST<T>::value == false.  Otherwise the test passes.
  //
  // Two helper functions exist to test each part: test_type_exist and test_type_value.

  namespace {
    template <template <typename> class FILTER, typename T>
    constexpr bool tt_exist_impl(bool_decoy<FILTER<T>> x) { return true; }
    template <template <typename> class FILTER, typename T>
    constexpr bool tt_exist_impl(...) { return false; }
  }

  template <template <typename> class TEST, typename T>
  constexpr bool test_type_exist() { return tt_exist_impl<TEST, T>(true); }

  template <template <typename> class TEST, typename T>
  constexpr bool test_type_value() { return TEST<T>::value; }




  // TruncateCall reduces the number of arguments for calling a function if too many are used.
  // @CAO: This should be simplified using TypeSet

  namespace {
    template <typename... PARAMS>
    struct tcall_impl {
      template <typename FUN_T, typename... EXTRA>
      static auto call(FUN_T fun, PARAMS... args, EXTRA...) {
        return fun(args...);
      }
    };
  }

  // Truncate the arguments provided, using only the relevant ones for a function call.
  template <typename R, typename... PARAMS, typename... ARGS>
  auto TruncateCall(std::function<R(PARAMS...)> fun, ARGS &&... args) {
    return tcall_impl<PARAMS...>::call(fun, std::forward<ARGS>(args)...);
  }

  // Expand a function to take (and ignore) extra arguments.
  template <typename R, typename... ARGS>
  struct AdaptFunction {
    template <typename... EXTRA_ARGS>
    static auto Expand(const std::function<R(ARGS...)> & fun) {
      return [fun](ARGS... args, EXTRA_ARGS...){ return fun(args...); };
    }
  };


  // Apply a tuple as arguments to a function!
  // Unroll all IDs for the tuple, then get all of them at once, calling function.
  // Based on Kerrek SB in http://stackoverflow.com/questions/10766112/c11-i-can-go-from-multiple-args-to-tuple-but-can-i-go-from-tuple-to-multiple

  // Hidden namespace for implementation details, users never invoke these directly
  namespace {
    template <typename FUN_T, typename TUPLE_T, bool is_done, int TOTAL, int... N>
    struct apply_impl {
      static auto apply(FUN_T & fun, const TUPLE_T & tup) {
        constexpr auto num_ids = sizeof...(N);
        constexpr bool done = (TOTAL==1+num_ids);
        return apply_impl<FUN_T, TUPLE_T, done, TOTAL, N..., num_ids>::apply(fun, tup);
      }
    };

    template <typename FUN_T, typename TUPLE_T, int TOTAL, int... N>
    struct apply_impl<FUN_T, TUPLE_T, true, TOTAL, N...> {
      static auto apply(FUN_T & fun, const TUPLE_T & tup) {
        return fun(std::get<N>(tup)...);
      }
    };
  }

  // User invokes ApplyTuple
  template <typename FUN_T, typename TUPLE_T>
  auto ApplyTuple(FUN_T fun, const TUPLE_T & tup) {
    using tuple_decay_t = std::decay_t<TUPLE_T>;
    constexpr auto tup_size = std::tuple_size<tuple_decay_t>::value;
    return apply_impl<FUN_T, TUPLE_T, tup_size==0, tup_size>::apply(fun, tup);
  }


  // Combine multiple keys into a single hash value.
  template <typename T>
  std::size_t CombineHash(const T & x) { return std::hash<T>()(x); }

  template<typename T1, typename T2, typename... EXTRA>
  std::size_t CombineHash(const T1 & x1, const T2 & x2, const EXTRA &... x_extra) {
    const std::size_t hash2 = CombineHash(x2, x_extra...);
    return std::hash<T1>()(x1) + 0x9e3779b9 + (hash2 << 19) + (hash2 >> 13);
  }



  // Change the internal type arguments on a template...
  // Adapted from: Sam Varshavchik
  // http://stackoverflow.com/questions/36511990/is-it-possible-to-disentangle-a-template-from-its-arguments-in-c
  namespace {
    template<typename T, typename ...U> struct AdaptTemplateHelper {
      using type = T;
    };

    template<template <typename...> class T, typename... V, typename... U>
    struct AdaptTemplateHelper<T<V...>, U...> {
      using type = T<U...>;
    };
  }

  template<typename T, typename... U>
  using AdaptTemplate = typename AdaptTemplateHelper<T, U...>::type;


  // Variation of AdaptTemplate that only adapts first template argument.
  namespace {
    template<typename T, typename U> class AdaptTemplateHelper_Arg1 {
    public:
      using type = T;
    };

    template<template <typename...> class T, typename X, typename ...V, typename U>
    class AdaptTemplateHelper_Arg1<T<X, V...>, U> {
    public:
      using type = T<U, V...>;
    };
  }

  template<typename T, typename U>
  using AdaptTemplate_Arg1 = typename AdaptTemplateHelper_Arg1<T, U>::type;


  // Some math inside templates...
  template <int I, int... Is> struct tIntMath {
    static constexpr int Sum() { return I + tIntMath<Is...>::Sum(); }
    static constexpr int Product() { return I * tIntMath<Is...>::Product(); }
  };

  template <int I> struct tIntMath<I> {
    static constexpr int Sum() { return I; }
    static constexpr int Product() { return I; }
  };
}


#endif
