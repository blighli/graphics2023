#pragma once
#include <iostream>
#include<vector>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#define assertm(msg,expr) assert(((void)msg,(expr)))
namespace ecs {
	//稀疏集sparse set/packed array 从component查询entity
	class SparseSet {
	public:
		void Add(size_t index) {
			if (index >= sparse_.size()) {
				sparse_.resize(index * 2 + 1, -1);
			}
			if (sparse_[index] == -1) {
				sparse_[index] = density_.size();
				density_.push_back(index);

			}
		}
		void Remove(size_t index) {
			int find = (index < sparse_.size()) ? sparse_[index] : -1;
			if (find >= 0) {
				//调整原末尾
				sparse_[density_.back()] = find;
				std::swap(density_[find], density_.back());
				//删除新末尾
				sparse_[density_.back()] = -1;
				density_.pop_back();
			}
		}
		bool Contain(size_t index) {
			return index < sparse_.size() && sparse_[index] != -1;
		}
		void Print() {
			for (size_t i = 0; i < density_.size(); i++) {
				std::cout << density_[i] << " ";
			}
			std::cout << "\n";
		}
		//迭代器
		class Iterator {
		public:
			Iterator(std::vector<int>& vec, size_t pos = 0) : vec_(vec), pos_(pos) {}

			// 解引用运算符
			int& operator*() {
				return vec_[pos_];
			}

			// 前缀递增
			Iterator& operator++() {
				pos_++;
				return *this;
			}

			// 后缀递增
			Iterator operator++(int) {
				Iterator temp = *this;
				++(*this);
				return temp;
			}

			// 相等运算符
			bool operator==(const Iterator& other) const {
				return pos_ == other.pos_;
			}

			// 不等运算符
			bool operator!=(const Iterator& other) const {
				return !(*this == other);
			}

		private:
			std::vector<int>& vec_;
			size_t pos_;
		};

		// begin 和 end 方法
		Iterator begin() {
			return Iterator(density_);
		}

		Iterator end() {
			return Iterator(density_, density_.size());
		}
	private:
		std::vector<int> density_;
		std::vector<int> sparse_;
	};
	//

	using ComponentID = uint32_t;
	using Entity = uint32_t;


	struct Resource {};
	struct Component {};
	template<typename Category>
	class IndexGetter final {
	public:
		template<typename T>
		static uint32_t Get() {
			static uint32_t id = curIdx_++;
			return id;
		}
	private:
		inline static uint32_t curIdx_ = 0;

	};

	template<typename T>
		requires std::is_integral_v<T>
	struct IDGenerator final {
	public:
		static T Gen() {
			return curId_++;
		}
	private:
		inline static T curId_ = {};
	};
	using EntityGenerator = IDGenerator<Entity>;

	class Commands;
	class Resources;
	class Querier;
	using UpdateSystem = void(*)(Commands, Querier, Resources);
	using StartupSystem = void(*)(Commands);
	class World final {
		friend class Commands;
		friend class Resources;
		friend class Querier;
		using ComponentContainer = std::unordered_map<ComponentID, void*>;

	public:
		World() = default;
		World(const World&) = delete;
		World& operator=(const World&) = delete;
		World& AddSartupSystem(StartupSystem sys) {
			startupSystems_.push_back(sys);
			return *this;
		}
		World& AddSystem(UpdateSystem sys) {
			updateSystems_.push_back(sys);
			return *this;
		}
		template<typename T>
		World& SetResource(T&& resource);

		template<typename T>
		World& RemoveResource();
		void Startup();
		void Update();
		void Shutdown() {
			entities_.clear();
			resources_.clear();
			componentMap_.clear();

		}
	private:
		struct Pool final {
			std::vector<void*> instances;
			std::vector<void*> cache;
			using CreateFunc = void* (*)(size_t, size_t&);
			using DestroyFunc = void(*)(void*);

