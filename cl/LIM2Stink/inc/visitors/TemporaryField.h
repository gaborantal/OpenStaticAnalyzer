#ifndef TEMPORARY_FIELD
#define TEMPORARY_FIELD

#include <lim/inc/lim.h>
#include <graph/inc/graph.h>
#include <rul/inc/RulHandler.h>
#include "../LIM2Stink.h"
#include <map>

namespace columbus { namespace lim { namespace antipatterns {

  class TemporaryFieldVisitor : public LIM2StinkVisitor {

  public:
	std::map<unsigned int, std::map<columbus::lim::asg::logical::Attribute*, int>> accMap;

	std::map<unsigned int, unsigned int> classOfMethods;

	TemporaryFieldVisitor(asg::Factory& fact, graph::Graph& inGraph, columbus::rul::RulHandler& rl);

    virtual ~TemporaryFieldVisitor();

    /** Visit methods */
	virtual void visit(const columbus::lim::asg::logical::Method&, bool callVirtualBase = true);
	virtual void visit(const columbus::lim::asg::logical::Class&, bool callVirtualBase = true);
	virtual void visitEnd(const columbus::lim::asg::logical::Class& _node, bool callVirtualBase);

	//virtual void visit(const columbus::lim::asg::logical::Attribute& _node, bool callVirtualBase = true);

  };

}}}

#endif
