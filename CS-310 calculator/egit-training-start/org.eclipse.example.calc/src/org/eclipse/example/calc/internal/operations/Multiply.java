package org.eclipse.example.calc.internal.operations;

import org.eclipse.example.calc.BinaryOperation;

/**
 * Binary Multiply operation
 */
public class Multiply extends AbstractOperation implements BinaryOperation {

	@Override
	public float perform(float arg1, float arg2) {
		return arg1 * arg2;
	}

	@Override
	public String getName() {
		return "*";
	}

}
