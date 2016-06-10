#ifndef OSMSCOUT_IMPORT_GENAREAWAYINDEX_H
#define OSMSCOUT_IMPORT_GENAREAWAYINDEX_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2011  Tim Teulings

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

#include <osmscout/import/Import.h>

#include <list>
#include <map>

#include <osmscout/Pixel.h>

#include <osmscout/util/FileWriter.h>
#include <osmscout/util/Geometry.h>

namespace osmscout {

  class AreaWayIndexGenerator : public ImportModule
  {
  private:
    typedef std::map<Pixel,size_t>                 CoordCountMap;
    typedef std::map<Pixel,std::list<FileOffset> > CoordOffsetsMap;

    struct TypeData
    {
      uint32_t   indexLevel;   //! magnification level of index
      size_t     indexCells;   //! Number of filled cells in index
      size_t     indexEntries; //! Number of entries over all cells

      uint32_t   cellXStart;
      uint32_t   cellXEnd;
      uint32_t   cellYStart;
      uint32_t   cellYEnd;
      uint32_t   cellXCount;
      uint32_t   cellYCount;

      FileOffset indexOffset; //! Position in file where the offset of the bitmap is written to

      TypeData();

      inline bool HasEntries()
      {
        return indexCells>0 &&
               indexEntries>0;
      }
    };

  private:
    bool FitsIndexCriteria(const ImportParameter& parameter,
                           Progress& progress,
                           const TypeInfo& typeInfo,
                           const TypeData& typeData,
                           const CoordCountMap& cellFillCount) const;

    void CalculateStatistics(size_t level,
                             TypeData& typeData,
                             const CoordCountMap& cellFillCount) const;

    bool CalculateDistribution(const TypeConfig& typeConfig,
                               const ImportParameter& parameter,
                               Progress& progress,
                               std::vector<TypeData>& wayTypeData,
                               size_t& maxLevel) const;

    bool WriteBitmap(Progress& progress,
                     FileWriter& writer,
                     const TypeInfo& typeInfo,
                     const TypeData& typeData,
                     const CoordOffsetsMap& typeCellOffsets);

  public:
    void GetDescription(const ImportParameter& parameter,
                        ImportModuleDescription& description) const;

    bool Import(const TypeConfigRef& typeConfig,
                const ImportParameter& parameter,
                Progress& progress);
  };
}

#endif
