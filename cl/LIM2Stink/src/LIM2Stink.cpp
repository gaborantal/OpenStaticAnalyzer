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

	LIM2StinkVisitor::LIM2StinkVisitor(asg::Factory& fact, columbus::graph::Graph& inGraph, const set<string>& patterns, columbus::rul::RulHandler& rl)
		: factory(fact),
		inGraph(inGraph),
		patterns(patterns),
		rl(rl),
		revEdges(factory.getReverseEdges()) { }

	LIM2StinkVisitor::~LIM2StinkVisitor() {}

  //
  // Visitors
  //

  void LIM2StinkVisitor::visit(const asg::logical::Class& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
    WriteMsg::write(WriteMsg::mlDebug, "STARTED visiting class: %s\n", _node.getName().c_str());
	auto it = _node.getMemberListIteratorBegin();

	for (; it != _node.getMemberListIteratorEnd(); ++it) {
		const asg::logical::Member& memberNode = dynamic_cast<const asg::logical::Member&>(*it);

		if (asg::Common::getIsAttribute(memberNode)) {
			auto* attr = dynamic_cast<const asg::logical::Attribute*>(&memberNode);


			//

			WriteMsg::write(WriteMsg::mlNormal, "Attribute name: %s\n", attr->getName().c_str());
		} else if (asg::Common::getIsMethod(memberNode)) {
			auto* attr = dynamic_cast<const asg::logical::Method*>(&memberNode);
			WriteMsg::write(WriteMsg::mlNormal, "Method name: %s\n", attr->getName().c_str());
		}
	}

    // asg::Common::collectDescendants( revEdges, _node, std::set<const logical::Class*>& descendants, InheritanceCache* cache)


    WriteMsg::write(WriteMsg::mlDebug, "ENDED visiting class: %s\n", _node.getName().c_str());
  }

  void LIM2StinkVisitor::visitEnd(const asg::logical::Class& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visitEnd(_node);
    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[START] (Class):  %s\n", _node.getName().c_str());


    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[END]   (Class):  %s\n", _node.getName().c_str());
  }


  void LIM2StinkVisitor::visit(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
    WriteMsg::write( WriteMsg::mlDebug, "Visit   [START] (Method): %s\n", _node.getName().c_str());



    WriteMsg::write( WriteMsg::mlDebug, "Visit   [END]   (Method): %s\n", _node.getName().c_str());
  }

  void LIM2StinkVisitor::visitEnd(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visitEnd(_node);
    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[EMPTY] (Method): %s\n", _node.getName().c_str());
  }

  //
  // Helpers
  //

  void LIM2StinkVisitor::addWarning( const asg::logical::Member& member, graph::Graph& graph, graph::Node& node, const string& name, const string& text) {
	// do not process warnings for the same TUID twice
	graph::Attribute::AttributeIterator aIt = node.findAttributeByName("TUID");
	if (!aIt.hasNext()) {
		cout << "Node without TUID, skip" << endl;
		return;
	}
	
	string tuid = ((graph::AttributeString&)aIt.next()).getValue();

    asg::ListIteratorAssocSourcePosition<asg::physical::File> posIt = member.getIsContainedInListIteratorAssocBegin();
 
    string path = posIt->getName();
    asg::SourcePosition sp = posIt.getAssocClass();

    graphsupport::addWarning(graph, node, name, path, sp.getLine(), sp.getColumn(), sp.getEndLine(), sp.getEndColumn(), text);

	WriteMsg::write(WriteMsg::mlNormal, "[NODE WARNING] %s(%d): %s %s\n", path.c_str(), sp.getLine(), name.c_str(), text.c_str());
  }

  const asg::logical::Scope* LIM2StinkVisitor::getParentScope(const asg::logical::Member& member) const {
	  asg::ListIterator<asg::base::Base> it = revEdges.constIteratorBegin(member.getId(), asg::edkScope_HasMember);
	  if (it != revEdges.constIteratorEnd(member.getId(), asg::edkScope_HasMember)) {
		  return dynamic_cast<const asg::logical::Scope*>(&(*it));
	  }
	  return 0;
  }


  int LIM2StinkVisitor::extractInt(const graph::Node& gNode, const string& name) const {
	  graph::Attribute::AttributeIterator aIt = gNode.findAttributeByName(name);
	  if (aIt.hasNext()) {
		  return (int)((graph::AttributeInt&)aIt.next()).getValue();
	  }
	  return 0;
  }

}}}
