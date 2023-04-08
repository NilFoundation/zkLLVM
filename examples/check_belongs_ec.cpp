/* a * x^2 + y^2 = 1 + d * x^2 * y^2, a = -1, d = 0x52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3 */

[[circuit]] bool check_ec(__zkllvm_field_curve25519_base x, __zkllvm_field_curve25519_base y) {

    // __zkllvm_field_curve25519_base d_coeff = 0x52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3_cppui255;
    __zkllvm_field_curve25519_base d_coeff = {1,1,1,1};

    __zkllvm_field_curve25519_base left = y*y - x*x;
    __zkllvm_field_curve25519_base right = 1 + x*x*y*y*d_coeff;

    if (left == right) {return true;} 
    else               {return false;}
}