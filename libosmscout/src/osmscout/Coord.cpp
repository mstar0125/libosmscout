/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

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

#include <osmscout/Coord.h>

namespace osmscout {

  Id Coord::GetOSMScoutId() const
  {
    Id id;

    uint64_t latValue=(uint64_t)round((coord.GetLat()+90.0)*latConversionFactor);
    uint64_t lonValue=(uint64_t)round((coord.GetLon()+180.0)*lonConversionFactor);

    id=((latValue & 0x000000ff) <<  8)+  // 0 => 8
       ((lonValue & 0x000000ff) <<  0)+  // 0 => 0
       ((latValue & 0x0000ff00) << 16)+  // 8 => 24
       ((lonValue & 0x0000ff00) <<  8)+  // 8 => 16
       ((latValue & 0x00ff0000) << 24)+  // 16 => 40
       ((lonValue & 0x00ff0000) << 16)+  // 16 => 32
       ((latValue & 0x07000000) << 27)+  // 24 => 51
       ((lonValue & 0x07000000) << 24);  // 24 => 48

    id=id << 8;

    id|=serial;

    return id;
  }

  Id Coord::GetHash() const
  {
    uint64_t latValue=(uint64_t)round((coord.GetLat()+90.0)*latConversionFactor);
    uint64_t lonValue=(uint64_t)round((coord.GetLon()+180.0)*lonConversionFactor);
    uint64_t number=0;

    for (size_t i=0; i<27; i++) {
      size_t bit=26-i;

      number=number << 1;
      number=number+((latValue >> bit) & 0x01);

      number=number << 1;
      number=number+((lonValue >> bit) & 0x01);

      number=number << 8;
      number=number | serial;
    }

    return number;
  }
}
