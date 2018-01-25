#ifndef REFUSED_BEQUEST
#define REFUSED_BEQUEST

#include <lim/inc/lim.h>
#include <graph/inc/graph.h>
#include <rul/inc/RulHandler.h>
#include <map>

#include "../LIM2Stink.h"

namespace columbus { namespace lim { namespace antipatterns {

  class RefusedBequestVisitor : public LIM2StinkVisitor {

  public:
	columbus::lim::asg::OverrideRelations& ovr;

	RefusedBequestVisitor(asg::Factory& fact, graph::Graph& inGraph, columbus::rul::RulHandler& rl, columbus::lim::asg::OverrideRelations& ovr);

    virtual ~RefusedBequestVisitor();

    /** Visit methods */
	virtual void visit(const columbus::lim::asg::logical::Method&, bool callVirtualBase = true);
	virtual void visit(const columbus::lim::asg::logical::Class&, bool callVirtualBase = true);
	virtual void visitEnd(const columbus::lim::asg::logical::Class& _node, bool callVirtualBase);


  };

}}}

#endif
