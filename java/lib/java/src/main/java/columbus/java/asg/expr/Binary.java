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


/**
 * Interface Binary, which represents the {@link columbus.java.asg.expr.Binary Binary} node.
 * @columbus.node (missing)
 * @columbus.edge hasLeftOperand ({@link columbus.java.asg.expr.Expression Expression}, single) : (missing)
 * @columbus.edge hasRightOperand ({@link columbus.java.asg.expr.Expression Expression}, single) : (missing)
 */
public interface Binary extends Expression {

	/**
	 * Gives back the reference of the node the {@link columbus.java.asg.expr.Binary#edgeHasLeftOperand hasLeftOperand} edge points to.
	 * @return Returns the end point of the hasLeftOperand edge.
	 */
	public Expression getLeftOperand();

	/**
	 * Sets the {@link columbus.java.asg.expr.Binary#edgeHasLeftOperand hasLeftOperand} edge.
	 * @param id The new end point of the hasLeftOperand edge.
	 */
	public void setLeftOperand(int id);

	/**
	 * Sets the {@link columbus.java.asg.expr.Binary#edgeHasLeftOperand hasLeftOperand} edge.
	 * @param node The new end point of the hasLeftOperand edge.
	 */
	public void setLeftOperand(Expression node);

	/**
	 * Gives back the reference of the node the {@link columbus.java.asg.expr.Binary#edgeHasRightOperand hasRightOperand} edge points to.
	 * @return Returns the end point of the hasRightOperand edge.
	 */
	public Expression getRightOperand();

	/**
	 * Sets the {@link columbus.java.asg.expr.Binary#edgeHasRightOperand hasRightOperand} edge.
	 * @param id The new end point of the hasRightOperand edge.
	 */
	public void setRightOperand(int id);

	/**
	 * Sets the {@link columbus.java.asg.expr.Binary#edgeHasRightOperand hasRightOperand} edge.
	 * @param node The new end point of the hasRightOperand edge.
	 */
	public void setRightOperand(Expression node);

}

