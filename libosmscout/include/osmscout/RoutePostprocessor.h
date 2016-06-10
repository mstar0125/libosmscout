#ifndef OSMSCOUT_ROUTEPOSTPROCESSOR_H
#define OSMSCOUT_ROUTEPOSTPROCESSOR_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2012  Tim Teulings

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

#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include <osmscout/CoreFeatures.h>

#include <osmscout/TypeConfig.h>
#include <osmscout/TypeFeatures.h>

// Database
#include <osmscout/Database.h>

// Routing
#include <osmscout/RouteData.h>
#include <osmscout/RoutingProfile.h>

namespace osmscout {

  /**
   * \ingroup Routing
   */
  class OSMSCOUT_API RoutePostprocessor
  {
  public:
    class OSMSCOUT_API Postprocessor
    {
    public:
      virtual ~Postprocessor();

      virtual bool Process(const RoutePostprocessor& postprocessor,
                           const RoutingProfile& profile,
                           RouteDescription& description,
                           Database& database) = 0;
    };

    typedef std::shared_ptr<Postprocessor> PostprocessorRef;

    /**
     * \ingroup Routing
     * Places the given description at the start node
     */
    class OSMSCOUT_API StartPostprocessor : public Postprocessor
    {
    private:
      std::string startDescription;

    public:
      StartPostprocessor(const std::string& startDescription);

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Places the given description at the target node
     */
    class OSMSCOUT_API TargetPostprocessor : public Postprocessor
    {
    private:
      std::string targetDescription;

    public:
      TargetPostprocessor(const std::string& targetDescription);

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Calculates the overall running distance and time for each node
     */
    class OSMSCOUT_API DistanceAndTimePostprocessor : public Postprocessor
    {
    public:
      DistanceAndTimePostprocessor();

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Places a name description as way description
     */
    class OSMSCOUT_API WayNamePostprocessor : public Postprocessor
    {
    public:
      WayNamePostprocessor();

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Places a crossing ways description as a description of the name of all ways crossing the given node
     */
    class OSMSCOUT_API CrossingWaysPostprocessor : public Postprocessor
    {
    private:
      void AddCrossingWaysDescriptions(const RoutePostprocessor& postprocessor,
                                       const RouteDescription::CrossingWaysDescriptionRef& description,
                                       const RouteDescription::Node& node,
                                       const ObjectFileRef& originObject,
                                       const ObjectFileRef& targetObject);

    public:
      CrossingWaysPostprocessor();

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Places a turn description for every node
     */
    class OSMSCOUT_API DirectionPostprocessor : public Postprocessor
    {
    private:
      static const double curveMinInitialAngle;
      static const double curveMaxInitialAngle;
      static const double curveMaxNodeDistance;
      static const double curveMaxDistance;
      static const double curveMinAngle;

    public:
      DirectionPostprocessor();

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Adds driving hint based on motorway_junction tags
     */
    class OSMSCOUT_API MotorwayJunctionPostprocessor : public Postprocessor
    {
    public:
      MotorwayJunctionPostprocessor();

      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);
    };

    /**
     * \ingroup Routing
     * Generates drive instructions
     */
    class OSMSCOUT_API InstructionPostprocessor : public Postprocessor
    {
    private:
      enum State {
        street,
        roundabout,
        motorway,
        link
      };

    private:
      TypeInfoSet motorwayTypes;
      TypeInfoSet motorwayLinkTypes;

      bool                     inRoundabout;
      size_t                   roundaboutCrossingCounter;

    private:
      State GetInitialState(const RoutePostprocessor& postprocessor,
                            RouteDescription::Node& node);

      void HandleRoundaboutEnter(RouteDescription::Node& node);
      void HandleRoundaboutNode(RouteDescription::Node& node);
      void HandleRoundaboutLeave(RouteDescription::Node& node);
      void HandleDirectMotorwayEnter(RouteDescription::Node& node,
                                     const RouteDescription::NameDescriptionRef& toName);
      void HandleDirectMotorwayLeave(RouteDescription::Node& node,
                                     const RouteDescription::NameDescriptionRef& fromName);
      bool HandleNameChange(const std::list<RouteDescription::Node>& path,
                            std::list<RouteDescription::Node>::const_iterator& lastNode,
                            std::list<RouteDescription::Node>::iterator& node);
      bool HandleDirectionChange(const std::list<RouteDescription::Node>& path,
                                 std::list<RouteDescription::Node>::iterator& node);

    public:
      bool Process(const RoutePostprocessor& postprocessor,
                   const RoutingProfile& profile,
                   RouteDescription& description,
                   Database& database);

      void AddMotorwayType(const TypeInfoRef& type);
      void AddMotorwayLinkType(const TypeInfoRef& type);
    };

    typedef std::shared_ptr<InstructionPostprocessor> InstructionPostprocessorRef;

  private:
    std::unordered_map<FileOffset,AreaRef> areaMap;
    std::unordered_map<FileOffset,WayRef>  wayMap;
    NameFeatureValueReader                 *nameReader;
    RefFeatureValueReader                  *refReader;
    BridgeFeatureReader                    *bridgeReader;
    RoundaboutFeatureReader                *roundaboutReader;

  private:
    bool ResolveAllAreasAndWays(const RouteDescription& description,
                                Database& database);
    void Cleanup();

  public:
    RoutePostprocessor();

    AreaRef GetArea(FileOffset offset) const;
    WayRef GetWay(FileOffset offset) const;

    RouteDescription::NameDescriptionRef GetNameDescription(const ObjectFileRef& object) const;
    RouteDescription::NameDescriptionRef GetNameDescription(const Area& area) const;
    RouteDescription::NameDescriptionRef GetNameDescription(const Way& way) const;

    bool IsRoundabout(const ObjectFileRef& object) const;
    bool IsBridge(const Way& way) const;

    bool IsOfType(const ObjectFileRef& object,
                  const TypeInfoSet& types) const;

    Id GetNodeId(const ObjectFileRef& object,
                 size_t nodeIndex) const;
    size_t GetNodeIndex(const ObjectFileRef& object,
                        Id nodeId) const;

    bool CanUseBackward(const RoutingProfile& profile,
                        Id fromNodeId,
                        const ObjectFileRef& object) const;

    bool CanUseForward(const RoutingProfile& profile,
                       Id fromNodeId,
                       const ObjectFileRef& object) const;

    bool IsBackwardPath(const ObjectFileRef& object,
                        size_t fromNodeIndex,
                        size_t toNodeIndex) const;

    bool IsForwardPath(const ObjectFileRef& object,
                       size_t fromNodeIndex,
                       size_t toNodeIndex) const;

    bool IsNodeStartOrEndOfObject(const ObjectFileRef& nodeObject,
                                  size_t nodeIndex,
                                  const ObjectFileRef& object) const;

    void GetCoordinates(const ObjectFileRef& object,
                        size_t nodeIndex,
                        double& lat,
                        double& lon) const;

    bool PostprocessRouteDescription(RouteDescription& description,
                                     const RoutingProfile& profile,
                                     Database& database,
                                     std::list<PostprocessorRef> processors);
  };
}

#endif
