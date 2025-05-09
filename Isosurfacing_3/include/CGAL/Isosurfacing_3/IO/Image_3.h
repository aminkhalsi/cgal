// Copyright (c) 2022-2024 INRIA Sophia-Antipolis (France), GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL$
// $Id$
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Julian Stahl
//                 Mael Rouxel-Labbé

#ifndef CGAL_ISOSURFACING_3_IO_IMAGE_3_H
#define CGAL_ISOSURFACING_3_IO_IMAGE_3_H

#include <CGAL/license/Isosurfacing_3.h>

#include <CGAL/Isosurfacing_3/Cartesian_grid_3.h>
#include <CGAL/Isosurfacing_3/Interpolated_discrete_values_3.h>

#include <CGAL/Image_3.h>

namespace CGAL {
namespace Isosurfacing {
namespace IO {

/**
 * \ingroup IS_IO_functions_grp
 *
 * \brief extracts geometry and values from a `CGAL::Image_3`.
 *
 * The dimensions and geometric span are read from the image. The values stored
 * in the image must be of type `Geom_traits::FT` or implicitly convertible to it.
 *
 * \tparam Grid must be `CGAL::Isosurfacing::Cartesian_grid_3<GeomTraits>` whose `GeomTraits` is a model of `IsosurfacingTraits_3`
 * \tparam Values must be `CGAL::Isosurfacing::Interpolated_discrete_values_3<Grid>`
 *
 * \param image the image providing the data
 * \param grid the output grid
 * \param values the output values
 */
// We need to have the API pass us an existing grid / values pair because the values
// usually keep a reference to the grid.
template <typename Grid, typename Values>
bool convert_image_to_grid(const CGAL::Image_3& image,
                           Grid& grid,
                           Values& values)
{
  using Geom_traits = typename Grid::Geom_traits;
  using FT = typename Geom_traits::FT;
  using Iso_cuboid_3 = typename Geom_traits::Iso_cuboid_3;

  typename Geom_traits::Construct_point_3 point = grid.geom_traits().construct_point_3_object();
  typename Geom_traits::Construct_iso_cuboid_3 iso_cuboid = grid.geom_traits().construct_iso_cuboid_3_object();

  // compute span
  const FT max_x = image.tx() + (image.xdim() - 1) * image.vx();
  const FT max_y = image.ty() + (image.ydim() - 1) * image.vy();
  const FT max_z = image.tz() + (image.zdim() - 1) * image.vz();
  Iso_cuboid_3 span = iso_cuboid(point(image.tx(), image.ty(), image.tz()),
                                 point(max_x, max_y, max_z));

  // get spacing
  // std::array<FT, 3> spacing = make_array(image.vx(), image.vy(), image.vz());

  grid = Grid { span, CGAL::make_array<std::size_t>(image.xdim(), image.ydim(), image.zdim()) };

  // copy values
  for(std::size_t x=0; x<image.xdim(); ++x)
    for(std::size_t y=0; y<image.ydim(); ++y)
      for(std::size_t z=0; z<image.zdim(); ++z)
        values(x, y, z) = image.value(x, y, z);

  return true;
}

/**
 * \ingroup IS_IO_functions_grp
 *
 * \brief create a `CGAL::Image_3` from a grid and a field of values.
 *
 * \tparam Grid must be `CGAL::Isosurfacing::Cartesian_grid_3<GeomTraits>` with `GeomTraits`
 *              a model of `IsosurfacingTraits_3`
 * \tparam Values must be `CGAL::Isosurfacing::Interpolated_discrete_values_3<Grid>`
 *
 * \param grid the space partitioning data structure
 * \param values the field of values
 */
template <typename Grid, typename Values>
CGAL::Image_3 convert_grid_to_image(const Grid& grid,
                                    const Values& values)
{
  using Geom_traits = typename Grid::Geom_traits;

  using FT = typename Geom_traits::FT;
  using Point_3 = typename Geom_traits::Point_3;

  const Geom_traits& gt = grid.geom_traits();
  typename Geom_traits::Compute_x_3 x_coord = gt.compute_x_3_object();
  typename Geom_traits::Compute_y_3 y_coord = gt.compute_y_3_object();
  typename Geom_traits::Compute_z_3 z_coord = gt.compute_z_3_object();
  typename Geom_traits::Construct_vertex_3 vertex = gt.construct_vertex_3_object();

  // select number type
  WORD_KIND wordkind;
  if constexpr(std::is_floating_point_v<FT>)
    wordkind = WK_FLOAT;
  else
    wordkind = WK_FIXED;

  // select signed or unsigned
  SIGN sign;
  if constexpr(std::is_signed_v<FT>)
    sign = SGN_SIGNED;
  else
    sign = SGN_UNSIGNED;

  // get spacing
  const double vx = CGAL::to_double(grid.spacing()[0]);
  const double vy = CGAL::to_double(grid.spacing()[1]);
  const double vz = CGAL::to_double(grid.spacing()[2]);

  // create image
  _image* im = _createImage(grid.xdim(), grid.ydim(), grid.zdim(),
                            1,           // vectorial dimension
                            vx, vy, vz,  // voxel size
                            sizeof(FT),  // image word size in bytes
                            wordkind,    // image word kind WK_FIXED, WK_FLOAT, WK_UNKNOWN
                            sign);       // image word sign

  // error handling
  if(im == nullptr || im->data == nullptr)
    throw std::bad_alloc();

  // set min coordinates
  const Point_3& min_p = vertex(grid.span(), 0);
  im->tx = float(CGAL::to_double(x_coord(min_p)));
  im->ty = float(CGAL::to_double(y_coord(min_p)));
  im->tz = float(CGAL::to_double(z_coord(min_p)));

  // copy data
  FT* data = static_cast<FT*>(im->data); // @fixme what compatibility with non trivial FTs?
  for(std::size_t x=0; x<grid.xdim(); ++x) {
    for(std::size_t y=0; y<grid.ydim(); ++y) {
      for(std::size_t z=0; z<grid.zdim(); ++z)
      {
        const std::size_t lid = grid.linear_index(x, y, z);
        data[lid] = values(grid.point(lid));
      }
    }
  }

  return Image_3 { im, Image_3::OWN_THE_DATA };
}

} // namespace IO
} // namespace Isosurfacing
} // namespace CGAL

#endif // CGAL_ISOSURFACING_3_IO_IMAGE_3_H
