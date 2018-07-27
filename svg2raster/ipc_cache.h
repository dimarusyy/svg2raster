#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>

template <typename T>
using ipc_shared_alloc_t = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

using ipc_string_t = boost::interprocess::basic_string<char, std::char_traits<char>, ipc_shared_alloc_t<char>> ;

using ipc_string_allocator_t = ipc_shared_alloc_t<ipc_string_t>;

template <typename T>
using ipc_pair_allocator_t = ipc_shared_alloc_t<std::pair<ipc_string_t const, T>>;

template <typename T, typename V>
using ipc_shared_map_t = boost::interprocess::map<T, V, std::less<T>, ipc_shared_alloc_t<std::pair<T const, V>>>;

template <typename T>
using ipc_map_t = ipc_shared_map_t<ipc_string_t, T>;



