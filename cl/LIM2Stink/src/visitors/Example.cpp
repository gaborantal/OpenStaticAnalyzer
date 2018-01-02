#include "../../inc/visitors/ExampleVisitor.h"
#include <common/inc/WriteMessage.h>
#include <sstream>
#include <cstdio>
#include <iostream>


using namespace std;
using namespace common;
using namespace columbus::lim;

namespace columbus { namespace lim { namespace antipatterns {

  ExampleVisitor::ExampleVisitor(asg::Factory& fact, columbus::graph::Graph& inGraph, columbus::rul::RulHandler& rl)
    : LIM2StinkVisitor(fact, inGraph, rl) { }

  ExampleVisitor::~ExampleVisitor() {}

  void ExampleVisitor::visit(const asg::logical::Class& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
    WriteMsg::write(WriteMsg::mlDebug, "STARTED visiting class: %s\n", _node.getName().c_str());

    auto it = _node.getMemberListIteratorBegin();

    for (; it != _node.getMemberListIteratorEnd(); ++it) {
      const asg::logical::Member& memberNode = dynamic_cast<const asg::logical::Member&>(*it);

      if (asg::Common::getIsAttribute(memberNode)) {
        auto* attr = dynamic_cast<const asg::logical::Attribute*>(&memberNode);
        WriteMsg::write(WriteMsg::mlNormal, "Attribute name: %s\n", attr->getName().c_str());
      } else if (asg::Common::getIsMethod(memberNode)) {
        auto* attr = dynamic_cast<const asg::logical::Method*>(&memberNode);
        WriteMsg::write(WriteMsg::mlNormal, "Method name: %s\n", attr->getName().c_str());
      }
    }

    // asg::Common::collectDescendants( revEdges, _node, std::set<const logical::Class*>& descendants, InheritanceCache* cache)

    WriteMsg::write(WriteMsg::mlDebug, "ENDED visiting class: %s\n", _node.getName().c_str());
  }

  void ExampleVisitor::visitEnd(const asg::logical::Class& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visitEnd(_node);
    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[START] (Class):  %s\n", _node.getName().c_str());


    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[END]   (Class):  %s\n", _node.getName().c_str());
  }


  void ExampleVisitor::visit(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
    WriteMsg::write( WriteMsg::mlDebug, "Visit   [START] (Method): %s\n", _node.getName().c_str());



    WriteMsg::write( WriteMsg::mlDebug, "Visit   [END]   (Method): %s\n", _node.getName().c_str());
  }

  void ExampleVisitor::visitEnd(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visitEnd(_node);
    WriteMsg::write( WriteMsg::mlDebug, "VisitEnd[EMPTY] (Method): %s\n", _node.getName().c_str());
  }
}}}