			CreateFunc create;
			DestroyFunc destroy;
		public:
			Pool(CreateFunc create, DestroyFunc destroy) :create(create), destroy(destroy) {}

			void* Create() {
				const size_t newNum = 10050;
				//if (!cache.empty()) {
				//	instances.push_back(cache.back());
				//	cache.pop_back();
				//}
				//else {
				//	instances.push_back(create());
				//}
				if (cache.empty()) {
					size_t len;
					char* ptr = (char*)create(newNum, len);
					for (int i = newNum - 1; i >= 0; i--) {
						cache.push_back(ptr + i * len);
					}
				}
				instances.push_back(cache.back());
				cache.pop_back();

				return instances.back();
			}
			void Destroy(void* elem) {
				if (auto it = find(instances.begin(), instances.end(), elem); it != instances.end()) {
					cache.push_back(*it);
					std::swap(*it, instances.back());
					instances.pop_back();
				}
				else {
					assertm("your element not in pool", false);
				}
			}
		};
		struct ComponentInfo
		{
			Pool pool;
			SparseSet sparseSet;
			ComponentInfo(Pool::CreateFunc create, Pool::DestroyFunc destroy) :pool(create, destroy) {}
			ComponentInfo() :pool(nullptr, nullptr) {}
		};
		using ComponentMap = std::unordered_map<ComponentID, ComponentInfo>;
		ComponentMap componentMap_;
		std::unordered_map<Entity, ComponentContainer> entities_;
		struct ResourceInfo {
			void* resource = nullptr;
			using CreateFunc = void* (*)(void);
			using DestroyFunc = void(*)(void*);
			CreateFunc create;
			DestroyFunc destroy;
			ResourceInfo() = default;
			ResourceInfo(CreateFunc create, DestroyFunc destroy) :create(create), destroy(destroy) {
				assertm("you must give a non-null create function", create);
				assertm("you must give a non-null create function", destroy);
			}
			~ResourceInfo() {
				destroy(resource);
			}
		};
		std::unordered_map<ComponentID, ResourceInfo>resources_;
		std::vector<StartupSystem>startupSystems_;
		std::vector<UpdateSystem>updateSystems_;
	};



	class Commands final {
	public:
		Commands(World& world) :world_(world) {

		}
		template<typename... ComponetTypes>
		Commands& Spawn(ComponetTypes&&...components) {
			SpawnAndReturn<ComponetTypes...>(std::forward<ComponetTypes>(components)...);
			return *this;
		}
		template<typename... ComponetTypes>
		Entity SpawnAndReturn(ComponetTypes&&...components) {
			Entity entity = EntityGenerator::Gen();
			doSpawn(entity, std::forward<ComponetTypes>(components)...);
			return entity;
		}
		Commands& Destroy(Entity entity) {
			if (auto it = world_.entities_.find(entity); it != world_.entities_.end()) {
				for (auto& [id, component] : it->second) {
					World::ComponentInfo& componentInfo = world_.componentMap_[id];
					componentInfo.pool.Destroy(component);
					componentInfo.sparseSet.Remove(entity);
				}
				world_.entities_.erase(it);
			}
			return *this;
		}
		template<typename T>
		Commands& SetResource(T&& resource) {
			auto index = IndexGetter<Resource>::Get<T>();
			if (auto it = world_.resources_.find(index); it != world_.resources_.end()) {
				assertm("resource already exists", !it->second.resource);
				it->second.resource = new T(std::forward<T>(resource));
			}
			else {
				auto newIt = world_.resources_.emplace(index, World::ResourceInfo(
					[]()->void* {return new T; },
					[](void* elem) {delete (T*)elem; }
				));
				newIt.first->second.resource = new T;
				*((T*)newIt.first->second.resource) = std::forward<T>(resource);
			}
			return *this;
		}
		template<typename T>
		Commands& RemoveResource() {
			auto index = IndexGetter<Resource>::Get<T>();
			if (auto it = world_.resources_.find(index); it != world_.resources_.end()) {
				delete (T*)it->second.resource;
				it->second.resource = nullptr;
			}

			return *this;
		}
	private:
		World& world_;
		template<typename T, typename... Remains>
		void doSpawn(Entity entity, T&& component, Remains&&...remains) {
			auto index = IndexGetter<Component>::Get<T>();
			if (auto it = world_.componentMap_.find(index); it == world_.componentMap_.end()) {
				world_.componentMap_.emplace(index, World::ComponentInfo(
					[](size_t size, size_t& len)->void* {len = sizeof(T); return new T[size]; },
					[](void* elem) {delete (T*)elem; }
				));
			}
			World::ComponentInfo& componentInfo = world_.componentMap_[index];
			void* elem = componentInfo.pool.Create();
			*((T*)elem) = std::forward<T>(component);

			componentInfo.sparseSet.Add(entity);

			auto it = world_.entities_.emplace(entity, World::ComponentContainer{});
			it.first->second[index] = elem;
			if constexpr (sizeof...(remains) != 0) {
				doSpawn<Remains...>(entity, std::forward<Remains>(remains)...);
			}
		}


	};

