#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>

#include <unordered_map>

template <typename T>
using ipc_shared_allocator_t = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

template <typename K, typename V>
using ipc_shared_map_t = boost::interprocess::map<K, V, std::less<K>, ipc_shared_allocator_t<std::pair<K const, V>>>;

template <typename T>
using ipc_shared_list_t = boost::interprocess::list<T>;

//////////////////////////////////////////////////////////////////////////

using ipc_string_t = boost::interprocess::basic_string<char, std::char_traits<char>, ipc_shared_allocator_t<char>>;

using ipc_string_allocator_t = ipc_shared_allocator_t<ipc_string_t>;

//////////////////////////////////////////////////////////////////////////

template <int N, typename TList, typename TMap>
struct lru_strategy_t
{
	lru_strategy_t(TList& list, TMap& map) 
		: _list(list), _map(map)
	{
	}

	bool exists(typename TList::iterator hit_it) const
	{
		if (hit_it != _list.end())
		{
			// in cache
			_list.splice(_list.begin(), _list, hit_it);
			return true;
		}
		else
		{
			if (_list.size() == N) 		// full ?
			{
				// delete the last
				const auto last_it = _list.back();
				_list.pop_back();
				_map.erase(last_it);
			}
			return false;
		}
	}

	template <typename K, typename V>
	void insert(const K& key, const V& value) const
	{
		// insert to front
		_list.push_front(key);
		_map[key] = std::make_pair(value, _list.begin());
	}

private:
	TList& _list;
	TMap& _map;
};

template <typename K, typename V, size_t N = 10,
	typename TList = std::list<K>, 
	typename TMap = std::unordered_map<K, std::pair<V, typename TList::iterator>>,
	typename TStrategy = lru_strategy_t<N, TList, TMap>>
struct cache_t
{
	cache_t()
		: _strategy(_items_list, _items_map)
	{
	}

	bool get(const K& key, V& value)
	{
		const auto f_it = _items_map.find(key);
		if (f_it == _items_map.end())
		{
			return false;
		}

		value = f_it->second.first;
		return true;
	}

	void put(const K& key, const V& value)
	{
		// try to get item from cache
		auto list_it = _items_list.end();	
		const auto f_it = _items_map.find(key);
		if (f_it != _items_map.end())
		{
			list_it = f_it->second.second;
		}

		// apply strategy
		if (!_strategy.exists(list_it))
		{
			_strategy.insert<K, V>(key, value);
		}
	}

private:
	TList _items_list;
	TMap _items_map;
	TStrategy _strategy;
};
