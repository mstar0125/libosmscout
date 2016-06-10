/*
  Tiles - a demo program for libosmscout
  Copyright (C) 2011  Tim Teulings

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <iomanip>
#include <limits>

#include <osmscout/Database.h>
#include <osmscout/MapService.h>

#include <osmscout/MapPainterAgg.h>

#include <osmscout/util/StopClock.h>
#include <osmscout/util/Tiling.h>

/*
  Example for the nordrhein-westfalen.osm (to be executed in the Demos top
  level directory), drawing the "Ruhrgebiet":

  src/Tiler ../maps/nordrhein-westfalen ../stylesheets/standard.oss 51.2 6.5 51.7 8 10 13
*/

static unsigned int tileWidth=256;
static unsigned int tileHeight=256;
static const double DPI=96.0;

bool write_ppm(const agg::rendering_buffer& buffer,
               const char* file_name)
{
  FILE* fd=fopen(file_name, "wb");

  if (fd) {
    fprintf(fd,"P6 %d %d 255\n", buffer.width(),buffer.height());

    for (size_t y=0; y<buffer.height();y++)
    {
      const unsigned char* row=buffer.row_ptr(y);

      fwrite(row,1,buffer.width()*3,fd);
    }

    fclose(fd);
    return true;
  }

  return false;
}

