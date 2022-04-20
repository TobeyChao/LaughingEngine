#pragma once
#include "MPL/TypeList.h"

template<typename ...ComponentType>
class System : public ISystem
{
	friend EntityAdmin;

public:
	System()
	{
		ParseDataStructure();
	}

	~System()
	{
		m_EntityIDToIndex.clear();
		m_ComponentHash.clear();
		m_Admin = nullptr;
	}

	template<typename T>
	T* GetComponent(EntityID ID)
	{
		return m_Admin->GetComponent<T>(ID);
	}

private:
	virtual void SetEntityAdmin(EntityAdmin* admin)
	{
		m_Admin = admin;
	}

	virtual void OnEntityCreated(const Entity& entity)
	{
		for (const size_t& comHash : m_ComponentHash)
		{
			if (!entity.ComponentHash.contains(comHash))
			{
				return;
			}
		}
		m_EntitiesCache.push_back(entity.EntityID);
		m_EntityIDToIndex[entity.EntityID] = m_EntitiesCache.size() - 1;
	}

	virtual void OnEntityModified(const Entity& entity)
	{
		// 如果改变了的Entity没有符合的组件啦，就把他删掉啦
		for (const size_t& comHash : m_ComponentHash)
		{
			if (!entity.ComponentHash.contains(comHash))
			{
				RemoveEntityCache(entity.EntityID);
				return;
			}
		}
	}

	virtual void OnEntityDestroyed(const Entity& entity)
	{
		RemoveEntityCache(entity.EntityID);
	}

	void RemoveEntityCache(EntityID ID)
	{
		size_t index = m_EntityIDToIndex[ID];
		m_EntitiesCache.erase(m_EntitiesCache.begin() + index);
		m_EntityIDToIndex.erase(ID);
	}

	void ParseDataStructure()
	{
		using Type = TypeList<ComponentType...>;

		ParseDataStructure<Type>(std::make_index_sequence<Size<Type>()>());

#ifdef _DEBUG
		for (auto& hash : m_ComponentHash)
		{
			std::cout << "Hash: " << hash << std::endl;
		}
#endif // DEBUG
	}

	template<typename TypeList, std::size_t... Is>
	constexpr void ParseDataStructure(std::index_sequence<Is...>)
	{
		(ParseDataStructureImpl<typename TypeAt<Is, TypeList>::Type>(), ...);
	}

	template<typename T>
	constexpr void ParseDataStructureImpl()
	{
		m_ComponentHash.insert(typeid(T).hash_code());
	}

protected:
	std::vector<EntityID> m_EntitiesCache;

private:
	std::unordered_map<EntityID, size_t> m_EntityIDToIndex;
	std::unordered_set<size_t> m_ComponentHash;
	EntityAdmin* m_Admin;
};