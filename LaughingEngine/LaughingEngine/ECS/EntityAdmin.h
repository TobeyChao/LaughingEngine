#ifndef __ENTITY_ADMIN__
#define __ENTITY_ADMIN__
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
	// 创建Entity
	template<typename T>
	EntityID CreateEntity()
	{
		Entity entity;
		entity.Hash = typeid(T).hash_code();
		entity.EntityID = m_NextEntityID;
		entity.MemHandle = m_ArchetypeStorage.Get(typeid(T).hash_code()).Allocate();
		// 设置Entity签名
		SetEntityComHash<T>(entity, std::make_index_sequence<Size<T>()>());
		m_Entities[m_NextEntityID] = std::move(entity);

		NotifySystemEntityCreated(m_Entities[m_NextEntityID]);
		return m_NextEntityID++;
	}

	// 设置Component
	template<typename T, typename ...Args>
	T* SetComponentData(EntityID ID, Args&&... args)
	{
		const Entity& entity = m_Entities[ID];
		T* ret = m_ArchetypeStorage.Get(entity.Hash).template Create<T, Args...>(entity.MemHandle, std::forward<decltype(args)>(args)...);
		return ret;
	}

	// 获取Component
	template<typename T>
	T* GetComponent(EntityID ID)
	{
		const Entity& entity = m_Entities[ID];
		return m_ArchetypeStorage.Get(entity.Hash).Get<T>(entity.MemHandle);
	}

	// 销毁Entity
	void DestroyEntity(EntityID ID)
	{
		NotifySystemEntityCreated(m_Entities[ID]);

		m_ArchetypeStorage.Get(m_Entities[ID].Hash).Free(m_Entities[ID].MemHandle);
		m_Entities.erase(ID);
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
		m_ArchetypeStorage.Get(typeid(TypeList).hash_code()).template Init<TypeList>();
	}

	void Update(float deltaTime)
	{
		std::for_each(m_Systems.begin(), m_Systems.end(), [deltaTime](auto system)
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
		std::for_each(m_Systems.begin(), m_Systems.end(), [&entity](auto system)
			{
				system->OnEntityCreated(entity);
			});
	}

	void NotifySystemEntityModified(const Entity& entity)
	{
		std::for_each(m_Systems.begin(), m_Systems.end(), [&entity](auto system)
			{
				system->OnEntityModified(entity);
			});
	}

	void NotifySystemEntityDestroyed(const Entity& entity)
	{
		std::for_each(m_Systems.begin(), m_Systems.end(), [&entity](auto system)
			{
				system->OnEntityDestroyed(entity);
			});
	}

private:
	// System
	std::vector<std::shared_ptr<ISystem>> m_Systems;
	// Entity
	EntityID m_NextEntityID = 1;
	std::unordered_map<EntityID, Entity> m_Entities;
	// Component Pool
	ArchetypeStorage m_ArchetypeStorage;
};
#endif // __ENTITY_ADMIN__