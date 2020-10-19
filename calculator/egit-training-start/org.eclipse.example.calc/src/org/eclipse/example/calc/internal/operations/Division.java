package org.eclipse.example.calc.internal.operations;

import org.eclipse.example.calc.BinaryOperation;

/**
 * Binary division operation
 */
public class Division extends AbstractOperation implements BinaryOperation {

	@Override
	public float perform(float arg1, float arg2) {
		// Handles division by zero
		if(arg2 == 0) {
			return 000;
		}
		else {
			return arg1 / arg2;
		}
	}

	@Override
	public String getName() {
		return "/";
	}

}
