#include"resource.h"
#include"engine.h"
#include"application.h"
#include"pool.h"
using namespace ecs;

const size_t t_num = 20;
ThreadPool pool(t_num);
//component
struct Pos {
	glm::vec3 pos;
};
struct Vec {
	glm::vec3 vec;
};
struct InPut {
	
};
struct Model {
	glm::mat4 model;
	glm::mat4 scale;
	glm::mat4 rotation;
};
struct uboD{
	DynamicBufferObject* ptr;
	size_t len;
};
void StartUpSystem(Commands commands) {
	for (size_t i = 0; i < 10000; i++) {
		int x = i % 100 - 50;
		int y = i / 100 - 50;
		commands
			.Spawn<Pos, Model, Vec>(Pos{ glm::vec3(x,0.1,y) }, Model{ glm::mat4(1),  glm::scale(glm::mat4(1.), glm::vec3(0.1,.1, .1)) ,glm::mat4(1) }, Vec{ glm::vec3(0.001,0, .001) });
	}
	commands
		.Spawn<Pos, Model, InPut>(Pos{ glm::vec3(0,0.2,0) }, Model{ glm::mat4(1),  glm::scale(glm::mat4(1.), glm::vec3(0.2,.2, .2)) ,glm::mat4(1) }, InPut{});

}

void updateVecSystem(Commands commands, Querier querier, Resources resources) {
	glm::vec3 target= resources.Get<Camera*>()->Target;
	for (auto entity : querier.Query<Vec>()) {
		auto dis = target - querier.Get<Pos>(entity).pos;
		dis.y = 0;
		float length = glm::length(dis);
		float length_target = 1.f;
		querier.Get<Vec>(entity).vec = glm::vec3(0.f);
		float a=std::min((2 * length / length_target - 1), 1.f);
		querier.Get<Vec>(entity).vec = a * 0.01f * glm::normalize(dis);
	}
}
//暴力O(n^2)避免碰撞算法
void avoidCollisionSystem(Commands commands, Querier querier, Resources resources) {


		std::vector<ecs::Entity> query = querier.Query<Vec>();
		for (size_t i = 0; i < query.size(); ++i) {
			for (size_t j = i + 1; j < query.size(); ++j) {
				glm::vec3& posA = querier.Get<Pos>(query[i]).pos;
				glm::vec3& posB = querier.Get<Pos>(query[j]).pos;

				glm::vec3 delta = posB - posA;
				float distance = glm::length(delta);
				float mind = 0.4f;
				if (distance < mind) {
					glm::vec3 correction = (delta / distance) * (mind - distance) * 0.5f;
					querier.Get<Vec>(query[i]).vec = (querier.Get<Vec>(query[i]).vec + querier.Get<Vec>(query[j]).vec) * 0.4f;
					querier.Get<Vec>(query[j]).vec = querier.Get<Vec>(query[i]).vec;
					posA -= correction * 0.5f;
					
					posB += correction * 0.5f;
				}
			}
		}
}
//空间格数据结构
struct GridCell {
	std::vector<size_t> entities; // 存储实体索引
};
class SpatialGrid {
public:
	SpatialGrid(float cellSize, glm::ivec2 gridSize) : cellSize(cellSize), gridSize(gridSize) {
		grid.resize(gridSize.x * gridSize.y);
	}

	void addEntityToCell(size_t entityID, const glm::vec2& position) {
		const float size =0.8;
		glm::ivec2 minCell = worldToCellIndex(position - glm::vec2(size));
		glm::ivec2 maxCell = worldToCellIndex(position + glm::vec2(size));
		glm::ivec2 cellIndex = worldToCellIndex(position);
		for (int x = minCell.x; x <= maxCell.x; ++x) {
			for (int y = minCell.y; y <= maxCell.y; ++y) {
				int index = cellIndexToFlatIndex(glm::ivec2(x, y));
				if (index >= 0 && index < grid.size())
					grid[index].entities.push_back(entityID);
			}
		}	
		/*int index = cellIndexToFlatIndex(cellIndex);
		if(index>=0&&index< grid.size())
		grid[index].entities.push_back(entityID);*/
	}

	std::vector<GridCell>& getCells() {
		return grid;
	}

private:
	glm::ivec2 worldToCellIndex(const glm::vec2& position) {

		return glm::ivec2(glm::floor(position / cellSize))+ gridSize/2;
	}

	int cellIndexToFlatIndex(const glm::ivec2& cellIndex) {
		return cellIndex.x + cellIndex.y * gridSize.x;
	}

	std::vector<GridCell> grid;
	float cellSize;
	glm::ivec2 gridSize;
};

