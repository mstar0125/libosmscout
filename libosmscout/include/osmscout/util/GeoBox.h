#ifndef OSMSCOUT_UTIL_GEOBOX_H
#define OSMSCOUT_UTIL_GEOBOX_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2015  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/private/CoreImportExport.h>

#include <osmscout/GeoCoord.h>

namespace osmscout {

  /**
   * \ingroup Geometry
   *
   * Anonymous geographic rectangular bounding box.
   *
   * The bounding box is defined by two coordinates (type GeoCoord) that span a
   * (in coordinate space) rectangular area.
   */
  struct OSMSCOUT_API GeoBox
  {
    GeoCoord minCoord;
    GeoCoord maxCoord;

    bool valid;

    /**
     * The default constructor creates an invalid instance.
     */
    GeoBox();

    /**
     * Copy-Constructor
     */
    inline GeoBox(const GeoBox& other)
    : minCoord(other.minCoord),
      maxCoord(other.maxCoord),
      valid(other.valid)
    {
      // no code
    }


    /**
     * Initialize the GeoBox based on the given coordinates. The two Coordinates
     * together span a rectangular (in coordinates, not on the sphere) area.
     */
    GeoBox(const GeoCoord& coordA,
           const GeoCoord& coordB);

    /**
     * Invalidate the bounding Box
     */
    inline void Invalidate()
    {
      valid=false;
      minCoord.Set(0.0,0.0);
      maxCoord.Set(0.0,0.0);
    }

    /**
     * Assign a new rectangular area bases an two coordinates defining the bounds.
     */
    void Set(const GeoCoord& coordA,
             const GeoCoord& coordB);

    /**
     * Resize the bounding box to include the original bounding box and the given bounding box
     */
    void Include(const GeoBox& other);

    /**
     * Returns 'true' if coord is within the bounding box. The boundingBox interval is as an open interval
     * at one end [..[.
     */
    inline bool Includes(const GeoCoord& coord) const
    {
      return minCoord.GetLat()<=coord.GetLat() &&
             maxCoord.GetLat()>coord.GetLat() &&
             minCoord.GetLon()<=coord.GetLon() &&
             maxCoord.GetLon()>coord.GetLon();
    }

    inline bool Intersects(const GeoBox& other) const
    {
      return !(other.GetMaxLon()<minCoord.GetLon() ||
               other.GetMinLon()>=maxCoord.GetLon() ||
               other.GetMaxLat()<minCoord.GetLat() ||
               other.GetMinLat()>=maxCoord.GetLat());
    }

    /**
     * Returns true, if the GeoBox instance is valid. This means there were
     * values assigned to the box. While being valid, the rectangle spanned by
     * the coordinate might still be degraded.
     */
    inline bool IsValid() const
    {
      return valid;
    }

    /**
     * Return the coordinate with the minimum value for the lat/lon values of the area.
     */
    inline GeoCoord GetMinCoord() const
    {
      return minCoord;
    }

    /**
     * Return the coordinate with the maximum value for the lat/lon values of the area.
     */
    inline GeoCoord GetMaxCoord() const
    {
      return maxCoord;
    }

    GeoCoord GetCenter() const;

    /**
     * Return the minimum latitude of the GeBox.
     */
    inline double GetMinLat() const
    {
      return minCoord.GetLat();
    }

    /**
     * Return the minimum longitude of the GeBox.
     */
    inline double GetMinLon() const
    {
      return minCoord.GetLon();
    }

    /**
     * Return the maximum latitude of the GeBox.
     */
    inline double GetMaxLat() const
    {
      return maxCoord.GetLat();
    }

    /**
     * Return the maximum longitude of the GeBox.
     */
    inline double GetMaxLon() const
    {
      return maxCoord.GetLon();
    }

    /**
     * Returns the width of the bounding box (maxLon-minLon).
     */
    inline double GetWidth() const
    {
      return maxCoord.GetLon()-minCoord.GetLon();
    }

    /**
     * Returns the height of the bounding box (maxLat-minLat).
     */
    inline double GetHeight() const
    {
      return maxCoord.GetLat()-minCoord.GetLat();
    }

    /**
     * Return a string representation of the coordinate value in a human readable format.
     */
    std::string GetDisplayText() const;

    static GeoBox BoxByCenterAndRadius(const GeoCoord& center,double radius);
  };
}

#endif
