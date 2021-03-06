/*
 *  This file is part of OpenStaticAnalyzer.
 *
 *  Copyright (c) 2004-2017 Department of Software Engineering - University of Szeged
 *
 *  Licensed under Version 1.2 of the EUPL (the "Licence");
 *
 *  You may not use this work except in compliance with the Licence.
 *
 *  You may obtain a copy of the Licence in the LICENSE file or at:
 *
 *  https://joinup.ec.europa.eu/software/page/eupl
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the Licence is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the Licence for the specific language governing permissions and
 *  limitations under the Licence.
 */

#include "lim/inc/lim.h"
#include "lim/inc/Common.h"
#include "common/inc/WriteMessage.h"

#include "lim/inc/messages.h"
#include <algorithm>
#include <string.h>
#include "common/inc/math/common.h"


namespace columbus { namespace lim { namespace asg {

typedef boost::crc_32_type  Crc_type;

namespace logical { 
  MethodGenericSpec::MethodGenericSpec(NodeId _id, Factory *_factory) :
    MethodGeneric(_id, _factory),
    hasArgumentsContainer(),
    m_specialize(0)
  {
  }

  MethodGenericSpec::~MethodGenericSpec() {
  }

  void MethodGenericSpec::clone(const MethodGenericSpec& other, bool tryOnVirtualParent) {
    logical::MethodGeneric::clone(other, false);

    //isAssocEdge
    hasArgumentsContainer = other.hasArgumentsContainer;
    m_specialize = other.m_specialize;
  }

  void MethodGenericSpec::prepareDelete(bool tryOnVirtualParent){
    while (!hasArgumentsContainer.empty()) {
      const NodeId id = *hasArgumentsContainer.begin();
      if (factory->getExistsReverseEdges())
        factory->reverseEdges->removeEdge(id, this->getId(), edkMethodGenericSpec_HasArguments);
      hasArgumentsContainer.pop_front();
    }
    removeSpecialize();
    logical::MethodGeneric::prepareDelete(false);
  }

  NodeKind MethodGenericSpec::getNodeKind() const {
    return ndkMethodGenericSpec;
  }

  ListIteratorAssocTypeArgumentConstraintKind<type::Type> MethodGenericSpec::getArgumentsListIteratorAssocBegin() const {
    return ListIteratorAssocTypeArgumentConstraintKind<type::Type>(&hasArgumentsContainer, factory, true);
  }

  ListIteratorAssocTypeArgumentConstraintKind<type::Type> MethodGenericSpec::getArgumentsListIteratorAssocEnd() const {
    return ListIteratorAssocTypeArgumentConstraintKind<type::Type>(&hasArgumentsContainer, factory, false);
  }

  bool MethodGenericSpec::getArgumentsIsEmpty() const {
    return getArgumentsListIteratorAssocBegin() == getArgumentsListIteratorAssocEnd();
  }

  unsigned int MethodGenericSpec::getArgumentsSize() const {
    unsigned int size = 0;
    ListIteratorAssocTypeArgumentConstraintKind<type::Type> endIt = getArgumentsListIteratorAssocEnd();
    for (ListIteratorAssocTypeArgumentConstraintKind<type::Type> it = getArgumentsListIteratorAssocBegin(); it != endIt; ++it) {
      ++size;
    }
    return size;
  }

  logical::MethodGeneric* MethodGenericSpec::getSpecialize() const {
    logical::MethodGeneric *_node = NULL;
    if (m_specialize != 0)
      _node = dynamic_cast<logical::MethodGeneric*>(factory->getPointer(m_specialize));
    if ( (_node == NULL) || factory->getIsFiltered(_node))
      return NULL;

    return _node;
  }

  bool MethodGenericSpec::setEdge(EdgeKind edgeKind, NodeId edgeEnd, void* acValue, bool tryOnVirtualParent) {
    switch (edgeKind) {
      case edkMethodGenericSpec_HasArguments:
        addArguments(edgeEnd, *((TypeArgumentConstraintKind*)acValue));
        return true;
      case edkMethodGenericSpec_Specialize:
        setSpecialize(edgeEnd);
        return true;
      default:
        break;
    }
    if (logical::MethodGeneric::setEdge(edgeKind, edgeEnd, acValue, false)) {
      return true;
    }
    return false;
  }

  bool MethodGenericSpec::removeEdge(EdgeKind edgeKind, NodeId edgeEnd, bool tryOnVirtualParent) {
    switch (edgeKind) {
      case edkMethodGenericSpec_HasArguments:
        removeArguments(edgeEnd);
        return true;
      case edkMethodGenericSpec_Specialize:
        removeSpecialize();
        return true;
      default:
        break;
    }
    if (logical::MethodGeneric::removeEdge(edgeKind, edgeEnd, false)) {
      return true;
    }
    return false;
  }

  void MethodGenericSpec::addArguments(const type::Type *_node, TypeArgumentConstraintKind acValue) {
    if (_node == NULL)
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_NODE_IS_NULL);

    if (&(_node->getFactory()) != this->factory)
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_FACTORY_OF_NODES_DOES_NOT_MATCH);

