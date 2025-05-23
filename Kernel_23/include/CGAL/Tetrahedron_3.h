// Copyright (c) 1999
// Utrecht University (The Netherlands),
// ETH Zurich (Switzerland),
// INRIA Sophia-Antipolis (France),
// Max-Planck-Institute Saarbruecken (Germany),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL$
// $Id$
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Andreas Fabri, Stefan Schirra

#ifndef CGAL_TETRAHEDRON_3_H
#define CGAL_TETRAHEDRON_3_H

#include <CGAL/assertions.h>
#include <CGAL/Kernel/Return_base_tag.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Dimension.h>

#include <type_traits>

namespace CGAL {

template <class R_>
class Tetrahedron_3 : public R_::Kernel_base::Tetrahedron_3
{
  typedef typename R_::Boolean               Boolean;
  typedef typename R_::Bounded_side          Bounded_side;
  typedef typename R_::Orientation           Orientation;
  typedef typename R_::Oriented_side         Oriented_side;
  typedef typename R_::Point_3               Point_3;
  typedef typename R_::Aff_transformation_3  Aff_transformation_3;

  typedef Tetrahedron_3                      Self;
  static_assert(std::is_same<Self, typename R_::Tetrahedron_3>::value);

public:

  typedef Dimension_tag<3>  Ambient_dimension;
  typedef Dimension_tag<3>  Feature_dimension;

  typedef typename R_::Kernel_base::Tetrahedron_3  Rep;

  const Rep& rep() const
  {
    return *this;
  }

  Rep& rep()
  {
    return *this;
  }

  typedef          R_                       R;

  Tetrahedron_3() {}

  Tetrahedron_3(const Rep& t)
      : Rep(t) {}

  Tetrahedron_3(Rep&& t)
      : Rep(std::move(t)) {}

  Tetrahedron_3(const Point_3& p, const Point_3& q,
                const Point_3& r, const Point_3& s)
    : Rep(typename R::Construct_tetrahedron_3()(Return_base_tag(), p, q, r, s)) {}

  Tetrahedron_3 transform(const Aff_transformation_3 &t) const
  {
    return Tetrahedron_3(t.transform(this->vertex(0)),
                         t.transform(this->vertex(1)),
                         t.transform(this->vertex(2)),
                         t.transform(this->vertex(3)));
  }

  decltype(auto)
  vertex(int i) const
  {
    return R().construct_vertex_3_object()(*this,i);
  }

  decltype(auto)
  operator[](int i) const
  {
    return vertex(i);
  }

  Boolean is_degenerate() const
  {
    return R().is_degenerate_3_object()(*this);
  }

  Orientation orientation() const
  {
    return R().orientation_3_object()(*this);
  }

  Bounded_side bounded_side(const Point_3 &p) const
  {
    return R().bounded_side_3_object()(*this, p);
  }

  Oriented_side oriented_side(const Point_3 &p) const
  {
    return R().oriented_side_3_object()(*this, p);
  }

  Boolean has_on_positive_side(const Point_3 &p) const
  {
    return R().has_on_positive_side_3_object()(*this, p);
  }

  Boolean has_on_negative_side(const Point_3 &p) const
  {
    return R().has_on_negative_side_3_object()(*this, p);
  }

  Boolean has_on_boundary(const Point_3 &p) const
  {
    return R().has_on_boundary_3_object()(*this, p);
  }

  Boolean has_on_bounded_side(const Point_3 &p) const
  {
    return R().has_on_bounded_side_3_object()(*this, p);
  }

  Boolean has_on_unbounded_side(const Point_3 &p) const
  {
    return R().has_on_unbounded_side_3_object()(*this, p);
  }

  decltype(auto)
  volume() const
  {
    return R().compute_volume_3_object()(*this);
  }

  Bbox_3
  bbox() const
  {
    return R().construct_bbox_3_object()(*this);
  }

};


template < class R >
std::ostream &
operator<<(std::ostream &os, const Tetrahedron_3<R> &t)
{
    switch(IO::get_mode(os)) {
    case IO::ASCII :
        return os << t[0] << ' ' << t[1] << ' ' << t[2] << ' ' << t[3];
    case IO::BINARY :
        return os << t[0]  << t[1]  << t[2] << t[3];
    default:
        os << "Tetrahedron_3(" << t[0] <<  ", " << t[1] <<   ", " << t[2];
        os <<  ", " << t[3] << ")";
        return os;
    }
}

template < class R >
std::istream &
operator>>(std::istream &is, Tetrahedron_3<R> &t)
{
    typename R::Point_3 p, q, r, s;

    is >> p >> q >> r >> s;

    if (is)
        t = Tetrahedron_3<R>(p, q, r, s);
    return is;
}

} //namespace CGAL

#endif  // CGAL_TETRAHEDRON_3_H
