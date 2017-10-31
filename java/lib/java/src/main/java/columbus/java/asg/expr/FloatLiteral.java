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
 * Interface FloatLiteral, which represents the {@link columbus.java.asg.expr.FloatLiteral FloatLiteral} node.
 * @columbus.node (missing)
 * @columbus.attr floatValue (float) : (missing)
 */
public interface FloatLiteral extends NumberLiteral {

	/**
	 * Gives back the {@link columbus.java.asg.expr.FloatLiteral#attributeFloatValue floatValue} of the node.
	 * @return Returns with the floatValue.
	 */
	public float getFloatValue();

	/**
	 * Sets the {@link columbus.java.asg.expr.FloatLiteral#attributeFloatValue floatValue} of the node.
	 * @param value The new value of the floatValue.
	 */
	public void setFloatValue(float value);

}

