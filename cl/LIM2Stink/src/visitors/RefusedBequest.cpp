#include "../../inc/visitors/RefusedBequest.h"
#include <lim2graph/inc/VisitorGraphConverter.h>
#include <common/inc/WriteMessage.h>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <iterator>

using namespace std;
using namespace common;
using namespace columbus::lim;

namespace columbus { namespace lim { namespace antipatterns {

  RefusedBequestVisitor::RefusedBequestVisitor(asg::Factory& fact, columbus::graph::Graph& inGraph, columbus::rul::RulHandler& rl, columbus::lim::asg::OverrideRelations& ovr)
    : LIM2StinkVisitor(fact, inGraph, rl), ovr(ovr) { }

  RefusedBequestVisitor::~RefusedBequestVisitor() {}

  void RefusedBequestVisitor::visit(const asg::logical::Class& _node, bool callVirtualBase) {
	  std::set<const columbus::lim::asg::logical::Class*> descendants;
	  std::set<const columbus::lim::asg::logical::Member*> protectedMembers;
	  std::set<const columbus::lim::asg::logical::Member*> tmpProtectedMembers;

	  asg::Common::collectDescendants(revEdges, _node, descendants);

	  // Collect protected members
	  auto it = _node.getMemberListIteratorBegin();
	  for (; it != _node.getMemberListIteratorEnd(); ++it) {
		  auto& memberNode  = dynamic_cast<const asg::logical::Member&>(*it);
		  if (memberNode.getAccessibility() == asg::ackProtected) {
			  protectedMembers.insert(&memberNode);
			  tmpProtectedMembers.insert(&memberNode);
		  }
	  }

	  if (protectedMembers.size() == 0) {
		  return;
	  }

	 /*
	 Overrides are empty :'( 
	 auto overrides = or.getOverrideList(_node.getId());

	  WriteMsg::write(WriteMsg::mlNormal, "[RB] OVERRIDEs %d \n", overrides.size());
	  for (auto o : overrides) {
		  auto t = factory.getPointer(o);
		  auto& k = dynamic_cast<const asg::logical::Method&>(*t);
		  WriteMsg::write(WriteMsg::mlNormal, "[RB] OVERRIDEs %s uses\n", k.getName().c_str());  
	  }
	  */


	  // Collect which protected members are used by descendants
	  for (auto desc : descendants) {
		  auto memberIt = desc->getMemberListIteratorBegin();
		  for (; memberIt != desc->getMemberListIteratorEnd(); ++memberIt) {
			  auto& mNode = dynamic_cast<const asg::logical::Member&>(*memberIt);
			  if (asg::Common::getIsMethod(*memberIt)) {
				  auto& descMethod = dynamic_cast<const asg::logical::Method&>(*memberIt);

				  auto abc = descMethod.getAccessesAttributeListIteratorBegin();
				  // Collect which protected members are used by descendats
				  for (; abc != descMethod.getAccessesAttributeListIteratorEnd(); ++abc) {
					  auto theAttr = abc->getAttribute();
					  if (protectedMembers.find(theAttr) != protectedMembers.end()) {
						  tmpProtectedMembers.erase(theAttr);
						  WriteMsg::write(WriteMsg::mlDebug, "[RB] method %s uses %s\n", descMethod.getName().c_str(), theAttr->getName().c_str());
					  }
				  }

				  // Collect which protected methods are used by descendats
				  auto xyz = descMethod.getCallsListIteratorBegin();
				  for (; xyz != descMethod.getCallsListIteratorEnd(); ++xyz) {
					  auto theMethod = xyz->getMethod();

					  if (protectedMembers.find(theMethod) != protectedMembers.end()) {
						  tmpProtectedMembers.erase(theMethod);
						  WriteMsg::write(WriteMsg::mlDebug, "[RB] method %s uses %s\n", descMethod.getName().c_str(), theMethod->getName().c_str());
					  }
				  }
			  }
		  }

		  float requiredPercent = atof(rul.getSettingValue("RefusedBequest", "Min").data());
		  float actualPercent = 1.0 - (float)tmpProtectedMembers.size() / protectedMembers.size();
		  if (actualPercent < requiredPercent) {
			  LIM2StinkVisitor::addWarning(_node, "RefusedBequest", desc->getName() + " refuses the bequest of " + _node.getName());
		  }
		  WriteMsg::write(WriteMsg::mlDebug, "[RB] %d of %d members were not used in the descendat %s means %.3f (required %.3f)\n", tmpProtectedMembers.size(), protectedMembers.size(), desc->getName().c_str(), actualPercent, requiredPercent);
		  tmpProtectedMembers = protectedMembers;
	  }

  }

  void RefusedBequestVisitor::visit(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
  }

  void RefusedBequestVisitor::visitEnd(const asg::logical::Class& _node, bool callVirtualBase) {

  }

}}}
