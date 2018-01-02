#ifndef LIM2STINK_EXAMPLE
#define LIM2STINK_EXAMPLE

#include <lim/inc/lim.h>
#include <graph/inc/graph.h>
#include <rul/inc/RulHandler.h>
#include "../LIM2Stink.h"

namespace columbus { namespace lim { namespace antipatterns {

  class ExampleVisitor : public LIM2StinkVisitor {

  public:

    ExampleVisitor(asg::Factory& fact, graph::Graph& inGraph, columbus::rul::RulHandler& rl);

    virtual ~ExampleVisitor();

    /** Visit methods */
    virtual void visit(const columbus::lim::asg::logical::Class&, bool callVirtualBase = true);
    virtual void visitEnd(const columbus::lim::asg::logical::Class&, bool callVirtualBase = true);
    virtual void visit(const columbus::lim::asg::logical::Method&, bool callVirtualBase = true);
    virtual void visitEnd(const columbus::lim::asg::logical::Method&, bool callVirtualBase = true);
  };

}}}

#endif