    if (!((_node->getNodeKind() == ndkType) ))
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_INVALID_NODE_KIND);

    AssocEdge<NodeId,TypeArgumentConstraintKind> ac(_node->getId(), acValue);
    hasArgumentsContainer.push_back(ac);
    if (factory->reverseEdges)
      factory->reverseEdges->insertEdge(_node, this, edkMethodGenericSpec_HasArguments);
  }

  void MethodGenericSpec::addArguments(NodeId _id, TypeArgumentConstraintKind acValue) {
    const type::Type *node = dynamic_cast<type::Type*>(factory->getPointer(_id));
    if (node == NULL)
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_INVALID_NODE_KIND);
    addArguments( node, acValue );
  }

  void MethodGenericSpec::removeArguments(NodeId id) {
    if (!factory->getExist(id))
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_END_POINT_OF_THE_EDGE_DOES_NOT_EXIST);

    ListIteratorAssocTypeArgumentConstraintKind<type::Type>::Container::iterator it = find(hasArgumentsContainer.begin(), hasArgumentsContainer.end(), id);

    if (it == hasArgumentsContainer.end())
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_END_POINT_OF_THE_EDGE_DOES_NOT_EXIST);

    hasArgumentsContainer.erase(it);

    if (factory->getExistsReverseEdges())
      factory->reverseEdges->removeEdge(id, this->getId(), edkMethodGenericSpec_HasArguments);
  }

  void MethodGenericSpec::removeArguments(type::Type *_node) {
    if (_node == NULL)
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_EDGE_IS_NULL);

    removeArguments(_node->getId());
  }

  void MethodGenericSpec::setSpecialize(NodeId _id) {
    logical::MethodGeneric *_node = NULL;
    if (_id) {
      if (!factory->getExist(_id))
        throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_END_POINT_OF_THE_EDGE_DOES_NOT_EXIST);

      _node = dynamic_cast<logical::MethodGeneric*> (factory->getPointer(_id));
      if ( _node == NULL) {
        throw LimException(COLUMBUS_LOCATION, CMSG_EX_INVALID_NODE_KIND);
      }
      if (&(_node->getFactory()) != this->factory)
        throw LimException(COLUMBUS_LOCATION, CMSG_EX_THE_FACTORY_OF_NODES_DOES_NOT_MATCH );

      if (m_specialize) {
        if (factory->getExistsReverseEdges())
          factory->reverseEdges->removeEdge(m_specialize, m_id, edkMethodGenericSpec_Specialize);
      }
      m_specialize = _node->getId();
      if (factory->getExistsReverseEdges())
        factory->reverseEdges->insertEdge(m_specialize, this->getId(), edkMethodGenericSpec_Specialize);
    } else {
      if (m_specialize) {
        throw LimException(COLUMBUS_LOCATION, CMSG_EX_CAN_T_SET_EDGE_TO_NULL);
      }
    }
  }

  void MethodGenericSpec::setSpecialize(logical::MethodGeneric *_node) {
    if (_node == NULL)
      throw LimException(COLUMBUS_LOCATION, CMSG_EX_CAN_T_SET_EDGE_TO_NULL);

    setSpecialize(_node->getId());
  }

  void MethodGenericSpec::removeSpecialize() {
      if (m_specialize) {
        if (factory->getExistsReverseEdges())
          factory->reverseEdges->removeEdge(m_specialize, m_id, edkMethodGenericSpec_Specialize);
      }
      m_specialize = 0;
  }

  void MethodGenericSpec::accept(Visitor &visitor) const {
    visitor.visit(*this);
  }

  void MethodGenericSpec::acceptEnd(Visitor &visitor) const {
    visitor.visitEnd(*this);
  }

  double MethodGenericSpec::getSimilarity(const base::Base& base){
    if(base.getNodeKind() == getNodeKind()) {
      const MethodGenericSpec& node = dynamic_cast<const MethodGenericSpec&>(base);
      double matchAttrs = 0;
      std::string str1, str2;
      size_t strMax;
      double strSim;
      str1 = getName();
      str2 = node.getName();
      strMax = std::max(str1.size(), str2.size());
      strSim = 1 - ((double)common::math::editDistance(str1, str2) / (strMax > 0 ? strMax : 1));
      if (strSim < Common::SimilarityMinForStrings)
        return 0.0;
      matchAttrs += strSim;
      if(node.getAccessibility() == getAccessibility()) ++matchAttrs;
      if(node.getCommentLines() == getCommentLines()) ++matchAttrs;
      str1 = getDemangledName();
      str2 = node.getDemangledName();
      strMax = std::max(str1.size(), str2.size());
      strSim = 1 - ((double)common::math::editDistance(str1, str2) / (strMax > 0 ? strMax : 1));
      if (strSim < Common::SimilarityMinForStrings)
        return 0.0;
      matchAttrs += strSim;
      if(node.getIsCompilerGenerated() == getIsCompilerGenerated()) ++matchAttrs;
      if(node.getIsStatic() == getIsStatic()) ++matchAttrs;
      if(node.getLanguage() == getLanguage()) ++matchAttrs;
      str1 = getMangledName();
      str2 = node.getMangledName();
      strMax = std::max(str1.size(), str2.size());
      strSim = 1 - ((double)common::math::editDistance(str1, str2) / (strMax > 0 ? strMax : 1));
      if (strSim < Common::SimilarityMinForStrings)
        return 0.0;
      matchAttrs += strSim;
      if(node.getLLOC() == getLLOC()) ++matchAttrs;
      if(node.getLOC() == getLOC()) ++matchAttrs;
      if(node.getTLLOC() == getTLLOC()) ++matchAttrs;
      if(node.getTLOC() == getTLOC()) ++matchAttrs;
      if(node.getIsAnonymous() == getIsAnonymous()) ++matchAttrs;
      if(node.getDistinctOperands() == getDistinctOperands()) ++matchAttrs;
      if(node.getDistinctOperators() == getDistinctOperators()) ++matchAttrs;
      if(node.getIsAbstract() == getIsAbstract()) ++matchAttrs;
      if(node.getIsVirtual() == getIsVirtual()) ++matchAttrs;
      if(node.getMethodKind() == getMethodKind()) ++matchAttrs;
      if(node.getNestingLevel() == getNestingLevel()) ++matchAttrs;
      if(node.getNestingLevelElseIf() == getNestingLevelElseIf()) ++matchAttrs;
      if(node.getNumberOfBranches() == getNumberOfBranches()) ++matchAttrs;
      if(node.getNumberOfStatements() == getNumberOfStatements()) ++matchAttrs;
      if(node.getTotalOperands() == getTotalOperands()) ++matchAttrs;
      if(node.getTotalOperators() == getTotalOperators()) ++matchAttrs;
      return matchAttrs / (24 / (1 - Common::SimilarityMinimum)) + Common::SimilarityMinimum;
    } else {
      return 0.0;
    }
  }

  void MethodGenericSpec::swapStringTable(RefDistributorStrTable& newStrTable, std::map<Key,Key>& oldAndNewStrKeyMap ){
    std::map<Key,Key>::iterator foundKeyId;

    foundKeyId = oldAndNewStrKeyMap.find(m_demangledName);
    if (foundKeyId != oldAndNewStrKeyMap.end()) {
      m_demangledName = foundKeyId->second;
    } else {
      Key oldkey = m_demangledName;
      m_demangledName = newStrTable.set(factory->getStringTable().get(m_demangledName));
      oldAndNewStrKeyMap.insert(std::pair<Key,Key>(oldkey,m_demangledName));    }

    foundKeyId = oldAndNewStrKeyMap.find(m_mangledName);
    if (foundKeyId != oldAndNewStrKeyMap.end()) {
      m_mangledName = foundKeyId->second;
    } else {
      Key oldkey = m_mangledName;
      m_mangledName = newStrTable.set(factory->getStringTable().get(m_mangledName));
      oldAndNewStrKeyMap.insert(std::pair<Key,Key>(oldkey,m_mangledName));    }

    foundKeyId = oldAndNewStrKeyMap.find(m_name);
    if (foundKeyId != oldAndNewStrKeyMap.end()) {
      m_name = foundKeyId->second;
    } else {
      Key oldkey = m_name;
      m_name = newStrTable.set(factory->getStringTable().get(m_name));
      oldAndNewStrKeyMap.insert(std::pair<Key,Key>(oldkey,m_name));    }

  }

  NodeHashType MethodGenericSpec::getHash(std::set<NodeId>& travNodes) const {
    if (hashOk) return nodeHashCache;
    common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_BEGIN,this->getId()); 
    if (travNodes.count(getId())>0) {
      common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_SKIP);
      return 0;
    }
    travNodes.insert(getId());
    Crc_type resultHash;
    resultHash.process_bytes( "logical::MethodGenericSpec", strlen("logical::MethodGenericSpec"));
    common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_END,resultHash.checksum()); 
    nodeHashCache = resultHash.checksum();
    hashOk = true;
    return nodeHashCache;
  }

  void MethodGenericSpec::save(io::BinaryIO &binIo,bool withVirtualBase  /*= true*/) const {
    MethodGeneric::save(binIo,false);

    binIo.writeUInt4(m_specialize);


    for (ListIteratorAssocTypeArgumentConstraintKind<type::Type>::Container::const_iterator ait = hasArgumentsContainer.begin(); ait != hasArgumentsContainer.end(); ++ait) {
      binIo.writeUInt4(ait->node);
      binIo.writeUByte1(ait->assocClass);
    }
    binIo.writeUInt4(0); // This is the end sign
  }

  void MethodGenericSpec::load(io::BinaryIO &binIo, bool withVirtualBase /*= true*/) {
    MethodGeneric::load(binIo,false);

    m_specialize =  binIo.readUInt4();

    NodeId _id;

    _id = binIo.readUInt4();
    while (_id) {
      TypeArgumentConstraintKind _ac = static_cast<TypeArgumentConstraintKind>(binIo.readUByte1());
      hasArgumentsContainer.push_back(AssocEdge<NodeId,TypeArgumentConstraintKind>(_id, _ac));
      _id = binIo.readUInt4();
    }
  }


}


}}}
