#include "../../inc/visitors/Singleton.h"
#include <common/inc/WriteMessage.h>
#include <sstream>
#include <cstdio>
#include <iostream>

using namespace std;
using namespace common;
using namespace columbus::lim;

namespace columbus { namespace lim { namespace antipatterns {

  SingletonVisitor::SingletonVisitor(asg::Factory& fact, columbus::graph::Graph& inGraph, columbus::rul::RulHandler& rl)
    : LIM2StinkVisitor(fact, inGraph, rl) { }

  SingletonVisitor::~SingletonVisitor() {}

  void SingletonVisitor::visit(const asg::logical::Method& _node, bool callVirtualBase) {
    VisitorAbstractNodes::visit(_node);
    WriteMsg::write( WriteMsg::mlDebug, "Visit   [START] (Method): %s\n", _node.getName().c_str());

    // Looking for private or protected constructors
    if (
      _node.getMethodKind() == asg::mekConstructor &&
      ( _node.getAccessibility() == asg::ackPrivate ||
        _node.getAccessibility() == asg::ackProtected )
      ) {
      WriteMsg::write(WriteMsg::mlDebug, "Protected/private constructor found: %s\n", _node.getName().c_str());

      // Check for calls to this constructor (reverse edge)
      asg::ListIterator<asg::base::Base> it = revEdges.constIteratorBegin(_node.getId(), asg::edkMethodCall_Method);
      if (it != revEdges.constIteratorEnd(_node.getId(), asg::edkMethodCall_Method)) {
        const asg::logical::MethodCall& methodCall = dynamic_cast<const asg::logical::MethodCall&>(*it);

        // Get caller method (reverse edge)
        asg::ListIterator<asg::base::Base> it2 = revEdges.constIteratorBegin(methodCall.getId(), asg::edkMethod_Calls);
        if (it2 != revEdges.constIteratorEnd(methodCall.getId(), asg::edkMethod_Calls)) {
          const asg::logical::Method& callerMethod = dynamic_cast<const asg::logical::Method&>(*it2);

          // Check if the caller is static
          if (callerMethod.getIsStatic()) {
            WriteMsg::write(WriteMsg::mlDebug, "Protected constructor (%s) called from static method (%s)\n", _node.getName(), callerMethod.getName().c_str());

            // Check if the two methods are in the same class
            const asg::logical::Class& callerParentClass = dynamic_cast<const asg::logical::Class&>(getParentScope(callerMethod));
            const asg::logical::Class& nodeParentClass = dynamic_cast<const asg::logical::Class&>(getParentScope(_node));

            if (callerParentClass.getId() == nodeParentClass.getId()) {
              WriteMsg::write(WriteMsg::mlDebug, "Singleton found: %s\n", callerParentClass.getName().c_str());
              addWarning(callerParentClass, "Singleton", "This class is probably a singleton.");
            }
          }
        }
      }
    }

    WriteMsg::write( WriteMsg::mlDebug, "Visit   [END]   (Method): %s\n", _node.getName().c_str());
  }
}}}
