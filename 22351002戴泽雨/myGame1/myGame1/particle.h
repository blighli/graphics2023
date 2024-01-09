#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<vector>
namespace MyGame {

	enum UPDATER {
		BASIC
	};

	struct Particle {
	public:
		//lifetime position color velocity
		Particle(float alifetime = 0.f, const glm::vec2& aposition = glm::vec2{ 0.f },
			const glm::vec4& acolor = glm::vec4{ 1.0f }, const glm::vec2& av = glm::vec2{ 0.f }) :
			lifetime(alifetime), position(aposition), color(acolor), velocity(av) {
		}

		auto setPosition(const glm::vec2 apos)->void {
			position = apos;
		}
		auto setVelocity(const glm::vec2 av)->void {
			velocity = av;
		}
		auto setColor(const glm::vec4 acolor)->void {
			color = acolor;
		}
		auto setLifeTime(float alifetime)->void {
			lifetime = alifetime;
		}
		auto getPositon()const ->const glm::vec2& {
			return position;
		}
		auto getVelocity()const ->const glm::vec2& {
			return velocity;
		}
		auto getColor()const ->const glm::vec4& {
			return color;
		}
		auto isAlive()const->bool {
			if (lifetime > 1e-20f)
				return true;
			return false;
		}
		auto getLifetime()const ->float {
			return lifetime;
		}
		auto updateLifeTime(float dt)->void {
			lifetime -= dt;
		}
		

	private:
		glm::vec2 position;
		glm::vec2 velocity;
		glm::vec4 color;
		float lifetime;
	};

	struct ParticleUpdater {
	public:
		virtual auto update(Particle&, float)->void = 0;
	private:
	};
	struct BasicUpdater final:ParticleUpdater {
	public:
		auto update(Particle& particle, float dt)->void;
	private:

	};

	struct ParticleInitializer {
	public:
		virtual auto Init(Particle& particle,float lifetime,const glm::vec2 &position, const glm::vec4 &color,const glm::vec2 velocity)->void = 0;
		virtual auto Generate(float lifetime,const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->Particle = 0;
	private:
	};

	struct BasicInitializer final:ParticleInitializer {
	public:
		auto Init(Particle& particle, float lifetime,const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->void;
		auto Generate(float lifetime,const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->Particle;
	private:
	};

	struct RadomInitializer final :ParticleInitializer {
	public:
		auto Init(Particle& particle, float lifetime, const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->void;
		auto Generate(float lifetime, const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->Particle;
	private:
	};

	struct ParticleGenerator {
	public:
		ParticleGenerator(int aparticleNum, float alifetime,
			const glm::vec2& aposition = glm::vec2{ 0.f }, const glm::vec4 acolor = glm::vec4{ 1.0f },
			const glm::vec2 av = glm::vec2{ 0.f }) :particleNum(aparticleNum), position(aposition), tag(0), color(acolor),
			velocity(av), lifetime(alifetime),initializer(nullptr),updater(nullptr),particles(aparticleNum) {
		}
		auto setInitializer(ParticleInitializer* ainitializer)->void{
			initializer = ainitializer;
		}
		auto setUpdater(ParticleUpdater* aupdater)->void {
			updater = aupdater;
		}
		auto addParticle(int num = 5)->void;
		auto update(float dt)->void;
		auto setPosition(const glm::vec2 apos)->void {
			position = apos;
		}
		auto setLifeTime(float alifetime)->void {
			lifetime = alifetime;
		}
		auto setColor(const glm::vec4 acolor)->void {
			color = acolor;
		}
		auto setVelocity(const glm::vec2 avelocity)->void {
			velocity = avelocity;
		}
		auto getParticles()const->const std::vector<Particle>& {
			return particles;
		}
	private:
		int particleNum;
		int tag;
		float lifetime;
		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 velocity;
		std::vector<Particle> particles;
		ParticleInitializer* initializer;
		ParticleUpdater* updater;
	};
}