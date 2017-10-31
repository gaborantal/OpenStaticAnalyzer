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

#ifndef _LIM_ControlFlowBlock_H_
#define _LIM_ControlFlowBlock_H_

#include "lim/inc/lim.h"

/**
* \file ControlFlowBlock.h
* \brief Contains declaration of the base::ControlFlowBlock class.
* \brief The it get atributes from 
*/

namespace columbus { namespace lim { namespace asg {
namespace base {

  /**
  * \brief ControlFlowBlock class, which represents the base::ControlFlowBlock node.
  * (missing)
  * 
  * Edges:
  *   - calls (logical::Method, multiple) : (missing)
  *   - pred (base::ControlFlowBlock, multiple) : (missing)
  */
  class ControlFlowBlock : public Base {
    protected:
      /**
      * \internal
      * \brief Non-public constructor, only factory can instantiates nodes.
      * \param nodeId  [in] The id of the node.
      * \param factory [in] Poiter to the Factory the node belongs to.
      */
      ControlFlowBlock(NodeId nodeId, Factory *factory);

      /**
      * \internal
      * \brief Non-public destructor, only factory can destroy nodes.
      */
      virtual ~ControlFlowBlock();

    private:
      /**
      * \brief This function always throws a LimException due to copying is not allowed!
      */
      ControlFlowBlock & operator=(const ControlFlowBlock&);

      /**
      * \brief This function always throws a LimException due to copying is not allowed!
      */
      ControlFlowBlock(const ControlFlowBlock&);

    protected:
      /**
      * \internal
      * \brief Clone method.
      */
      virtual void clone(const ControlFlowBlock& other, bool tryOnVirtualParent);

    public:
      /**
      * \brief Gives back the NodeKind of the node.
      * \return Returns with the NodeKind.
      */
      virtual NodeKind getNodeKind() const;

      /**
      * \brief Delete all edge.
      */
      virtual void prepareDelete(bool tryOnVirtualParent);

    protected:
      /**
      * \brief Set or add the edge by edge kind
      * \param edgeKind           [in] The kind of the edge.
      * \param edgeEnd            [in] The id of node which is on the end of the edge.
      * \param acValue            [in] The value of the association class.
      * \param tryOnVirtualParent [in] This is help for the traversal.
      * \return Return true if setting was success.
      */
      virtual bool setEdge(EdgeKind edgeKind, NodeId edgeEnd, void *acValue, bool tryOnVirtualParent);

    protected:
      /**
      * \brief Remove the edge by edge kind
      * \param edgeKind           [in] The kind of the edge.
      * \param edgeEnd            [in] The id of node which is on the end of the edge.
      * \param tryOnVirtualParent [in] This is help for the traversal.
      * \return Return true if removing was success.
      */
      virtual bool removeEdge(EdgeKind edgeKind, NodeId edgeEnd, bool tryOnVirtualParent);

    public:

      // ---------- Edge getter function(s) ----------

      /**
      * \brief Gives back iterator for the calls edges.
      * \return Returns an iterator for the calls edges.
      */
      ListIterator<logical::Method> getCallsListIteratorBegin() const;

      /**
      * \brief Gives back iterator for the calls edges.
      * \return Returns an iterator for the calls edges.
      */
      ListIterator<logical::Method> getCallsListIteratorEnd() const;

      /**
      * \brief Tells whether the node has calls edges or not.
      * \return Returns true if the node doesn't have any calls edge.
      */
      bool getCallsIsEmpty() const;

      /**
      * \brief Gives back how many calls edges the node has.
      * \return Returns with the number of calls edges.
      */
      unsigned getCallsSize() const;

      /**
      * \brief Gives back iterator for the pred edges.
      * \return Returns an iterator for the pred edges.
      */
      ListIterator<base::ControlFlowBlock> getPredListIteratorBegin() const;

      /**
      * \brief Gives back iterator for the pred edges.
      * \return Returns an iterator for the pred edges.
      */
      ListIterator<base::ControlFlowBlock> getPredListIteratorEnd() const;

      /**
      * \brief Tells whether the node has pred edges or not.
      * \return Returns true if the node doesn't have any pred edge.
      */
      bool getPredIsEmpty() const;

      /**
      * \brief Gives back how many pred edges the node has.
      * \return Returns with the number of pred edges.
      */
      unsigned getPredSize() const;


      // ---------- Edge setter function(s) ----------

      /**
      * \brief Adds a new calls edge to the node and inserts it after the other ones.
      * \param node [in] The end point of the new calls edge.
      */
      void addCalls(const logical::Method *node);

      /**
      * \brief Adds a new calls edge to the node and inserts it after the other ones.
      * \param id [in] The end point of the new calls edge.
      */
      void addCalls(NodeId id);

      /**
      * \brief Remove the calls edge by id from the node.
      * \param id [in] The end point of the calls edge.
      */
      void removeCalls(NodeId id);

      /**
      * \brief Remove the calls edge from the node.
      * \param node [in] The end point of the calls edge.
      */
      void removeCalls(logical::Method *node);

      /**
      * \brief Adds a new pred edge to the node and inserts it after the other ones.
      * \param node [in] The end point of the new pred edge.
      */
      void addPred(const ControlFlowBlock *node);

      /**
      * \brief Adds a new pred edge to the node and inserts it after the other ones.
      * \param id [in] The end point of the new pred edge.
      */
      void addPred(NodeId id);

      /**
      * \brief Remove the pred edge by id from the node.
      * \param id [in] The end point of the pred edge.
      */
      void removePred(NodeId id);

      /**
      * \brief Remove the pred edge from the node.
      * \param node [in] The end point of the pred edge.
      */
      void removePred(ControlFlowBlock *node);

    protected:

      // ---------- Edges ----------

      /** \internal \brief Container stores the id of the nodes the calls edge points to. */
      ListIterator<logical::Method>::Container callsContainer;

      /** \internal \brief Container stores the id of the nodes the pred edge points to. */
      ListIterator<base::ControlFlowBlock>::Container predContainer;

    public:

      // ---------- Accept fundtions for Visitor ----------

      /**
      * \brief It calls the appropriate visit method of the given visitor.
      * \param visitor [in] The used visitor.
      */
      virtual void accept(Visitor& visitor) const;

      /**
      * \brief It calls the appropriate visitEnd method of the given visitor.
      * \param visitor [in] The used visitor.
      */
      virtual void acceptEnd(Visitor& visitor) const;

      /**
      * \internal
      * \brief Calculate node similarity.
      * \param nodeIf [in] The other node.
      */
      virtual double getSimilarity(const base::Base& node);

      /**
      * \internal
      * \brief Calculate node hash.
      */
      virtual NodeHashType getHash(std::set<NodeId>&  node) const ;

    protected:
      /**
      * \internal
      * \brief It is swap the string table ids to the other string table.
      * \param newStrTable [in] The new table
      * \param oldAndNewStrKeyMap [in] The map for fast serch.
      */
      virtual void swapStringTable(RefDistributorStrTable& newStrTable, std::map<Key,Key>& oldAndNewStrKeyMap );

      /**
      * \internal
      * \brief Saves the node.
      * \param io [in] The node is written into io.
      */
      virtual void save(io::BinaryIO &io, bool withVirtualBase = true) const;

      /**
      * \internal
      * \brief Loads the node.
      * \param io [in] The node is read from io.
      */
      virtual void load(io::BinaryIO &io, bool withVirtualBase = true);


      friend class lim::asg::Factory;
      friend class lim::asg::VisitorSave;
  };

} 


}}}
#endif

