cmake_minimum_required(VERSION 3.30)
project(mySTL)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(mySTL
#        main.cpp
        mytype_traits/my_enable_if.h
        mytype_traits/my_add_const.h
        mytype_traits/my_add_cv.h
        mytype_traits/my_add_lvalue_reference.h
        mytype_traits/my_type_identity.h
        mytype_traits/my_remove_reference.h
        mytype_traits/my_add_pointer.h
        mytype_traits/my_add_volatile.h
        mytype_traits/my_add_rvalue_reference.h
        mytype_traits/my_is_const.h
        mytype_traits/my_integral_constant.h
        mytype_traits/my_apply_cv.h
        mytype_traits/my_is_volatile.h
        mytype_traits/my_conditional.h
        mytype_traits/my_decay.h
        mytype_traits/my_is_array.h
        mytype_traits/my_remove_const.h
        mytype_traits/my_remove_volatile.h
        mytype_traits/my_remove_cv.h
        mytype_traits/my_is_reference.h
        mytype_traits/my_is_function.h
        mytype_traits/my_remove_extent.h
        mytype_traits/my_has_virtual_destructor.h
        mytype_traits/my_is_abstract.h
        mytype_traits/my_is_arithmetic.h
        mytype_traits/my_is_floating_point.h
        mytype_traits/my_is_integral.h
        mytype_traits/my_is_bounded_array.h
        mytype_traits/my_is_class.h
        mytype_traits/my_is_union.h
        mytype_traits/my_is_convertible.h
        mytype_traits/my_is_void.h
        mytype_traits/my_is_same.h
        utility_/my_declval.h
        mytype_traits/my_is_null_pointer.h
        mytype_traits/my_is_pointer.h
        mytype_traits/my_is_signed.h
        mytype_traits/my_is_unbounded_array.h
        mytype_traits/my_is_unsigned.h
        mytype_traits/my_make_const_lvalue_ref.h
        mytype_traits/my_make_signed.h
        mytype_traits/my_make_unsigned.h
        mytype_traits/my_remove_const_ref.h
        mytype_traits/my_remove_pointer.h
        mytype_traits/my_void_t.h
        utility_/my_forward.h
        utility_/my_move.h
        utility_/my_swap.h
        my_algorithm.h
        my_exception.h
        memory_/my_allocator.h
        my_utility.h
        my_vector.h
        my_type_traits.h
        my_uninitialized.h
        iterator_/my_back_insert_iterator.h
        iterator_/my_iterator_traits.h
        iterator_/my_front_insert_iterator.h
        iterator_/my_insert_iterator.h
        iterator_/my_istream_iterator.h
        iterator_/my_ostream_iterator.h
        iterator_/my_iterator_advance.h
        iterator_/my_iterator_distance.h
        iterator_/my_reverse_iterator.h
        my_iterator.h
#        test/my_vector_test.cpp
        my_string.h
#        test/my_string_test.cpp
        my_deque.h
#        test/my_deque_test.cpp
        my_queue.h
#        test/my_queue_test.cpp
        my_stack.h
#        test/my_stack_test.cpp
        my_array.h
#        test/my_array_test.cpp
        my_pair.h
#        test/mt_pair_test.cpp
        my_tuple.h
#        test/my_tuple_test.cpp
        my_optional.h
#        test/my_optional_test.cpp
        my_list.h
        my_initializer_list.h
#        test/my_list_test.cpp
        my_hashtable.h
#        test/my_hashtable_test.cpp
        my_unordered_map.h
#        test/my_unordered_map_test.cpp
        my_unordered_multimap.h
#        test/my_unordered_multimap_test.cpp
        my_unordered_set.h
#        test/my_unordered_set_test.cpp
        my_unordered_multiset.h
#        test/my_unordered_multiset_test.cpp
        my_heap_sort.h
#        test/my_heap_sort_test.cpp
        my_priority_queue.h
#        test/my_priority_queue_test.cpp
        my_rbtree.h
#        test/my_rbtree_test.cpp
        my_set.h
#        test/my_set_test.cpp
        my_multiset.h
#        test/my_multiset_test.cpp
        my_rbtree_map.h
        my_map.h
#        test/my_rbtree_map_test.cpp
#        test/my_map_test.cpp
        my_unique_ptr.h
#        test/my_unique_ptr_test.cpp
        my_shared_ptr.h
#        test/my_shared_ptr_test.cpp
        chrono_/my_ratio.h
#        test/my_ratio_test.cpp
        chrono_/my_duration.h
        test/my_duration_test.cpp
)