SpatialGrid sgrid(4.f, glm::vec2(100, 100));
//空间格算法实现
void AddtoCellSystem(Commands commands, Querier querier, Resources resources) {
	for (auto entity : querier.Query<Vec>()) {
		glm::vec3& pos=querier.Get<Pos>(entity).pos;
		sgrid.addEntityToCell(entity, glm::vec2(pos.x, pos.z));
	}
}
void CellAvoidCollisionSystem(Commands commands, Querier querier, Resources resources) {
	std::vector<GridCell>& cells = sgrid.getCells();
	auto cal=[&](size_t tindex) {
		for (size_t index = tindex; index < cells.size(); index += t_num) {
			std::vector<size_t>& query = cells[index].entities;
			for (size_t i = 0; i < query.size(); ++i) {
				for (size_t j = i + 1; j < query.size(); ++j) {
					glm::vec3& posA = querier.Get<Pos>(query[i]).pos;
					glm::vec3& posB = querier.Get<Pos>(query[j]).pos;

					glm::vec3 deltaPos = posB - posA;
					float distance = glm::length(deltaPos);
					float minDis = 0.3f;

					if (distance < minDis) {
						glm::vec3& vecA = querier.Get<Vec>(query[i]).vec;
						glm::vec3& vecB = querier.Get<Vec>(query[j]).vec;
						glm::vec3 deltaVel = vecB - vecA;
						glm::vec3 collisionNormal = glm::normalize(deltaPos);
						float relativeVelocity = glm::dot(deltaVel, collisionNormal);
						const float restitution = .0;
						if (relativeVelocity < 0) {
							float impulseStrength = -(1 + restitution) * relativeVelocity; // restitution 是碰撞恢复系数
							glm::vec3 impulse = impulseStrength * collisionNormal;
							vecA -= impulse;
							vecB += impulse ;
						}
						else {

							glm::vec3 correction = (deltaPos / distance) * (minDis - distance) * 0.2f;
							posA -= correction;
							posB += correction;
						}
						//glm::vec3 correction = (deltaPos / distance) * (minDis - distance) * 0.05f;
						//glm::vec3 half_v = (vecA + vecB) * 0.5f;
						//vecA= half_v - (vecA- half_v)*0.8f;
						//vecB= half_v - (vecB - half_v) * 0.8f;

						//querier.Get<Vec>(query[i]).vec = (querier.Get<Vec>(query[i]).vec + querier.Get<Vec>(query[j]).vec) * 0.45f;
						//querier.Get<Vec>(query[j]).vec = querier.Get<Vec>(query[i]).vec;
				
					
					}
				}
			}
			query.clear();
		}
		};
	std::vector<std::future<void>> futures(t_num);
	for (size_t i = 0; i < t_num; i++) {
		futures[i]= pool.enqueue(cal, i);
	}
	for (size_t i = 0; i < t_num; i++) {
		futures[i] .get();
	}
}

void updateModelSystem(Commands commands, Querier querier, Resources resources) {

	for (auto entity : querier.Query<Vec>()) {
		querier.Get<Pos>(entity).pos += querier.Get<Vec>(entity).vec;
		querier.Get<Model>(entity).model = glm::translate(glm::mat4(1.0f), querier.Get<Pos>(entity).pos)*querier.Get<Model>(entity).rotation * querier.Get<Model>(entity).scale;
	}
	for (auto entity : querier.Query<InPut>()) {
		querier.Get<Pos>(entity).pos = resources.Get<Camera*>()->Target;
		querier.Get<Model>(entity).model = glm::translate(glm::mat4(1.0f), querier.Get<Pos>(entity).pos) * querier.Get<Model>(entity).rotation * querier.Get<Model>(entity).scale;
	}
}

void updateDynamicUniformSystem(Commands commands, Querier querier, Resources resources) {

	uint32_t index = 0;
	uboD ubo=resources.Get<uboD>();
	for (auto entity : querier.Query<Model>()) {
		DynamicBufferObject* objectData = (DynamicBufferObject*)(((uint64_t)ubo.ptr + (entity * ubo.len)));//entity编号与动态uniform一一对应
		objectData->model=querier.Get<Model>(entity).model;
		index++;
	}
}
::Resource::Resource(Engine& engine):engine(engine) {
}


void ::Resource::init() {
	sgrid;
	ecsWorld.AddSartupSystem(StartUpSystem);

	ecsWorld.AddSystem(updateVecSystem);
	//1 暴力
	//ecsWorld.AddSystem(avoidCollisionSystem);
	//2 空间格
	ecsWorld.AddSystem(AddtoCellSystem);
	ecsWorld.AddSystem(CellAvoidCollisionSystem);

	ecsWorld.AddSystem(updateModelSystem);
	ecsWorld.AddSystem(updateDynamicUniformSystem);
	ecsWorld.SetResource<uboD>(uboD{ engine.uboDynamic.data ,engine.dynamicAlignment });

	ecsWorld.SetResource<Camera*>(&engine.application->camera);
	ecsWorld.Startup();
}
void ::Resource::update() {
	ecsWorld.Update();
}