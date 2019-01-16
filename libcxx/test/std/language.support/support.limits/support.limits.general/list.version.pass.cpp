//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// WARNING: This test was generated by generate_feature_test_macros_tests.py and
// should not be edited manually.

// <list>

// Test the feature test macros defined by <list>

/*  Constant                                      Value
    __cpp_lib_allocator_traits_is_always_equal    201411L [C++17]
    __cpp_lib_erase_if                            201811L [C++2a]
    __cpp_lib_incomplete_container_elements       201505L [C++17]
    __cpp_lib_list_remove_return_type             201806L [C++2a]
    __cpp_lib_nonmember_container_access          201411L [C++17]
*/

#include <list>
#include "test_macros.h"

#if TEST_STD_VER < 14

# ifdef __cpp_lib_allocator_traits_is_always_equal
#   error "__cpp_lib_allocator_traits_is_always_equal should not be defined before c++17"
# endif

# ifdef __cpp_lib_erase_if
#   error "__cpp_lib_erase_if should not be defined before c++2a"
# endif

# ifdef __cpp_lib_incomplete_container_elements
#   error "__cpp_lib_incomplete_container_elements should not be defined before c++17"
# endif

# ifdef __cpp_lib_list_remove_return_type
#   error "__cpp_lib_list_remove_return_type should not be defined before c++2a"
# endif

# ifdef __cpp_lib_nonmember_container_access
#   error "__cpp_lib_nonmember_container_access should not be defined before c++17"
# endif

#elif TEST_STD_VER == 14

# ifdef __cpp_lib_allocator_traits_is_always_equal
#   error "__cpp_lib_allocator_traits_is_always_equal should not be defined before c++17"
# endif

# ifdef __cpp_lib_erase_if
#   error "__cpp_lib_erase_if should not be defined before c++2a"
# endif

# ifdef __cpp_lib_incomplete_container_elements
#   error "__cpp_lib_incomplete_container_elements should not be defined before c++17"
# endif

# ifdef __cpp_lib_list_remove_return_type
#   error "__cpp_lib_list_remove_return_type should not be defined before c++2a"
# endif

# ifdef __cpp_lib_nonmember_container_access
#   error "__cpp_lib_nonmember_container_access should not be defined before c++17"
# endif

#elif TEST_STD_VER == 17

# ifndef __cpp_lib_allocator_traits_is_always_equal
#   error "__cpp_lib_allocator_traits_is_always_equal should be defined in c++17"
# endif
# if __cpp_lib_allocator_traits_is_always_equal != 201411L
#   error "__cpp_lib_allocator_traits_is_always_equal should have the value 201411L in c++17"
# endif

# ifdef __cpp_lib_erase_if
#   error "__cpp_lib_erase_if should not be defined before c++2a"
# endif

# ifndef __cpp_lib_incomplete_container_elements
#   error "__cpp_lib_incomplete_container_elements should be defined in c++17"
# endif
# if __cpp_lib_incomplete_container_elements != 201505L
#   error "__cpp_lib_incomplete_container_elements should have the value 201505L in c++17"
# endif

# ifdef __cpp_lib_list_remove_return_type
#   error "__cpp_lib_list_remove_return_type should not be defined before c++2a"
# endif

# ifndef __cpp_lib_nonmember_container_access
#   error "__cpp_lib_nonmember_container_access should be defined in c++17"
# endif
# if __cpp_lib_nonmember_container_access != 201411L
#   error "__cpp_lib_nonmember_container_access should have the value 201411L in c++17"
# endif

#elif TEST_STD_VER > 17

# ifndef __cpp_lib_allocator_traits_is_always_equal
#   error "__cpp_lib_allocator_traits_is_always_equal should be defined in c++2a"
# endif
# if __cpp_lib_allocator_traits_is_always_equal != 201411L
#   error "__cpp_lib_allocator_traits_is_always_equal should have the value 201411L in c++2a"
# endif

# ifndef __cpp_lib_erase_if
#   error "__cpp_lib_erase_if should be defined in c++2a"
# endif
# if __cpp_lib_erase_if != 201811L
#   error "__cpp_lib_erase_if should have the value 201811L in c++2a"
# endif

# ifndef __cpp_lib_incomplete_container_elements
#   error "__cpp_lib_incomplete_container_elements should be defined in c++2a"
# endif
# if __cpp_lib_incomplete_container_elements != 201505L
#   error "__cpp_lib_incomplete_container_elements should have the value 201505L in c++2a"
# endif

# if !defined(_LIBCPP_VERSION)
#   ifndef __cpp_lib_list_remove_return_type
#     error "__cpp_lib_list_remove_return_type should be defined in c++2a"
#   endif
#   if __cpp_lib_list_remove_return_type != 201806L
#     error "__cpp_lib_list_remove_return_type should have the value 201806L in c++2a"
#   endif
# else // _LIBCPP_VERSION
#   ifdef __cpp_lib_list_remove_return_type
#     error "__cpp_lib_list_remove_return_type should not be defined because it is unimplemented in libc++!"
#   endif
# endif

# ifndef __cpp_lib_nonmember_container_access
#   error "__cpp_lib_nonmember_container_access should be defined in c++2a"
# endif
# if __cpp_lib_nonmember_container_access != 201411L
#   error "__cpp_lib_nonmember_container_access should have the value 201411L in c++2a"
# endif

#endif // TEST_STD_VER > 17

int main() {}
