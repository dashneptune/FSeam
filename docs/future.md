# Future to be implemented

## Other
* ~~Add Travis support~~ : Thanks to [Jalal Chabane](https://github.com/chaabaj)
* Add codecov support

## Improve Documentation
* Detailed documentation on dupe (into usage.md file)
* ~~Improve documentation on CMake usage (giving the different possibilities with either using a target or a list of file)~~
* Add detailed documentation on how to use the python script to generate the FSeam mock (in order to use FSeam without CMake)  
* Add an example on how to use FSeam with another Testing framework, using the generated target to register test in CTest
* ~~Add documentation on the fact that destructor have their name prefixed by Destructor_~~
* ~~Detailed documentation on dupeReturn (into usage.md file)~~
* ~~Detailed documentation on verify (into usage.md file)~~
* ~~Improve documentation, add get mock handler (into testing.md file)~~
* ~~Detailed documentation on logging feature (into logging.md file)~~
* ~~Detailed documentation on expectArg (into usage.md file)~~
* ~~Detailed documentation on free function (into free-function.md)~~

## V1.1.0:     
* **Functional Improvements**  
  * **Use function overload instead of specialization** for verify arg
  * **Adding inner-class-struct/class**: The code generation is wrong and not compilable when mocking a header file with a class embedded in another on
  * **Add argument expectation non copyable support** using constexpr if and store uniqueptr in case of non copyable argument
  * ~~**Implement a cleanup for the FreeFunctionDataStructure** as it currently is concatenated infinitely, it would be needed to ensure a cleanup at each build.~~
  * ~~**Remove Catch dependency from the FSeam module**:~~  
    * ~~Replace it by an option (to keep it only if using Catch2 with a find_package not required)~~
  * ~~**Rename MockVerifier.hpp into FSeam.hpp**~~
  * ~~**Add free/static function support**~~
  * ~~**Add logging functionality** : Would be useful in order to have detailed explanation on failures (instead of just a return true/false on the verify)~~
    * ~~Customization of the logging via registering method to do the logging~~
    * ~~Pre-defined logging with Catch2 and GTest~~
  * ~~Improve the Cmake utility; able to choose between a target or a list of file in order to compile FSeam tests.~~
  
* **Bug Fix** (if any :p)
  * ~~Installation doesn't create the FSeam folder (used by the generated includes)~~
  * ~~Error when mocking a class with a constructor not implemented in the header.~~
  * ~~Doesn't properly generate constant method mock~~
  * ~~Multiple free function/ static method mocking in a row are overwritting the FreeFunction data structure~~
  * ~~Static method mock of static method of class without namespace weren't correctly generated~~
  * ?

## V1.2.0

* **Design Improvements**
  * **Implement Babel support** https://github.com/FreeYourSoul/FSeam/issues/7
  * **Change position of template specialization** Do the specialization in the fseam.cc file
* **Functional Improvements**
  * **Implement Module mocking support**
  * **Provide a multi-threaded safety option**: Currently using singleton and static memory without having any synchronization mechanism. Any multithreading test would make the verify not being accurate.
