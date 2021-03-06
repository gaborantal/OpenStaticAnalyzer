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

package columbus.java.asg.expr;

import columbus.java.asg.type.Type;
import columbus.java.asg.base.Positioned;

/**
 * Interface Expression, which represents the {@link columbus.java.asg.expr.Expression Expression} node.
 * @columbus.node (missing)
 * @columbus.edge type ({@link columbus.java.asg.type.Type Type}, single) : (missing)
 */
public interface Expression extends Positioned {

	/**
	 * Gives back the reference of the node the {@link columbus.java.asg.expr.Expression#edgeType type} edge points to.
	 * @return Returns the end point of the type edge.
	 */
	public Type getType();

	/**
	 * Sets the {@link columbus.java.asg.expr.Expression#edgeType type} edge.
	 * @param id The new end point of the type edge.
	 */
	public void setType(int id);

	/**
	 * Sets the {@link columbus.java.asg.expr.Expression#edgeType type} edge.
	 * @param node The new end point of the type edge.
	 */
	public void setType(Type node);

}

