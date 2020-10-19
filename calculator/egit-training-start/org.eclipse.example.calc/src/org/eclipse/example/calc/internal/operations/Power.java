package org.eclipse.example.calc.internal.operations;

import org.eclipse.example.calc.BinaryOperation;

/**
 * Binary power operation
 */
public class Power extends AbstractOperation implements BinaryOperation {

	@Override
	public float perform(float arg1, float arg2) {
		float arg = 1;
		for(int i=0; i < arg2; i++)
			arg = arg * arg1;
		return arg;
	}

	@Override
	public String getName() {
		return "^";
	}

}
