/**
 *  @note This file is part of Empirical, https://github.com/devosoft/Empirical
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2019
 *
 *  @file matchbin_selectors.h
 *  @brief Selector structs that can be plugged into MatchBin.
 *
 */


#ifndef EMP_MATCH_BIN_SELECTORS_H
#define EMP_MATCH_BIN_SELECTORS_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <ratio>
#include <string>
#include <tuple>
#include <array>
#include <utility>

#include "tools/Binomial.h"

#include "../base/assert.h"
#include "../base/array.h"
#include "../base/vector.h"
#include "../tools/math.h"
#include "../tools/IndexMap.h"
#include "../tools/BitSet.h"
#include "../tools/string_utils.h"
#include "../tools/hash_utils.h"

namespace emp {

  struct CacheStateBase {
    CacheStateBase() = default;
    virtual ~CacheStateBase() {};
    virtual std::optional<emp::vector<size_t>> operator()(size_t n) = 0;
  };

  struct RouletteCacheState : public CacheStateBase {

    emp::IndexMap indexMap;
    emp::vector<size_t> uids;
    emp::Random rand;
    size_t default_n;

    RouletteCacheState() = default;
    RouletteCacheState(
      emp::IndexMap& im,
      emp::vector<size_t>& ids,
      emp::Random& r,
      const size_t default_n_
    ) : indexMap(im)
      , uids(ids)
      , rand(r)
      , default_n(default_n_)
    { ; }

    std::optional<emp::vector<size_t>> operator()(size_t n) override {

      if (n == 0) n = default_n;

      // don't perform a lookup into an empty IndexMap, that's a segfault
      // double braces: an empty vector inside an optional
      if (indexMap.GetSize() == 0) {
        return std::optional<emp::vector<size_t>>{emp::vector<size_t>{}};
      }

      emp::vector<size_t> res;
      res.reserve(n);

      for (size_t i = 0; i < n; ++i) {
        // if there's only one item in the IndexMap, don't do a random draw
        const double match_pos = (
          indexMap.GetSize() == 1
          ? 0
          : rand.GetDouble(indexMap.GetWeight())
        );
        const size_t idx = indexMap.Index(match_pos);
        res.push_back(uids[idx]);
      }
      return res;
    }

  };

  struct RankedCacheState: public CacheStateBase {

    emp::vector<size_t> uids;
    size_t requestSize;
    size_t default_n;

    RankedCacheState() = default;
    RankedCacheState(
      emp::vector<size_t>::iterator begin,
      size_t back,
      size_t n,
      size_t default_n_
    ) : uids(emp::vector<size_t>(begin, begin + back))
      , requestSize(n)
      , default_n(default_n_)
    { ; }

    std::optional<emp::vector<size_t>> operator()(size_t n) override {
      if (n == 0) n = default_n;

      if (n > requestSize) return std::nullopt;
      if (n >= uids.size()) return uids;

      return emp::vector<size_t>(uids.begin(), uids.begin()+n);
    }

  };

  /// Abstract base class for selectors
  template<typename CacheType>
  struct SelectorBase {
    virtual ~SelectorBase() {};
    virtual CacheType operator()(
        const emp::vector<size_t>& uids,
        const std::unordered_map<size_t, double>& scores,
        size_t n
        ) = 0;
    virtual std::string name() const = 0;
  };

  /// Returns matches within the threshold ThreshRatio sorted by match quality.
  template<
    typename ThreshRatio = std::ratio<-1,1>, // neg numerator means +infy
    size_t DefaultN = 1
  >
  struct RankedSelector : public SelectorBase<RankedCacheState> {

    RankedSelector(emp::Random&){ ; }

    using cache_state_type_t = RankedCacheState;

    std::string name() const override {
      return emp::to_string(
        "Ranked Selector (",
        "ThreshRatio: ",
        ThreshRatio::num,
        "/",
        ThreshRatio::den,
        ", "
        "DefaultN: ",
        DefaultN,
        ")"
      );
    }

