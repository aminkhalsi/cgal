// Copyright (c) 2001-2005  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL$
// $Id$
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Sylvain Pion, Andreas Fabri, Sebastien Loriot

#ifndef CGAL_FILTERED_PREDICATE_WITH_STATE_H
#define CGAL_FILTERED_PREDICATE_WITH_STATE_H

#include <string>
#include <CGAL/config.h>
#include <CGAL/Interval_nt.h>
#include <CGAL/Uncertain.h>
#include <CGAL/Profile_counter.h>
#include <optional>

namespace CGAL {

// This template class is a wrapper that implements the filtering for any
// predicate with state (dynamic filters with IA).

  template <class EP, class AP, class C2E, class C2A, class O1, bool Protection = true>
class Filtered_predicate_with_state
{
  C2E c2e;
  C2A c2a;
  O1  o1;
  mutable std::optional<EP>  oep;
  AP  ap;

public:
  // AP::result_type must be convertible to EP::result_type.
  typedef AP    Approximate_predicate;
  typedef EP    Exact_predicate;
  typedef C2E   To_exact_converter;
  typedef C2A   To_approximate_converter;

  Filtered_predicate_with_state(const O1 &o1)
    : c2e(), c2a(), o1(o1), oep(), ap(c2a(o1))
  {}

  template <typename... Args>
  auto
  operator()(const Args&... args) const
  {
    typedef typename Remove_needs_FT<CGAL::cpp20::remove_cvref_t<decltype((*oep)(c2e(args)...))> >::Type result_type;

#ifndef CGAL_EPICK_NO_INTERVALS
    typedef typename Remove_needs_FT<CGAL::cpp20::remove_cvref_t<decltype(ap(c2a(args)...))> >::Type Ares;

    CGAL_BRANCH_PROFILER(std::string(" failures/calls to   : ") + std::string(CGAL_PRETTY_FUNCTION), tmp);
    // Protection is outside the try block as VC8 has the CGAL_CFG_FPU_ROUNDING_MODE_UNWINDING_VC_BUG
    {
      Protect_FPU_rounding<Protection> p;
      try
        {
          Ares res = ap(c2a(args)...);
          if (is_certain(res))
            return result_type(get_certain(res));
        }
      catch (Uncertain_conversion_exception&) {}
    }
    CGAL_BRANCH_PROFILER_BRANCH(tmp);
    Protect_FPU_rounding<!Protection> p(CGAL_FE_TONEAREST);
    CGAL_expensive_assertion(FPU_get_cw() == CGAL_FE_TONEAREST);
#endif
    if(! oep){
      oep.emplace(c2e(o1));
    }
    return result_type((*oep)(c2e(args)...));
  }
};



} //namespace CGAL

#endif // CGAL_FILTERED_PREDICATE_WITH_STATE_H
