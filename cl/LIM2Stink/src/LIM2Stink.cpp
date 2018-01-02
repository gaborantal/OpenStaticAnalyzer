#include "../inc/LIM2Stink.h"
#include <lim2graph/inc/VisitorGraphConverter.h>
#include <common/inc/WriteMessage.h>
#include <graphsupport/inc/Metric.h>
#include <sstream>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace common;
using namespace columbus::lim;

namespace columbus { namespace lim { namespace antipatterns {

  LIM2StinkVisitor::LIM2StinkVisitor(asg::Factory& fact, columbus::graph::Graph& limGraph, columbus::rul::RulHandler& rul)
    : factory(fact),
    limGraph(limGraph),
    rul(rul),
    revEdges(factory.getReverseEdges()) { }

  LIM2StinkVisitor::~LIM2StinkVisitor() {}

  void LIM2StinkVisitor::addWarning( const asg::logical::Member& member, const string& name, const string& text) {
    string graphUID = lim2graph::VisitorGraphConverter::determineNodeName(factory.getRef(member.getId()));
    graph::Node node = limGraph.findNode(graphUID);

    if (node != graph::Graph::invalidNode) {
      asg::ListIteratorAssocSourcePosition<asg::physical::File> posIt = member.getIsContainedInListIteratorAssocBegin();

      string path = posIt->getName();
      asg::SourcePosition sp = posIt.getAssocClass();

      graphsupport::addWarning(limGraph, node, name, path, sp.getLine(), sp.getColumn(), sp.getEndLine(), sp.getEndColumn(), text);
      WriteMsg::write(WriteMsg::mlNormal, "[NODE WARNING] %s(%d): %s %s\n", path.c_str(), sp.getLine(), name.c_str(), text.c_str());
    }
  }

  const asg::logical::Scope& LIM2StinkVisitor::getParentScope(const asg::logical::Member& member) const {
    asg::ListIterator<asg::base::Base> it = revEdges.constIteratorBegin(member.getId(), asg::edkScope_HasMember);
    if (it != revEdges.constIteratorEnd(member.getId(), asg::edkScope_HasMember)) {
      return dynamic_cast<const asg::logical::Scope&>(*it);
    }
    // Does not have parent scope
    return dynamic_cast<const asg::logical::Scope&>(member);
  }
}}}
