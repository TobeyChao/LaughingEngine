#ifndef ENTITYADMIN_
#define ENTITYADMIN_
#include "Entity.h"
#include "ISystem.h"
#include "ArchetypeStorage.h"
#include <unordered_map>
#include <memory>

// 存储：
// 系统列表
// 实体哈希表
// 组件池
class EntityAdmin
{
public:
	EntityAdmin()
	{
		m_Entities.reserve(256);
	}

	// 创建Entity
	template<typename T>
	EntityID CreateEntity()
	{
		EntityID ID;
		if (m_FreeEntities.empty())
		{
			ID = m_NextEntityID++;
			Entity entity;
			SetEntityInfo<T>(entity, ID);
			m_Entities.push_back(std::move(entity));
		}
		else
		{
			ID = m_FreeEntities.top();
			m_FreeEntities.pop();
			Entity& entity = m_Entities[ID];
			SetEntityInfo<T>(entity, ID);
			SetEntityComHash<T>(entity, std::make_index_sequence<Length<T>()>());
		}

		NotifySystemEntityCreated(m_Entities[ID]);
		return ID;
	}

	// 设置Component
	template<typename T, typename ...Args>
	T* SetComponentData(EntityID ID, Args&&... args)
	{
		const Entity& entity = m_Entities[ID];
		T* ret = m_ArchetypeStorage.Get(entity.PoolIndex).template Create<T, Args...>(entity.MemHandle, std::forward<decltype(args)>(args)...);
		return ret;
	}

	// 获取Component
	template<typename T>
	T* GetComponent(EntityID ID)
	{
		const Entity& entity = m_Entities[ID];
		return m_ArchetypeStorage.Get(entity.PoolIndex).Get<T>(entity.MemHandle);
	}

	// 销毁Entity
	void DestroyEntity(EntityID ID)
	{
		Entity& entity = m_Entities[ID];
		NotifySystemEntityDestroyed(entity);
		m_ArchetypeStorage.Get(entity.PoolIndex).Free(entity.MemHandle);
		entity.IsValid = false;
		m_FreeEntities.push(ID);
	}

	// 注册系统
	template <typename T>
	void RegisterSystem()
	{
		std::shared_ptr<ISystem> system = std::make_shared<T>();
		system->SetEntityAdmin(this);
		m_Systems.push_back(std::move(system));
	}

	// 注册Archetype
	template <typename TypeList>
	void RegisterArchetype()
	{
		m_ArchetypeStorage.AddPool(typeid(TypeList).hash_code()).template Init<TypeList>();
	}

	void Update(float deltaTime)
	{
		std::ranges::for_each(m_Systems, [deltaTime](auto system)
			{
				system->Update(deltaTime);
			});
	}

	void Shutdown()
	{
		m_Entities.clear();
		m_Systems.clear();
		m_ArchetypeStorage.DestroyAll();
	}

private:
	template<typename T>
	void SetEntityInfo(Entity& entity, EntityID ID)
	{
		entity.IsValid = true;
		entity.Hash = typeid(T).hash_code();
		entity.EntityID = ID;
		entity.PoolIndex = m_ArchetypeStorage.GetIndex(typeid(T).hash_code());
		entity.MemHandle = m_ArchetypeStorage.Get(entity.PoolIndex).Allocate();
		SetEntityComHash<T>(entity, std::make_index_sequence<Length<T>()>());
	}

	template<typename TypeList, std::size_t... Is>
	constexpr void SetEntityComHash(Entity& entity, std::index_sequence<Is...>)
	{
		(SetEntityComHashImpl<typename TypeAt<Is, TypeList>::Type>(entity), ...);
	}

	template<typename T>
	constexpr void SetEntityComHashImpl(Entity& entity)
	{
		entity.ComponentHash.insert(typeid(T).hash_code());
	}

	void NotifySystemEntityCreated(const Entity& entity)
	{
		std::ranges::for_each(m_Systems, [&entity](auto system)
			{
				system->OnEntityCreated(entity);
			});
	}

	void NotifySystemEntityModified(const Entity& entity)
	{
		std::ranges::for_each(m_Systems, [&entity](auto system)
			{
				system->OnEntityModified(entity);
			});
	}

	void NotifySystemEntityDestroyed(const Entity& entity)
	{
		std::ranges::for_each(m_Systems, [&entity](auto system)
			{
				system->OnEntityDestroyed(entity);
			});
	}

private:
	// System
	std::vector<std::shared_ptr<ISystem>> m_Systems;
	// Entity
	EntityID m_NextEntityID = 0;
	std::vector<Entity> m_Entities;
	std::stack<EntityID> m_FreeEntities;
	// Component Pool
	ArchetypeStorage m_ArchetypeStorage;
};
#endif // ENTITYADMIN_
