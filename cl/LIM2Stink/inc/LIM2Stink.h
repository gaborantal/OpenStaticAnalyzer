#ifndef LIM_STINK
#define LIM_STINK

#include <lim/inc/lim.h>
#include <graph/inc/graph.h>
#include <rul/inc/RulHandler.h>

namespace columbus { namespace lim { namespace antipatterns {

  class LIM2StinkVisitor : public columbus::lim::asg::VisitorAbstractNodes {

  public:

    LIM2StinkVisitor(asg::Factory& fact, graph::Graph& inGraph, columbus::rul::RulHandler& rl);

    virtual ~LIM2StinkVisitor();

  protected:
    asg::Factory& factory;

    columbus::graph::Graph& inGraph;

    columbus::rul::RulHandler& rl;

    const asg::ReverseEdges& revEdges;

    void addWarning(const columbus::lim::asg::logical::Member& member, graph::Graph& graph, graph::Node& node, const std::string& name, const std::string& text);

    const columbus::lim::asg::logical::Scope* getParentScope(const columbus::lim::asg::logical::Member& member) const;

    // graph helpers
    int extractInt(const graph::Node& gNode, const std::string& name) const;
  };


}}} //columbus::lim::bsm

#endif
