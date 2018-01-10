#include "../../inc/visitors/TemporaryField.h"
#include <common/inc/WriteMessage.h>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <iterator>

using namespace std;
using namespace common;
using namespace columbus::lim;

namespace columbus { namespace lim { namespace antipatterns {

	TemporaryFieldVisitor::TemporaryFieldVisitor(asg::Factory& fact, columbus::graph::Graph& inGraph, columbus::rul::RulHandler& rl)
    : LIM2StinkVisitor(fact, inGraph, rl) { }

  TemporaryFieldVisitor::~TemporaryFieldVisitor() {}

  void TemporaryFieldVisitor::visit(const asg::logical::Class& _node, bool callVirtualBase) {
	  auto it = _node.getMemberListIteratorBegin();
	  for (; it != _node.getMemberListIteratorEnd(); ++it) {
		  auto& member = dynamic_cast<const asg::logical::Member&>(*it);
		  
		  if (asg::Common::getIsMethod(member)) {
			  auto m = dynamic_cast<const asg::logical::Method*>(&member);
			  classOfMethods[m->getId()] = _node.getId();
		  }
	  }

  }

  void TemporaryFieldVisitor::visit(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
	
	if (_node.getAccessesAttributeIsEmpty()) {
		return;
	}

	auto it = _node.getAccessesAttributeListIteratorBegin();
	for (; it != _node.getAccessesAttributeListIteratorEnd(); ++it) {
		asg::Attribute* attr = it->getAttribute();
		if (!attr) {
			continue;
		}
		auto classOfMethod = classOfMethods[_node.getId()];

		if (attr->getAccessibility() == asg::ackPublic ||
			attr->getAccessibility() == asg::ackProtected ||
			attr->getAccessibility() == asg::ackPackagePrivate) {

			WriteMsg::write(WriteMsg::mlDebug, "TemporaryField Visitor ATTR Node %s is public\n", attr->getName().c_str());
			continue;
		}

		if (attr->getIsStatic()) {
			WriteMsg::write(WriteMsg::mlDebug, "TemporaryField Visitor ATTR Node %s is static\n", attr->getName().c_str());
			continue;
		}
		
		if (accMap[classOfMethod].count(attr) == 0) {
			accMap[classOfMethod][attr] = 1;
		}
		else {
			accMap[classOfMethod][attr] = accMap[classOfMethod][attr]+1;
		}
	}
  }

  void TemporaryFieldVisitor::visitEnd(const asg::logical::Class& _node, bool callVirtualBase) {

	  for (auto& kv : accMap[_node.getId()]) {
		  WriteMsg::write(WriteMsg::mlDebug, "%s used from %d places\n", kv.first->getName().c_str(), kv.second);

		  if (kv.second < 2) {
			  LIM2StinkVisitor::addWarning(*kv.first, "TemporaryField", kv.first->getName() + " is a temporary field.");
		  }
	  }
	  accMap[_node.getId()].clear();
  }

}}}
