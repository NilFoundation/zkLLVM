---
description: Current limitations of the compiler
---

# Limitations

The zkLLVM project is under active development. The current limitations are listed below and will be addressed/updated in future versions.

* There can only be a single `[[circuit]]` directive in a project. A circuit can use other components.
* Following C++ standard types are not supported.
  * `string`
  * `float` , `double`
* Circuits cannot return pointer types for now.