int main(int argc, char* argv[])
{
  std::string  map;
  std::string  style;
  double       latTop,latBottom,lonLeft,lonRight;
  unsigned int startLevel;
  unsigned int endLevel;

  if (argc!=9) {
    std::cerr << "DrawMap ";
    std::cerr << "<map directory> <style-file> ";
    std::cerr << "<lat_top> <lon_left> <lat_bottom> <lon_right> ";
    std::cerr << "<start_zoom>" << std::endl;
    std::cerr << "<end_zoom>" << std::endl;
    return 1;
  }

  map=argv[1];
  style=argv[2];

  if (sscanf(argv[3],"%lf",&latTop)!=1) {
    std::cerr << "lon is not numeric!" << std::endl;
    return 1;
  }

  if (sscanf(argv[4],"%lf",&lonLeft)!=1) {
    std::cerr << "lat is not numeric!" << std::endl;
    return 1;
  }

  if (sscanf(argv[5],"%lf",&latBottom)!=1) {
    std::cerr << "lon is not numeric!" << std::endl;
    return 1;
  }

  if (sscanf(argv[6],"%lf",&lonRight)!=1) {
    std::cerr << "lat is not numeric!" << std::endl;
    return 1;
  }

  if (sscanf(argv[7],"%u",&startLevel)!=1) {
    std::cerr << "start zoom is not numeric!" << std::endl;
    return 1;
  }

  if (sscanf(argv[8],"%u",&endLevel)!=1) {
    std::cerr << "end zoom is not numeric!" << std::endl;
    return 1;
  }

  osmscout::DatabaseParameter databaseParameter;
  osmscout::DatabaseRef       database=std::make_shared<osmscout::Database>(databaseParameter);
  osmscout::MapServiceRef     mapService=std::make_shared<osmscout::MapService>(database);

  if (!database->Open(map.c_str())) {
    std::cerr << "Cannot open database" << std::endl;

    return 1;
  }

  osmscout::StyleConfigRef styleConfig=std::make_shared<osmscout::StyleConfig>(database->GetTypeConfig());

  if (!styleConfig->Load(style)) {
    std::cerr << "Cannot open style" << std::endl;
  }

  osmscout::TileProjection      projection;
  osmscout::MapParameter        drawParameter;
  osmscout::AreaSearchParameter searchParameter;
  osmscout::MapData             data;

  // Change this, to match your system
  drawParameter.SetFontName("/usr/share/fonts/truetype/msttcorefonts/Verdana.ttf");
  drawParameter.SetFontName("/usr/share/fonts/TTF/DejaVuSans.ttf");
  drawParameter.SetFontSize(5.0);
  // Fadings make problems with tile approach, we disable it
  drawParameter.SetDrawFadings(false);
  // To get accurate label drawing at tile borders, we take into account labels
  // of other than the current tile, too.
  drawParameter.SetDropNotVisiblePointLabels(false);

  searchParameter.SetUseLowZoomOptimization(false);
  searchParameter.SetMaximumAreaLevel(3);

  osmscout::MapPainterAgg painter(styleConfig);

  for (size_t level=std::min(startLevel,endLevel);
       level<=std::max(startLevel,endLevel);
       level++) {
    osmscout::Magnification magnification;
    int                     xTileStart,xTileEnd,xTileCount,yTileStart,yTileEnd,yTileCount;

    magnification.SetLevel(level);

    xTileStart=osmscout::LonToTileX(std::min(lonLeft,lonRight),
                                    magnification);
    xTileEnd=osmscout::LonToTileX(std::max(lonLeft,lonRight),
                                  magnification);
    xTileCount=xTileEnd-xTileStart+1;

    yTileStart=osmscout::LatToTileY(std::max(latTop,latBottom),
                                    magnification);
    yTileEnd=osmscout::LatToTileY(std::min(latTop,latBottom),
                                  magnification);

    yTileCount=yTileEnd-yTileStart+1;

    std::cout << "Drawing zoom " << level << ", " << (xTileCount)*(yTileCount) << " tiles [" << xTileStart << "," << yTileStart << " - " <<  xTileEnd << "," << yTileEnd << "]" << std::endl;

    unsigned long bitmapSize=tileWidth*tileHeight*3*xTileCount*yTileCount;
    unsigned char *buffer=new unsigned char[bitmapSize];


    memset(buffer,0,bitmapSize);

    agg::rendering_buffer rbuf(buffer,
                               tileWidth*xTileCount,
                               tileHeight*yTileCount,
                               tileWidth*xTileCount*3);

    double minTime=std::numeric_limits<double>::max();
    double maxTime=0.0;
    double totalTime=0.0;

    osmscout::TypeInfoSet nodeTypes;
    osmscout::TypeInfoSet wayTypes;
    osmscout::TypeInfoSet areaTypes;

    styleConfig->GetNodeTypesWithMaxMag(magnification,
                                        nodeTypes);

    styleConfig->GetWayTypesWithMaxMag(magnification,
                                       wayTypes);

    styleConfig->GetAreaTypesWithMaxMag(magnification,
                                        areaTypes);

    for (int y=yTileStart; y<=yTileEnd; y++) {
      for (int x=xTileStart; x<=xTileEnd; x++) {
        agg::pixfmt_rgb24   pf(rbuf);
        osmscout::StopClock timer;
        osmscout::GeoBox    boundingBox;

        projection.Set(x,y,
                       magnification,
                       DPI,
                       tileWidth,
                       tileHeight);

        projection.GetDimensions(boundingBox);

        std::cout << "Drawing tile " << level << "." << y << "." << x << " " << boundingBox.GetDisplayText() << std::endl;

        osmscout::GeoBox dataBoundingBox(osmscout::GeoCoord(osmscout::TileYToLat(y-1,magnification),osmscout::TileXToLon(x-1,magnification)),
                                         osmscout::GeoCoord(osmscout::TileYToLat(y+1,magnification),osmscout::TileXToLon(x+1,magnification)));

        std::list<osmscout::TileRef> tiles;

        mapService->LookupTiles(magnification,dataBoundingBox,tiles);
        mapService->LoadMissingTileData(searchParameter,*styleConfig,tiles);
        mapService->ConvertTilesToMapData(tiles,data);

        size_t bufferOffset=xTileCount*tileWidth*3*(y-yTileStart)*tileHeight+
                            (x-xTileStart)*tileWidth*3;

        rbuf.attach(buffer+bufferOffset,
                    tileWidth,tileHeight,
                    tileWidth*xTileCount*3);

        painter.DrawMap(projection,
                        drawParameter,
                        data,
                        &pf);

        timer.Stop();

        double time=timer.GetMilliseconds();

        minTime=std::min(minTime,time);
        maxTime=std::max(maxTime,time);
        totalTime+=time;

        std::string output=osmscout::NumberToString(level)+"_"+osmscout::NumberToString(x)+"_"+osmscout::NumberToString(y)+".ppm";

        write_ppm(rbuf,output.c_str());
      }
    }

    rbuf.attach(buffer,
                tileWidth*xTileCount,
                tileHeight*yTileCount,
                tileWidth*xTileCount*3);

    std::string output=osmscout::NumberToString(level)+"_full_map.ppm";

    write_ppm(rbuf,output.c_str());

    delete[] buffer;

    std::cout << "=> Time: ";
    std::cout << "total: " << totalTime << " msec ";
    std::cout << "min: " << minTime << " msec ";
    std::cout << "avg: " << totalTime/(xTileCount*yTileCount) << " msec ";
    std::cout << "max: " << maxTime << " msec" << std::endl;
  }

  database->Close();

  return 0;
}
