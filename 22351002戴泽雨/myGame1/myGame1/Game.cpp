#include"Game.h"
#include"shader.h"
#include"texture.h"
#include"resource_manager.h"
#include"game_obj.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<vector>
#include<glfw/glfw3.h>
#include"animator.h"
#include"particle.h"
namespace MyGame {

	//----test-----
	void printvec(const glm::vec2& v) {
		std::cout << "start print vec-----\n";
		std::cout << v.x << " " << v.y << std::endl;
		std::cout << "end of print -------\n\n";
	}
	//----test-----

	//helper
	

	//helper
	auto loadTexture(Resource::ResourceManager*)->void;
	auto AABBdetect(const GameObj& one, const GameObj& two)->bool;
	auto clamp(glm::vec2& src, const glm::vec2& min_clamper, const glm::vec2 max_clamper)->bool;
	auto BallRectDetect(const Ball& ball, const GameObj& two ,glm::vec2* diff = nullptr,glm::vec2*normal=nullptr)->bool;
	auto Reflect(const glm::vec2& dir, const glm::vec2& anormal, bool *canref = nullptr)->glm::vec2;
	auto acBallRectDetect(const Ball& ball, const GameObj& two, glm::vec2* diff, glm::vec2* normal)->bool;
	//end



	auto Game::render()->void {
		//draw backgroud
		spr->drawSprite(r->GetTexture2D("background"), startPosition,
			glm::vec2{ static_cast<float>(Width),static_cast<float>(Height) }, 0.f, glm::vec4{0.3,0.3,0.3,1.0});

		//draw paddle
		spr->drawSprite(paddle,startPosition);

		//draw ball
		for (auto& ball : balls) {
			if (ball.isVisable()) {
				const std::vector<Particle>& particles = ball.getParticleGenerator().getParticles();
				glm::vec2 particlesize = glm::vec2{ ballRadius * 2 };
				for (const auto& particle : particles) {
					if (particle.isAlive()) {
						spr->drawSprite(particle, particlesize, r->GetTexture2D("particle"), startPosition - particlesize * 0.5f);
					}
				}
				spr->drawSprite(ball, startPosition);
			}
		}

		//draw gamelevel
		//draw bonus
		std::vector<Bonus*> bonuses = level.getBonuses();
		for (auto& item : bonuses) {
			if (item->isVisable()) {
				spr->drawSprite((*item), startPosition);
			}
		}

		//draw bricks
		std::vector<Brick>& bricks = level.getBricks();
		for (auto& item : bricks) {
			if(item.isVisable())
				spr->drawSprite(item,startPosition);
		}

		//draw chara
		spr->drawSprite(chara);

	



		//draw testobj
		//spr->drawSprite(testObj);
		//std::cout << testObj.getTexture() << std::endl;

	}
	
