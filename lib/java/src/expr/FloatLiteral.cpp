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

#include "java/inc/java.h"
#include "java/inc/Common.h"
#include "common/inc/WriteMessage.h"

#include "java/inc/messages.h"
#include <algorithm>
#include <string.h>
#include "common/inc/math/common.h"
#include <sstream>


namespace columbus { namespace java { namespace asg {

typedef boost::crc_32_type  Crc_type;

namespace expr { 
  FloatLiteral::FloatLiteral(NodeId _id, Factory *_factory) :
         Base(_id, _factory),
         Commentable(_id, _factory),
    NumberLiteral(_id, _factory),
    m_floatValue(0.0)
  {
  }

  FloatLiteral::~FloatLiteral() {
  }

  void FloatLiteral::clone(const FloatLiteral& other, bool tryOnVirtualParent) {
    if (tryOnVirtualParent) {
      base::Base::clone(other, false);
    }
    if (tryOnVirtualParent) {
      base::Commentable::clone(other, false);
    }
    expr::NumberLiteral::clone(other, false);

    m_floatValue = other.m_floatValue;
  }

  void FloatLiteral::prepareDelete(bool tryOnVirtualParent){
    if (tryOnVirtualParent) {
      base::Base::prepareDelete(false);
    }
    if (tryOnVirtualParent) {
      base::Commentable::prepareDelete(false);
    }
    expr::NumberLiteral::prepareDelete(false);
  }

  NodeKind FloatLiteral::getNodeKind() const {
    return ndkFloatLiteral;
  }

  float FloatLiteral::getFloatValue() const {
    return m_floatValue;
  }

  void FloatLiteral::setFloatValue(float _floatValue) {
    m_floatValue = _floatValue;
  }

  bool FloatLiteral::setEdge(EdgeKind edgeKind, NodeId edgeEnd, bool tryOnVirtualParent) {
    if (tryOnVirtualParent) {
      if (base::Base::setEdge(edgeKind, edgeEnd, false)) {
        return true;
      }
    }
    if (tryOnVirtualParent) {
      if (base::Commentable::setEdge(edgeKind, edgeEnd, false)) {
        return true;
      }
    }
    if (expr::NumberLiteral::setEdge(edgeKind, edgeEnd, false)) {
      return true;
    }
    return false;
  }

  bool FloatLiteral::removeEdge(EdgeKind edgeKind, NodeId edgeEnd, bool tryOnVirtualParent) {
    if (tryOnVirtualParent) {
      if (base::Base::removeEdge(edgeKind, edgeEnd, false)) {
        return true;
      }
    }
    if (tryOnVirtualParent) {
      if (base::Commentable::removeEdge(edgeKind, edgeEnd, false)) {
        return true;
      }
    }
    if (expr::NumberLiteral::removeEdge(edgeKind, edgeEnd, false)) {
      return true;
    }
    return false;
  }

  void FloatLiteral::accept(Visitor &visitor) const {
    visitor.visit(*this);
  }

  void FloatLiteral::acceptEnd(Visitor &visitor) const {
    visitor.visitEnd(*this);
  }

  double FloatLiteral::getSimilarity(const base::Base& base){
    if(base.getNodeKind() == getNodeKind()) {
      const FloatLiteral& node = dynamic_cast<const FloatLiteral&>(base);
      double matchAttrs = 0;
      if(node.getIsCompilerGenerated() == getIsCompilerGenerated()) ++matchAttrs;
      if(node.getIsToolGenerated() == getIsToolGenerated()) ++matchAttrs;
      std::string str1, str2;
      size_t strMax;
      double strSim;
      str1 = getValue();
      str2 = node.getValue();
      strMax = std::max(str1.size(), str2.size());
      strSim = 1 - ((double)common::math::editDistance(str1, str2) / (strMax > 0 ? strMax : 1));
      if (strSim < Common::SimilarityMinForStrings)
        return 0.0;
      matchAttrs += strSim;
      if(node.getFloatValue() == getFloatValue()) ++matchAttrs;
      return matchAttrs / (4 / (1 - Common::SimilarityMinimum)) + Common::SimilarityMinimum;
    } else {
      return 0.0;
    }
  }

  void FloatLiteral::swapStringTable(RefDistributorStrTable& newStrTable, std::map<Key,Key>& oldAndNewStrKeyMap ){
    std::map<Key,Key>::iterator foundKeyId;

    foundKeyId = oldAndNewStrKeyMap.find(m_value);
    if (foundKeyId != oldAndNewStrKeyMap.end()) {
      m_value = foundKeyId->second;
    } else {
      Key oldkey = m_value;
      m_value = newStrTable.set(factory->getStringTable().get(m_value));
      oldAndNewStrKeyMap.insert(std::pair<Key,Key>(oldkey,m_value));    }

  }

  NodeHashType FloatLiteral::getHash(std::set<NodeId>& travNodes) const {
    if (hashOk) return nodeHashCache;
    common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_BEGIN,this->getId()); 
    if (travNodes.count(getId())>0) {
      common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_SKIP);
      return 0;
    }
    travNodes.insert(getId());
    Crc_type resultHash;
    resultHash.process_bytes( "expr::FloatLiteral", strlen("expr::FloatLiteral"));
    common::WriteMsg::write(CMSG_GET_THE_NODE_HASH_OF_NODE_END,resultHash.checksum()); 
    nodeHashCache = resultHash.checksum();
    hashOk = true;
    return nodeHashCache;
  }

  void FloatLiteral::sort(bool withVirtualBase /* = true */) {
    if (withVirtualBase)
      Base::sort(false);
    if (withVirtualBase)
      Commentable::sort(false);
    NumberLiteral::sort(false);
  }

  void FloatLiteral::save(io::BinaryIO &binIo,bool withVirtualBase  /*= true*/) const {
    if (withVirtualBase)
      Base::save(binIo,false);

    if (withVirtualBase)
      Commentable::save(binIo,false);

    NumberLiteral::save(binIo,false);

    binIo.writeFloat4(m_floatValue);

  }

  void FloatLiteral::load(io::BinaryIO &binIo, bool withVirtualBase /*= true*/) {
    if (withVirtualBase)
      Base::load(binIo, false);

    if (withVirtualBase)
      Commentable::load(binIo, false);

    NumberLiteral::load(binIo,false);

    m_floatValue = binIo.readFloat4();

  }


}


}}}