    RankedCacheState operator()(
      const emp::vector<size_t>& uids_,
      const std::unordered_map<size_t, double>& scores,
      size_t n
    ) override {

      if (n == 0) n = DefaultN;

      emp::vector<size_t> uids(uids_);

      // treat any negative numerator as positive infinity
      const double thresh = (
        ThreshRatio::num < 0
        ? std::numeric_limits<double>::infinity()
        : ((double) ThreshRatio::num) / ((double)ThreshRatio::den)
      );

      // Perform a bounded partial sort to find the first n results
      std::partial_sort(
        uids.begin(),
        uids.begin() + std::min(n, uids.size()),
        uids.end(),
        [&scores](const size_t &a, const size_t &b){
          return scores.at(a) < scores.at(b);
        }
      );


      size_t back = 0;
      while (
        back < uids.size()
        && back < n
        && scores.at(uids[back]) <= thresh
      ) ++back;


      return RankedCacheState(uids.begin(), back, n, DefaultN);
    }

  };

  /// Selector chooses probabilistically based on match quality with replacement.
  /// ThreshRatio: what is the raw maximum score to even be considered to match
  /// SkewRatio: how much more heavily should the best matches be weighted
  /// in terms of match probability; must be greater than 0
  /// (close to zero: very heavily, large: mostly even weighting)
  /// MaxBaselineRatio: maximum score that all scores will be normalized to
  /// baseline = min(min_score, MaxBaselineRatio)
  /// normalized_score = score - baseline
  /// ...
  ///  overall, p_match ~ 1 / (skew + score - baseline)
  template<
    typename ThreshRatio = std::ratio<-1, 1>,// we treat neg numerator as +infty
    typename SkewRatio = std::ratio<1, 10>,
    typename MaxBaselineRatio = std::ratio<1, 1>,//treat neg numerator as +infty
    size_t DefaultN = 1
  >
  struct RouletteSelector : public SelectorBase<RouletteCacheState> {

    using cache_state_type_t = RouletteCacheState;

    emp::Random & rand;

    RouletteSelector(emp::Random & rand_)
    : rand(rand_)
    { ; }

    std::string name() const override {
      return emp::to_string(
        "Roulette Selector (",
        "ThreshRatio: ",
        ThreshRatio::num,
        "/",
        ThreshRatio::den,
        ", ",
        "SkewRatio: ",
        SkewRatio::num,
        "/",
        SkewRatio::den,
        ", ",
        "MaxBaselineRatio: ",
        MaxBaselineRatio::num,
        "/",
        MaxBaselineRatio::den,
        ", ",
        "DefaultN: ",
        DefaultN,
        ")"
      );
    }

    RouletteCacheState operator()(
      const emp::vector<size_t>& uids_,
      const std::unordered_map<size_t, double>& scores,
      size_t n
    ) override {

      if (n == 0) n = DefaultN;

      emp::vector<size_t> uids(uids_);

      const double skew = (
        static_cast<double>(SkewRatio::num)/static_cast<double>(SkewRatio::den)
      );
      emp_assert(skew > 0);

      // treat any negative numerator as positive infinity
      const double thresh = (
        ThreshRatio::num < 0
        ? std::numeric_limits<double>::infinity()
        : static_cast<double>(ThreshRatio::num)
          / static_cast<double>(ThreshRatio::den)
      );

      // treat any negative numerator as positive infinity
      const double max_baseline = (
        MaxBaselineRatio::num < 0
        ? std::numeric_limits<double>::infinity()
        : static_cast<double>(MaxBaselineRatio::num)
          / static_cast<double>(MaxBaselineRatio::den)
      );

      double min_score = std::min_element(
        std::begin(scores),
        std::end(scores),
        [](const auto & a, const auto & b){
          return a.second < b.second;
        }
      )->second;

      size_t partition = std::distance(
        std::begin(uids),
        std::partition(
          std::begin(uids),
          std::end(uids),
          [&scores, thresh](size_t uid){ return scores.at(uid) <= thresh; }
        )
      );

      // skew relative to strongest match less than or equal to max_baseline
      // to take into account regulation...
      // (the default value of max_baseline is 1.0 because without
      // upregulation, the best possible match score is 1.0)
      const double baseline = std::min(min_score, max_baseline);
      emp_assert(baseline >= 0);
      emp_assert(baseline <= max_baseline);

      IndexMap match_index(partition);

      for (size_t p = 0; p < partition; ++p) {
        emp_assert(scores.at(uids[p]) - baseline >= 0);
        match_index.Adjust(p, 1.0 / ( skew + scores.at(uids[p]) - baseline ));
      }

      return RouletteCacheState(match_index, uids, rand, DefaultN);

    }

  };

