#ifndef SINGLETON
#define SINGLETON

#include <lim/inc/lim.h>
#include <graph/inc/graph.h>
#include <rul/inc/RulHandler.h>
#include "../LIM2Stink.h"

namespace columbus { namespace lim { namespace antipatterns {

  class SingletonVisitor : public LIM2StinkVisitor {

  public:

    SingletonVisitor(asg::Factory& fact, graph::Graph& inGraph, columbus::rul::RulHandler& rl);

    virtual ~SingletonVisitor();

    /** Visit methods */
    virtual void visit(const columbus::lim::asg::logical::Method&, bool callVirtualBase = true);
  };

}}}

#endif
