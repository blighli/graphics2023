#pragma once
#include"sprite_renderer.h"
#include"resource_manager.h"
#include<vector>
#include"game_obj.h"
namespace MyGame {
	using uint = unsigned int;
	enum class GAMESTATE {
		GAME_ACTIVE,
		GAME_PAUSE,
		GAME_OVER
	};

	//game is a singeleton
	struct Game {
		static auto getInstance()->Game& {
			static Game g{};
			return g;
		}
		auto render()->void;
		auto update(float dt)->void;
		auto Init(uint viewportW, uint viewportH, uint windowW, uint windowH)->void;
		auto ProcessInput(float dt)->void;
		auto getBalls()->std::vector<Ball>& {
			return balls;
		}
		auto getPaddle()->Paddle& {
			return paddle;
		}
		auto getWidth()const ->uint {
			return Width;
		}
		auto getHeight()const->uint {
			return Height;
		}
		auto setKeys(uint n, bool state)->void {
			Keys[n] = state;
		}
		auto getMaxBallNum()const->int {
			return maxBall;
		}
		auto addBall()->void;
	private:
		///varibles
		GameObj testObj;
		uint Width, Height;
		uint cWidth, cHeight;
		SpriteRenderer* spr;
		glm::vec2 startPosition;
		Resource::ResourceManager* r;
		std::vector<Ball> balls;
		GameLevel level;
		Paddle paddle;
		bool Keys[1024];
		GAMESTATE State;
		float paddleVelocity;
		glm::vec2 ballVelocity;
		glm::vec2 paddleSize;
		float ballRadius;
		GameObj chara;
		uint maxBall;
		//particle
		BasicInitializer basicInitializer;
		BasicUpdater basicUpdater;
		RadomInitializer randomInitializer;

		//big 5
		Game():
			Width(0),Height(0),spr(nullptr),r(nullptr),
			balls(0),paddle(),paddleVelocity(600.0f),
			State(GAMESTATE::GAME_ACTIVE), ballVelocity{0.,-800.0},
			paddleSize{120.0f,20.0f},ballRadius(10.0f),maxBall(20){};
		~Game() {};
		Game(const Game&) {};
		Game(const Game&&)noexcept {};
		Game& operator = (const Game&) {};

		auto handleCollision()->void;
		auto handleBallMove(float dt)->void;
		//auto handleGameOver()->void {};
		auto GameOver()->void;
		auto resetLevel()->void;

	};

	static auto getGame()->Game& {
		return Game::getInstance();
	}



	

}