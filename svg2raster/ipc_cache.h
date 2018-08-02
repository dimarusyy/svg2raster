#pragma once

#include <boost/optional.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/pair.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_recursive_mutex.hpp>

#include <memory>

//////////////////////////////////////////////////////////////////////////

template <typename T>
using ipc_shared_allocator_t = boost::interprocess::allocator<T, boost::interprocess::managed_shared_memory::segment_manager>;

//////////////////////////////////////////////////////////////////////////
// std::string
using ipc_string_t = boost::interprocess::basic_string<char, std::char_traits<char>, ipc_shared_allocator_t<char>>;
using ipc_string_allocator_t = ipc_shared_allocator_t<ipc_string_t>;

//////////////////////////////////////////////////////////////////////////
// std::pair<std::string, std::string>

template <typename K, typename V>
using ipc_shared_pair_t = boost::interprocess::pair<K, V>;

template <typename K, typename V>
using ipc_shared_pair_allocator_t = ipc_shared_allocator_t<std::pair<K, V>> ;

//////////////////////////////////////////////////////////////////////////

template <typename K, typename V>
using ipc_shared_map_t = boost::interprocess::map<K, V, std::less<K>, ipc_shared_pair_allocator_t<K const, V>>;

//////////////////////////////////////////////////////////////////////////s

template <typename T>
using ipc_shared_list_t = boost::interprocess::list<T, ipc_shared_allocator_t<T>>;

//////////////////////////////////////////////////////////////////////////

namespace details
{
	static boost::interprocess::permissions default_ipc_permissions()
	{
		//shared memory permissions
		boost::interprocess::permissions ipc_shm_permissions;
#ifdef _WIN32
		ipc_shm_permissions.set_unrestricted();
#else
		//set 0600 on Linux
		ipc_shm_permissions.set_permissions(0600);
#endif
		return ipc_shm_permissions;
	}

	static std::string default_shared_mutex_name()
	{
		return std::string("svg2raster.ipc_cache.lock");
	}

	static std::string default_shared_memory_name()
	{
		return std::string("svg2raster.memory");
	}

	static boost::interprocess::managed_shared_memory::size_type default_shared_memory_size()
	{
		return 256 * 1024;  // 256KB	
	}
}

//////////////////////////////////////////////////////////////////////////

namespace details
{
	template <size_t N, typename TList, typename TMap>
	struct lru_strategy_t
	{
		lru_strategy_t(TList& list, TMap& map)
			: _list(list), _map(map)
		{
		}

		typename TList::iterator exists(typename TList::iterator hit_it) const
		{
			if (hit_it != _list.end())
			{
				// in cache, move top
				_list.splice(_list.begin(), _list, hit_it);
				return hit_it;
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
				return _list.end();
			}
		}

		template <typename K, typename V>
		void insert(const K& key, const V& value) const
		{
			// insert to front
			_list.push_front(key);
			_map.emplace(key, std::make_pair(value, _list.begin()));
		}

		template <typename V>
		void assign(typename TMap::iterator hit_it, const V& value) const
		{
			hit_it->second = std::make_pair(value, _list.begin());
		}

	private:
		TList& _list;
		TMap& _map;
	};
}

//////////////////////////////////////////////////////////////////////////

template <size_t N>
struct ipc_cache_t 
{
	using shared_list_t = ipc_shared_list_t<ipc_string_t>;
	using shared_map_t = ipc_shared_map_t<ipc_string_t, ipc_shared_pair_t<ipc_string_t, shared_list_t::iterator>>;

	ipc_cache_t(const std::string& shm_name, boost::interprocess::managed_shared_memory::size_type shm_size = details::default_shared_memory_size())
		: _shared_mutex(boost::interprocess::open_or_create, 
						details::default_shared_mutex_name().c_str(),
						details::default_ipc_permissions()),
		  _shm(boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,
														  details::default_shared_memory_name().c_str(),
														  shm_size,
														  0,
														  details::default_ipc_permissions())),
		  _p_items_list{nullptr},
		  _p_items_map{nullptr}
	{
		boost::interprocess::scoped_lock<boost::interprocess::named_recursive_mutex> lock(_shared_mutex);
		if (!lock.owns())
		{
			throw std::runtime_error("can't lock shared named mutex");
		}
	
		// construct list
		_p_items_list = _shm.find_or_construct<shared_list_t>((shm_name + ".list").c_str())(_shm.get_allocator<typename shared_list_t::value_type>());
		// construct map
		_p_items_map = _shm.find_or_construct<shared_map_t >((shm_name + ".map").c_str())(_shm.get_allocator<typename shared_map_t::value_type>());
		
		_p_strategy = std::make_unique<details::lru_strategy_t<N, shared_list_t, shared_map_t>>(*_p_items_list, *_p_items_map);
	}

	boost::optional<std::string> get(const std::string& key)
	{
		boost::interprocess::scoped_lock<boost::interprocess::named_recursive_mutex> lock(_shared_mutex);
		if (!lock.owns())
		{
			throw std::runtime_error("can't lock shared named mutex");
		}

		auto* segment = _shm.get_segment_manager();
		ipc_string_t shm_key(key.c_str(), segment);

		const auto f_it = _p_items_map->find(shm_key);
		if (f_it != _p_items_map->end())
		{
			const auto rr = f_it->second.first;
			return boost::make_optional(std::string(f_it->second.first.begin(), f_it->second.first.end()));
		}
		return boost::none;
	}

	virtual void put(const std::string& key, const std::string &value)
	{
		boost::interprocess::scoped_lock<boost::interprocess::named_recursive_mutex> lock(_shared_mutex);
		if (!lock.owns())
		{
			throw std::runtime_error("can't lock shared named mutex");
		}

		// create shared memory key
		ipc_string_allocator_t segment(_shm.get_segment_manager());
		ipc_string_t key_shared(key.c_str(), segment);

		// create shared memory value
		ipc_string_t value_shared(value.c_str(), segment);

 		// try to get item from cache
 		auto list_it = _p_items_list->end();
 		const auto f_it = _p_items_map->find(key_shared);
		if (f_it != _p_items_map->end())
		{
			list_it = f_it->second.second;
		}

		// apply strategy
		if (_p_strategy->exists(list_it) == _p_items_list->end())
 		{
 			_p_strategy->insert<ipc_string_t, ipc_string_t>(key_shared, value_shared);
 		}
		else
		{
			//f_it->second = std::make_pair(value_shared, _p_items_list->begin());
			_p_strategy->assign<ipc_string_t>(f_it, value_shared);
		}
	}

private:
	mutable boost::interprocess::named_recursive_mutex _shared_mutex;
	boost::interprocess::managed_shared_memory _shm;

	shared_list_t* _p_items_list;
	shared_map_t* _p_items_map;

	std::unique_ptr<details::lru_strategy_t<N, shared_list_t, shared_map_t>> _p_strategy;
};