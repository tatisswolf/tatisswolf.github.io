package org.eclipse.example.calc.internal.operations;

import org.eclipse.example.calc.BinaryOperation;

/**
 * Percent plus operation
 */
public class Percent extends AbstractOperation implements BinaryOperation {

	@Override
	public float perform(float arg1, float arg2) {
		return arg1 * (arg2 / 100);
	}

	@Override
	public String getName() {
		return "%";
	}

}