	auto Game::Init(uint width,uint height,uint cwidth,uint cheight)->void {
		Width = width;
		Height = height;
		if (cwidth < width)
			cWidth = width;
		else
			cWidth = cwidth;
		if (cheight < height)
			cHeight = cheight;
		else
			cHeight = height;
		

		startPosition = glm::vec2{ (cWidth - Width) / 2,0. };
		spr = &getSpriteRenderer();
		r = &Resource::getResourceManager();

		//load shader
		r->LoadShader("./res/shaders/spritev.hlsl", "./res/shaders/spritef.hlsl", nullptr, "sprite");

		//set projection
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->cWidth), static_cast<float>(this->cHeight), 0.0f, -1.0f, 1.0f);
		r->GetShader("sprite")->Use().SetMatrix4("projection", projection);
		r->GetShader("sprite")->Use().SetInteger("sprite",0);



		//load texture
		loadTexture(r);

		//load level
		level.LoadFromFile("./res/levels/five.lvl");
		//
		//set Game obj
		//screen center
		glm::vec2 bottomCenter{ static_cast<float>(Width) / 2 ,static_cast<float>(Height) };
		
		//set paddle
		paddle = Paddle(glm::vec2{ bottomCenter.x - paddleSize.x/2,bottomCenter.y - paddleSize.y }, paddleSize,
			0.f, r->GetTexture2D("paddle"), glm::vec4{1.0f});

		// set balls, init has one ball
		balls.clear();
		
		//
		glm::vec2 paddlePos = paddle.getPosition();
		balls.push_back(Ball(ballRadius, ballVelocity,true, glm::vec2{ bottomCenter.x - ballRadius, bottomCenter.y - ballRadius * 2 - paddleSize.y },
			0.f, r->GetTexture2D("ball"),glm::vec4{ 1.0f }));
		
		//set balls' particle generator
		balls[0].InitParticleGenerator(&randomInitializer, &basicUpdater, 0.15f, glm::vec4{1.0f,0.5f,0.0f,1.0f});
		
		//set testobj
		testObj = GameObj(glm::vec2{ bottomCenter.x - paddleSize.x / 2,bottomCenter.y - paddleSize.y * 8 }, glm::vec2{paddleSize.y*4},
			0.f, r->GetTexture2D("container"), glm::vec4{ 1.0f });
		
		
		//alice
		chara = GameObj(glm::vec2{ bottomCenter.x - paddleSize.x / 2,bottomCenter.y - paddleSize.y * 8 }, glm::vec2{ paddleSize.y * 4 },
			0.f, r->GetTexture2D("alice0"), glm::vec4{ 1.0f });
		
		
		//bind alice animation
		AliceAnime(chara);
		
		//bind bricks animation
		std::vector<Brick>& bricks = level.getBricks();
		for (auto &brick: bricks) {
			if(!brick.isSolid())
				BrickExplosionBind(brick);
		}
	}

	auto Game::addBall()->void {
		
	}

	auto Game::GameOver()->void {
		std::cout << "GameOver\n";
		State = GAMESTATE::GAME_OVER;
	}

	auto Game::resetLevel()->void{
		State = GAMESTATE::GAME_ACTIVE;
		std::vector<Brick>& bricks = level.getBricks();
		for (auto& brick : bricks) {
			brick.recoverTex();
			if (brick.getTexture()!=nullptr)
				brick.Visualize();
			brick.Recover();
			if (brick.isUseAnime()) {
				brick.getAnimator().turnOFF();
			}
			if (brick.getBonus() != nullptr) {
				brick.getBonus()->Mask();
				brick.getBonus()->setPosition(brick.getPosition());
			}
		}
		paddle.setSize(paddleSize);
		paddle.turnOFFanimator();
		glm::vec2 bottomCenter{ static_cast<float>(Width) / 2 ,static_cast<float>(Height) };
		paddle.setPosition(glm::vec2{ bottomCenter.x - paddleSize.x / 2,bottomCenter.y - paddleSize.y });
		balls.clear();
		glm::vec2 paddlePos = paddle.getPosition();
		balls.push_back(Ball(ballRadius, ballVelocity,true, glm::vec2{ bottomCenter.x - ballRadius, bottomCenter.y - ballRadius * 2 - paddleSize.y },
			0.f, r->GetTexture2D("ball"), glm::vec4{ 1.0f }));
		balls[0].InitParticleGenerator(&randomInitializer, &basicUpdater, 0.15f, glm::vec4{ 1.0f,0.5f,0.0f,1.0f });
	}

	auto Game::update(float dt)->void {
		if (State == GAMESTATE::GAME_ACTIVE) {
			//std::cout << dt << std::endl;

			handleBallMove(dt);
			handleCollision();
			bool over = true;
			float fHeight = static_cast<float>(Height);
			for (auto& ball : balls) {
				if (ball.getPosition().y > fHeight)
					ball.Mask();
				else
					over = false;
			}
			if (over) GameOver();

			for (auto& ball : balls) {
				ball.updateParticleGenerator(dt);
			}


			//update bricks
			std::vector<Brick>& bricks = level.getBricks();
			for (auto& item : bricks) {
				if (item.isUseAnime() && item.getAnimator().isActive()) {
					item.updateByAnimator(dt);
				}
			}


			//update testobj
			if (testObj.isUseAnime() && testObj.getAnimator().isActive()) {
				//std::cout << "in this\n";
				testObj.updateByAnimator(dt);
			}

			//update paddle
			if (paddle.isUseAnime() && paddle.getAnimator().isActive()) {
				//std::cout << "in this\n";
				paddle.updateByAnimator(dt);
			}

			//update chara
			if (chara.isUseAnime() && chara.getAnimator().isActive()) {
				//std::cout << "in this\n";
				chara.updateByAnimator(dt);
			}

			//update bonus fall & paddle get bonus
			std::vector<Bonus*> bonuses = level.getBonuses();
			for (auto& item : bonuses) {
				if (item->isVisable()) {
					//bonus fall
					item->fall(dt);
					if (item->getPosition().y > Height)
						item->Mask();
					else {
						if (AABBdetect((*item), paddle)) {
							item->giveBonus();
							item->Mask();
						}
					}
				}
			}


			
		}
	}
	
	auto Game::handleCollision()->void {
		std::vector<Brick>& bricks = level.getBricks();
		bool DirectionChanged = false;
		//diff ,also normal dir
		glm::vec2 diff{ 0.f };
		glm::vec2 normal{ 0.f };
		glm::vec2 changedv{ 0.f };
		glm::vec2 savedDiff{ 0.f };
		float diffL = 0.f;
		Brick* collideBrk = nullptr;
		//for each ball
		for (auto& ball : balls) {
			//handle bricks collision
			DirectionChanged = false;
			glm::vec2 velocity = ball.getVelocity();
			for (auto& brick : bricks) {
				if (!brick.isDestoryed()) {
					if (acBallRectDetect(ball, brick,&diff,&normal)) {
						//printvec(normal);
						/*if (!brick.isSolid()) {
							collideBrk = &brick;
							brick.Destory();
							brick.turnONanimator();
							if (brick.getBonus() != nullptr) {
								brick.getBonus()->Visualize();
								//printvec(brick.getBonus()->getSize());
							}
						}*/
						if (glm::length(diff)>diffL) {
							DirectionChanged = true;
							changedv = Reflect(velocity, normal);
							diffL = glm::length(diff);
							savedDiff = diff;
							collideBrk = &brick;
						}
					}
				}
			}
			//amend position & cllide brk handle
			if (DirectionChanged) {
				ball.setVelocity(changedv);
				ball.setPosition(ball.getPosition() + savedDiff);
				if(!collideBrk->isSolid()) {
					collideBrk->Destory();
					collideBrk->turnONanimator();
					if (collideBrk->getBonus() != nullptr) {
						collideBrk->getBonus()->Visualize();
						//printvec(brick.getBonus()->getSize());
					}
				}
			}
			//handle paddle collision
			if (BallRectDetect(ball, paddle, &diff)) {
				glm::vec2 v = ball.getVelocity();
				normal = glm::vec2{ 0.,-1. };
				v = Reflect(v, normal);
				float originL = glm::length(v);
				float d = ( ball.getPosition().x + ball.getRadius()) - (paddle.getPositionX() + paddle.getSize().x / 2.0f);
				d /= paddle.getSize().x;
				v.x += originL * d;
				v *= originL / glm::length(v);
				ball.setVelocity(v);
				ball.setPosition(ball.getPosition() + diff);
			}


		}
	}

	auto Game::handleBallMove(float dt)->void {
		for (auto& ball : balls) {
			if (ball.isStuck())
				continue;
			glm::vec2 velocity = ball.getVelocity();
			//std::cout << velocity.x << ' ' << velocity.y << std::endl;
			glm::vec2 newPosition = ball.getPosition() + velocity * dt;
			//std::cout << newPosition.x << " " << newPosition.y << std::endl;
			//std::cout << velocity.x *dt  << ' ' << velocity.y*dt << std::endl;
			//std::cout <<  ball.getRadius() << " " << Width << std::endl;
			if (newPosition.x < 0) {
				newPosition.x = 0.f;
				ball.setVelocity(abs(velocity.x), velocity.y);
			}
			else if ((newPosition.x + ball.getRadius() * 2) > static_cast<float>(Width)) {
				newPosition.x = static_cast<float>(Width) - ball.getRadius() * 2;
				ball.setVelocity(-abs(velocity.x), velocity.y);
			}
			if (newPosition.y < 0) {
				newPosition.y = 0;
				ball.setVelocity(velocity.x, abs(velocity.y));
			}
			ball.setPosition(newPosition);
			//particlesystem
		}
	}



	auto Game::ProcessInput(float dt)->void {
		if (State == GAMESTATE::GAME_ACTIVE) {
			//move paddle
			float x = paddle.getPositionX();
			float deltx = 0.f;
			bool move = false;
			if (Keys[GLFW_KEY_A]) {
				move = true;
				deltx = -paddleVelocity * dt;
			}
			if (Keys[GLFW_KEY_D]) {
				move = true;
				deltx = paddleVelocity * dt;
			}
			if (move) {
				float nx = x + deltx;
				nx = glm::clamp(nx, 0.f, static_cast<float>(Width) - paddle.getSize().x);
				paddle.setPositionX(nx);
				if (nx != x) {
					for (auto& ball : balls) {
						if (ball.isStuck())
						{
							ball.setPosition(glm::vec2{ ball.getPosition().x + deltx,ball.getPosition().y });
							ball.updateParticleGenerator(dt);
						}
					}
				}
			}
			if (Keys[GLFW_KEY_SPACE]) {
				for (auto& ball : balls) {
					ball.setStuck(false);
				}
			}
			if (Keys[GLFW_KEY_B]) {
				chara.turnONanimator();
			}

			if (Keys[GLFW_KEY_U]) {
				//std::cout << "u\n";
				testObj.animatorBegin();
				glm::vec2 position = testObj.getPosition();
				Animator& animator = testObj.getAnimator();
				std::vector<uTex2Dp> textureLine;
				r = &Resource::getResourceManager();
				textureLine.push_back(std::make_pair(1, r->GetTexture2D("explosion0")));
				textureLine.push_back(std::make_pair(2, r->GetTexture2D("explosion1")));
				textureLine.push_back(std::make_pair(3, r->GetTexture2D("explosion2")));
				textureLine.push_back(std::make_pair(4, r->GetTexture2D("explosion3")));
				textureLine.push_back(std::make_pair(5, r->GetTexture2D("explosion4")));
				textureLine.push_back(std::make_pair(6, r->GetTexture2D("explosion5")));
				textureLine.push_back(std::make_pair(7, r->GetTexture2D("explosion6")));
				textureLine.push_back(std::make_pair(8, r->GetTexture2D("explosion7")));
				textureLine.push_back(std::make_pair(9, r->GetTexture2D("explosion8")));
				textureLine.push_back(std::make_pair(10, r->GetTexture2D("explosion8")));
				animator.setType(ANIMETYPE::REPEAT);
				animator.setTextureLine(textureLine);
				animator.ActivateTextureChannel();
	
				testObj.animatorEnd();
				
			}
		}
		else if (State == GAMESTATE::GAME_OVER) {
			if (Keys[GLFW_KEY_R]) {
				resetLevel();
			}
		}
	}