  /// Selector chooses probabilistically based on match quality with replacement using exponentially.
  /// ThreshRatio: what is the minimum probability of matching
  /// to even be considered for a match
  /// BRatio, CRatio, ZRatio: p_match ~ b ^ (c * normalized_score) ^ z
  /// MaxBaselineRatio: maximum score that all scores will be normalized to
  /// baseline = min(min_score, MaxBaselineRatio)
  /// normalized_score = score - baseline
  template<
    typename ThreshRatio = std::ratio<13, 10>,// treat neg numerator as +infty
    typename BRatio = std::ratio<1, 100>,
    typename CRatio = std::ratio<4, 1>,
    typename ZRatio = std::ratio<4, 1>,
    typename MaxBaselineRatio = std::ratio<5, 4>,//treat neg numerator as +infty
    size_t DefaultN = 1
  >
  struct ExpRouletteSelector : public SelectorBase<RouletteCacheState> {

    using cache_state_type_t = RouletteCacheState;

    emp::Random & rand;

    ExpRouletteSelector(emp::Random & rand_)
    : rand(rand_)
    { ; }

    std::string name() const override {
      return emp::to_string(
        "Exponential Roulette Selector (",
        "ThreshRatio: ",
        ThreshRatio::num,
        "/",
        ThreshRatio::den,
        ", ",
        "BRatio: ",
        BRatio::num,
        "/",
        BRatio::den,
        ", ",
        "CRatio: ",
        CRatio::num,
        "/",
        CRatio::den,
        ", ",
        "ZRatio: ",
        ZRatio::num,
        "/",
        ZRatio::den,
        ", ",
        "MaxBaselineRatio: ",
        MaxBaselineRatio::num,
        "/",
        MaxBaselineRatio::den,
        ", ",
        "DefaultN: ",
        DefaultN,
        ")"
      );
    }

    RouletteCacheState operator()(
      const emp::vector<size_t>& uids_,
      const std::unordered_map<size_t, double>& scores,
      size_t n
    ) override {

      if (n == 0) n = DefaultN;

      emp::vector<size_t> uids(uids_);

      const double b = (static_cast<double>(BRatio::num) / BRatio::den);
      emp_assert(b > 0 && b < 1);

      const double c = (static_cast<double>(CRatio::num) / CRatio::den);
      emp_assert(c > 0);

      const double z = (static_cast<double>(ZRatio::num) / ZRatio::den);
      emp_assert(z > 0);

      // treat any negative numerator as positive infinity
      const double thresh = (
        ThreshRatio::num < 0
        ? std::numeric_limits<double>::infinity()
        : static_cast<double>(ThreshRatio::num)
          / static_cast<double>(ThreshRatio::den)
      );

      // treat any negative numerator as positive infinity
      const double max_baseline = (
        MaxBaselineRatio::num < 0
        ? std::numeric_limits<double>::infinity()
        : static_cast<double>(MaxBaselineRatio::num)
          / static_cast<double>(MaxBaselineRatio::den)
      );

      double min_score = std::min_element(
        std::begin(scores),
        std::end(scores),
        [](const auto & a, const auto & b){
          return a.second < b.second;
        }
      )->second;

      size_t partition = std::distance(
        std::begin(uids),
        std::partition(
          std::begin(uids),
          std::end(uids),
          [&scores, thresh](size_t uid){ return scores.at(uid) <= thresh; }
        )
      );

      // skew relative to strongest match less than or equal to max_baseline
      // to take into account regulation...
      // (the default value of max_baseline is 1.0 because without
      // upregulation, the best possible match score is 1.0)
      const double baseline = std::min(min_score, max_baseline);
      emp_assert(baseline >= 0);
      emp_assert(baseline <= max_baseline);

      IndexMap match_index(partition);

      for (size_t p = 0; p < partition; ++p) {
        emp_assert(scores.at(uids[p]) - baseline >= 0);
        match_index.Adjust(
          p,
          std::pow(
            b,
            std::pow(c * (scores.at(uids[p]) - baseline), z)
          )
        );
      }

      return RouletteCacheState(match_index, uids, rand, DefaultN);

    }

  };

}

#endif