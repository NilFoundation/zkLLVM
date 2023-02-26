[[circuit]] __zkllvm_field_pallas_base field_add(
	__zkllvm_field_pallas_base a_pallas_base,
	__zkllvm_field_pallas_base b_pallas_base,
	__zkllvm_field_pallas_scalar a_pallas_scalar,
	__zkllvm_curve_pallas a_pallas_curve, 
	__zkllvm_curve_pallas b_pallas_curve, 
	__zkllvm_field_curve25519_base a_25519_field, 
	__zkllvm_field_curve25519_base b_25519_field
	) {
	
	for (int i = 0; i < 10; i++) {
		a_pallas_base = a_pallas_base + b_pallas_base;
		a_pallas_base = a_pallas_base - b_pallas_base;
		a_pallas_base = a_pallas_base * b_pallas_base;

		a_25519_field = a_25519_field + b_25519_field;
		// a_25519_field = a_25519_field - b_25519_field;
		// a_25519_field = a_25519_field * b_25519_field;

		// a_pallas_curve = a_pallas_curve * a_pallas_scalar;
		// a_pallas_curve = a_pallas_curve + b_pallas_curve;
	}

    return a_pallas_base + b_pallas_base;
}