//these are the helper functions only used in thif unit
	auto isInRect(const glm::vec2& point, const glm::vec2& mMin,const glm::vec2&mMax) {
		if (point.x > mMin.x&& point.y > mMin.y&& point.x < mMax.x && point.y < mMax.y) {
			return true;
		}
		return false;
	}

	auto AABBdetect(const GameObj& one, const GameObj& two)->bool {
		const glm::vec2& OnePosition = one.getPosition();
		const glm::vec2& OneSize = one.getSize();
		const glm::vec2& TwoPosition = two.getPosition();
		const glm::vec2& TwoSize = two.getSize();
		bool xoverlap = false, yoverlap = false;
		// xoverlap
		if (TwoPosition.x > (OnePosition.x + OneSize.x) || (TwoPosition.x + TwoSize.x) < OnePosition.x) {
			xoverlap = false;
		}
		else
			xoverlap = true;

		if (TwoPosition.y > (OnePosition.y + OneSize.y) || (TwoPosition.y + TwoSize.y) < OnePosition.y) {
			yoverlap = false;
		}
		else
			yoverlap = true;

		return xoverlap && yoverlap;

	};
	
	// to get if collision and collision state and diff(or collision normal direction)
	auto BallRectDetect(const Ball& ball, const GameObj& two, glm::vec2* diff , glm::vec2* normal)->bool {
		glm::vec2 v1 = { ball.getPosition() + glm::vec2{ball.getRadius()}-(two.getPosition() + two.getSize() / 2.0f) };
		glm::vec2 clampMax = two.getSize()/2.0f;
		glm::vec2 clampMin = -clampMax;
		glm::vec2 v2 = v1;
		bool clamped = clamp(v2, clampMin, clampMax);
		if (!clamped) { if (diff != nullptr) (*diff) = glm::vec2{ 0. }; return true; }

		float d = glm::length(v1 - v2);
		
		if (d < ball.getRadius()) {
			if (diff != nullptr)
				(*diff) = (v1 - v2) * (ball.getRadius() - d)/d;
			float d1 = glm::dot(v1 - v2, glm::vec2{ 1.,0. });
			float d2 = glm::dot(v1 - v2, glm::vec2{ 0.,1. });
			if (normal != nullptr)
			{
				if (abs(d1) > abs(d2)){
					if (d1 > 0)
						(*normal) = glm::vec2{ 1.0, 0. };
					else
						(*normal) = glm::vec2{ -1.0,0. };
				}
				else {
					if (d2 > 0)
						(*normal) = glm::vec2{ 0.,1. };
					else
						(*normal) = glm::vec2{ 0.,-1. };

				}

			}
			//find click origin
			return true;
		}
		return false;
	}

	//handle ball brick contact problem
	auto intersectT(const Ball& ball, const GameObj& obj , glm::vec2 *normal)->glm::vec2 {
		float left, right, up, down;
		const glm::vec2& pos = obj.getPosition();
		const glm::vec2& size = obj.getSize();
		const float radius = ball.getRadius();
		const glm::vec2 Origin = ball.getPosition() + glm::vec2{ radius,radius };
		const glm::vec2 inverseVelocitydir = -glm::normalize(ball.getVelocity());
		left = pos.x - radius;
		right = pos.x + size.x + radius;
		up = pos.y - radius;
		down = pos.y + size.y + radius;
		float t1 = 0.f, t2 = 0.f, t3 = 0.f, t4 = 0.f;
		float tx = 0.f, ty = 0.f;
		float anst = 0.f;
		if (abs(inverseVelocitydir.x) > 1e-6f) {
			t1 = (left - Origin.x) / inverseVelocitydir.x;
			t2 = (right - Origin.x) / inverseVelocitydir.x;
		}
		else {
			t1 = t2 = 1e10f;
		}
		if (abs(inverseVelocitydir.y) > 1e-6f) {
			t3 = (up - Origin.y) / inverseVelocitydir.y;
			t4 = (down - Origin.y) / inverseVelocitydir.y;
		}
		else
		{
			t3 = t4 = 1e10f;
		}
		tx = t1 > t2 ? t1 : t2;
		ty = t3 > t4 ? t3 : t4;
		anst = tx < ty ? tx : ty;
		glm::vec2 intPosition = Origin + inverseVelocitydir * anst;
		int tagx = 0, tagy = 0;
		int tag = 0;
		tagx += intPosition.x >= pos.x ? 1 : 0;
		tagx += intPosition.x > (pos.x + size.x) ? 1 : 0;
		tagy += intPosition.y >= pos.y ? 1 : 0;
		tagy += intPosition.y > (pos.y + size.y) ? 1 : 0;
		tag = tagy * 3 + tagx;
		//std::cout << tag << std::endl;
		/*
		0 1 2 
		3 4 5
		6 7 8
		*/
		if (tag % 2 == 1) {
			if (normal != nullptr) {
				tag /= 2;
				if (tag == 0)
					(*normal) = glm::vec2{ 0.f,-1.f };
				else if (tag == 1)
					(*normal) = glm::vec2{ -1.0f,0.f };
				else if (tag == 2)
					(*normal) = glm::vec2{ 1.0f,0.f };
				else
					(*normal) = glm::vec2{ 0.f,1.0f };
			}
				
			return inverseVelocitydir * anst;
		}
		else
		{
			tag = (tag + 1) / 3;
			//std::cout << tag << std::endl;

			glm::vec2 circleOrigin = pos;
			circleOrigin.x += size.x * (tag % 2);
			circleOrigin.y += size.y * (tag / 2);
			glm::vec2 toCircleOrigin = circleOrigin - Origin;
			float lenc = glm::dot(toCircleOrigin, inverseVelocitydir);
			glm::vec2 h = inverseVelocitydir * lenc - toCircleOrigin;
			float deltm = glm::length(intPosition - circleOrigin - h) - sqrt(abs(radius * radius - (h.x * h.x + h.y * h.y)));

			/*std::cout << (tag % 2) << " " << (tag / 2) << std::endl;
			std::cout << "p1\n";
			printvec(Origin + inverseVelocitydir * anst);
			std::cout << "p2\n";
			printvec(circleOrigin);*/
			anst -= deltm;
			
			if (normal != nullptr)
				(*normal) = glm::normalize(Origin + inverseVelocitydir * anst - circleOrigin);
			return inverseVelocitydir * anst;
		}
	}

	//acurate physical detect
	auto acBallRectDetect(const Ball& ball, const GameObj& two, glm::vec2 * diff,glm::vec2 *normal)->bool {
		glm::vec2 v1 = { ball.getPosition() + glm::vec2{ball.getRadius()}-(two.getPosition() + two.getSize() / 2.0f) };
		glm::vec2 clampMax = two.getSize() / 2.0f;
		glm::vec2 clampMin = -clampMax;
		glm::vec2 v2 = v1;
		bool clamped = clamp(v2, clampMin, clampMax);
		//if (!clamped) { if (diff != nullptr) (*diff) = glm::vec2{ 0.f }; return true; }

		float d = glm::length(v1 - v2);

		if (d < ball.getRadius() || (!clamped)) {
			/*if (diff != nullptr)
				(*diff) = (v1 - v2) * (ball.getRadius() - d) / d;*/
			//find click origin
			if (diff != nullptr)
				(*diff) = intersectT(ball, two, normal);
			return true;
		}
		return false;
	}

	//this clamp will change src,and return clampstate
	auto clamp(glm::vec2& src, const glm::vec2& min_clamper, const glm::vec2 max_clamper)->bool {
		//glm::vec2 ans;
		bool clampX = false, clampY = false;
		if (src.x < min_clamper.x) {
			src.x = min_clamper.x;
			clampX = true;
		}
		else if (src.x > max_clamper.x) {
			src.x = max_clamper.x;
			clampX = true;
		}
		if (src.y < min_clamper.y) {
			src.y = min_clamper.y;
			clampY = true;
		}
		else if (src.y > max_clamper.y) {
			src.y = max_clamper.y;
			clampY = true;
		}

		return (clampX || clampY);
	}
	
	auto Reflect(const glm::vec2 &dir,const glm::vec2 &anormal,bool *canref)->glm::vec2 {
		glm::vec2 normal = glm::normalize(anormal);
		float d = glm::dot(dir, normal);
		if (d > 0) {
			if(canref)
				(*canref) = false;
			return dir;
		}
		glm::vec2 ans = dir - normal * d * 2.0f;
		if(canref) 
			(*canref) = true;
		return ans;
	}