	class Resources final {
	public:
		Resources(World& world) :world_(world) {}

		template <typename T>
		bool Has() {
			auto index = IndexGetter<Resource>::Get<T>();
			auto it = world_.resources_.find(index);
			return it != world_.resources_.end() && it->second.resource;
		}
		template<typename T>
		T& Get() {
			auto index = IndexGetter<Resource>::Get<T>();
			return *(T*)world_.resources_[index].resource;
		}
	private:
		World& world_;
	};
	class Querier final {
	public:
		Querier(World& world) :world_(world) {}
		template<typename... Components>
		std::vector<Entity> Query() {
			std::vector<Entity> entities;
			doQuery<Components...>(entities);
			return entities;
		}
		template <typename T>
		bool Has(Entity entity) {
			auto index = IndexGetter<Component>::Get<T>();
			auto it = world_.entities_.find(entity);
			return it != world_.entities_.end() && it->second.find(index) != it->second.end();
		}
		template<typename T>
		T& Get(Entity entity) {
			auto index = IndexGetter<Component>::Get<T>();
			return *(T*)world_.entities_[entity][index];
		}
	private:
		World& world_;
		template<typename T, typename...Remains>
		void doQuery(std::vector<Entity>& outEntities) {
			auto index = IndexGetter<Component>::Get<T>();

			World::ComponentInfo& info = world_.componentMap_[index];
			for (auto e : info.sparseSet) {
				if constexpr (sizeof...(Remains) != 0) {
					doQueryRemains<Remains...>(e, outEntities);
				}
				else {
					outEntities.push_back(e);
				}
			}

		}
		template<typename T, typename...Remains>
		bool doQueryRemains(Entity entity, std::vector<Entity>& outEntities) {
			auto index = IndexGetter<Component>::Get<T>();
			auto& componentContainer = world_.entities_[entity];
			if (auto it = componentContainer.find(index); it == componentContainer.end()) {
				return false;
			}
			if constexpr (sizeof...(Remains) == 0) {
				outEntities.push_back(entity);
				return true;
			}
			else {
				return doQueryRemains<Remains...>(entity, outEntities);
			}
		}
	};
	template<typename T>
	inline World& World::SetResource(T&& resource) {
		Commands commands(*this);
		commands.SetResource(std::forward<T>(resource));
		return *this;
	}
	template<typename T>
	inline World& World::RemoveResource() {
		Commands commands(*this);
		commands.RemoveResource<T>();
		return *this;
	}
	inline void World::Startup() {
		for (auto sys : startupSystems_) {
			sys(Commands{ *this });
		}
	}

	inline void World::Update() {
		for (auto sys : updateSystems_) {
			sys(Commands{ *this }, Querier{ *this }, Resources{ *this });
		}
	}
}