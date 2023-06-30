# Input format for circuits

Circuit entry function excpects input in a JSON format. The input is a list of key-value pairs, where the key is the **type** of the variable and the value is the value of the variable.

{% hint style="info" %}
Notice, that the keys in  the input JSON must be the same as the types of the `[[circuit]]` function arguments.
{% endhint %}

Currently supported types are:
* string (for C-like strings char*)
* int (for integer values)
* field (for field elements)
* curve (for curve elements)
* array (for std::array of integer, field or curve elements)
* vector (for std::vector of integer, field or curve elements)

You can find examples of input files in the [corresponding folder](https://github.com/NilFoundation/zkllvm/tree/master/examples) of zkLLVM repository.