//end
	
	auto loadTexture(Resource::ResourceManager* r)->void {
		r->LoadTexture2D("./res/textures/background.png", true, "background");
		r->LoadTexture2D("./res/textures/paddle.png", true, "paddle");
		r->LoadTexture2D("./res/textures/ball.png", true, "ball");
		r->LoadTexture2D("./res/textures/block.png", false, "brick");
		r->LoadTexture2D("./res/textures/block_solid.png", false, "solid");
		r->LoadTexture2D("./res/textures/container.jpg", false, "container");
		r->LoadTexture2Dmulti("./res/textures/Explosion/explosion.png", true, "explosion", 3, 3);
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa000.png", true, "alice0");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa001.png", true, "alice1");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa002.png", true, "alice2");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa003.png", true, "alice3");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa004.png", true, "alice4");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa005.png", true, "alice5");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa006.png", true, "alice6");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa007.png", true, "alice7");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa008.png", true, "alice8");
		r->LoadTexture2D("./res/textures/aliceanime/alicebrust/spellBa009.png", true, "alice9");
		r->LoadTexture2D("./res/textures/Bonus/speed.png", true, "speedup");
		r->LoadTexture2D("./res/textures/Bonus/length.png", true, "lengthAdder");
		r->LoadTexture2D("./res/textures/Bonus/addball.png", true, "ballAdder");
		r->LoadTexture2D("./res/textures/particles/spellBulletDa001.png", true, "particle");
	}
	
}