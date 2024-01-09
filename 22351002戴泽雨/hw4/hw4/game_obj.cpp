#include"game_obj.h"
#include"sprite_renderer.h"
#include"Game.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"animator.h"

namespace MyGame {
	float Ball::velocityMax = 2000.0f;
	float Paddle::lengthMax = 500.0f;
	auto GameObj::render()const ->void {
		//spr.drawSprite(,)
		getSpriteRenderer().drawSprite(texture, position, size, angle, color);
	}
	auto GameObj::updateByAnimator(float dt)->void {
		if (!animator.isActive()) return;
		//std::cout << "222\n";
		GameObjState& package = animator.getStatePackage(dt);
		//std::cout << "333\n";
		if (animator.PositionChannelActive())
			this->position = package.position;
		if (animator.SizeChannelActive())
			this->size = package.size;
		if (animator.ColorChannelActive())
			this->color = package.color;
		if (animator.AngleChannelActive())
			this->angle = package.angle;
		if (animator.TextureChannelActive())
			this->texture = package.texture;
		if (animator.VisableChannelActive())
			this->visable = package.visable;
	}

	auto GameLevel::LoadFromFile(const std::string& filename)->void {
		Clear();
		//std::stringstream sStream;
		std::string line,tmpstr;
		
		std::ifstream File{ filename };
		
		if(!File) {
			std::cout << "ERROR::GAME_LEVEL:Failed to load game level files" << std::endl;
			return;
		}
		//rules:
		//bricks are first, bonuses are later
		/*
		[Bricks]
		...
		[end]

		[Bonus]
		...
		[end]
		*/
		int blockType=-1;
		std::vector<std::vector<uint>> bricksCanvas(0);
		std::vector<std::vector<uint>> bonusesCanvas(0);
		bool BRK = false, BNS = false;
		while (std::getline(File, line)) {
			//除去空行，找到第一行
			std::istringstream isstream{ line };
			isstream >> tmpstr;
			//std::cout << tmpstr;
			if (tmpstr[0] != '[') continue;

			//start parse bricks
			if (tmpstr == "[Bricks]") {
				if (bricks.size() != 0) {
					badMessage(4);
					return;
				}
				bool end = false;
				while (std::getline(File, line)) {
					//parse [end]
					std::istringstream isstream{ line };
					if (line[0] == '[') {
						isstream >> tmpstr;
						if (tmpstr == "[end]") { end = true; break; }
					}
					//parse line
					bricksCanvas.push_back( parseLine(isstream));
				}
				// check [end]
				if (!end) {
					badMessage(2);
					return;
				}
				parseBricksCanvas(bricksCanvas);
				BRK = true;
			}
			else if(BRK && tmpstr=="[Bonuses]"){
			
				bool end = false;
				while (std::getline(File, line)) {
					//parse [end]
					std::istringstream isstream{ line };
					if (line[0] == '[') {
						isstream >> tmpstr;
						if (tmpstr == "[end]") { end = true; break; }
					}
					//parse line
					bonusesCanvas.push_back(parseLine(isstream));
				}
				// check [end]
				if (!end) {
					badMessage(2);
					return;
				}
				parseBonusesCanvas(bonusesCanvas);
				BNS = true;
			}
			
		}
	}

	auto GameLevel::parseBonusesCanvas(const std::vector<std::vector<uint>>& bonusesCanvas)->void {
		uint bheight, bwidth;
		bheight = bonusesCanvas.size();
		if (bheight == 0 || bheight > height) return;
		bwidth = bonusesCanvas[0].size();
		if (bwidth == 0 || bwidth > width) return;
		for (auto& item : bonusesCanvas) {
			uint tmpl = item.size();
			if (tmpl != bwidth) {
				badMessage(3); return;
			}
		}

		Resource::ResourceManager& r = Resource::getResourceManager();
		for (uint i = 0; i < bheight; i++) {
			for (uint j = 0; j < bwidth; j++) {
				uint item = bonusesCanvas[i][j];
				if (item == 1) {
					Brick& brk = GetBrick(i, j);
					Bonus* b = new Bonus_Addvelocity(200.f, brk.getPosition(), glm::vec2{ fmin(brk.getSize().x,brk.getSize().y) },0.f,r.GetTexture2D("speedup"));
					b->Mask();
					brk.setBonus(b);
					bonuses.push_back(b);
				}
				else if (item == 2) {
					Brick& brk = GetBrick(i, j);
					Bonus* b = new Bonus_AddpaddleLength(30.f, brk.getPosition(), glm::vec2{ fmin(brk.getSize().x,brk.getSize().y) }, 0.f, r.GetTexture2D("lengthAdder"));
					b->Mask();
					brk.setBonus(b);
					bonuses.push_back(b);
				}
				else if (item == 3) {
					Brick& brk = GetBrick(i, j);
					Bonus* b = new Bonus_Addball(1, brk.getPosition(), glm::vec2{ fmin(brk.getSize().x,brk.getSize().y) }, 0.f, r.GetTexture2D("ballAdder"));
					b->Mask();
					brk.setBonus(b);
					bonuses.push_back(b);
				}
				else {

				}
			}
		}
	}

	//0:solid 1:soft
	auto GameLevel::parseBricksCanvas(const std::vector<std::vector<uint>>& bricksCanvas)->void {
		height = bricksCanvas.size();
		if (height == 0) return;
		width = bricksCanvas[0].size();
		if (width == 0) return;
		for (auto& item : bricksCanvas) {
			uint tmpl = item.size();
			if (tmpl != width) {
				badMessage(3); return;
			}
		}
		Game& g = getGame();
		float stepW = static_cast<float>(g.getWidth()) / width;
		float stepH = static_cast<float>(g.getHeight()*0.9) / height;
		Resource::ResourceManager& r = Resource::getResourceManager();
		glm::vec2 size = { stepW, stepH };
		for (uint i = 0; i < height;i++)
		{
			for (uint j = 0; j < width; j++) {
				uint item = bricksCanvas[i][j];
				if (item == 1) {
					bricks.push_back(Brick(true, nullptr,false, glm::vec2{ j * stepW ,i * stepH }, size, 0.f, r.GetTexture2D("solid")));
				}
				else if (item == 2) {
					bricks.push_back(Brick(false, nullptr,false, glm::vec2{ j * stepW ,i * stepH }, size, 0.f, r.GetTexture2D("brick"), glm::vec4{ 1.0,1.0,0.5,1.0 }));
				}																																
				else if (item == 3) {																											
					bricks.push_back(Brick(false, nullptr,false, glm::vec2{ j * stepW ,i * stepH }, size, 0.f, r.GetTexture2D("brick"), glm::vec4{0.5,1.0,0.5,1.0}));
				}																																
				else if (item == 4) {																											
					bricks.push_back(Brick(false, nullptr,false, glm::vec2{ j * stepW ,i * stepH }, size, 0.f, r.GetTexture2D("brick"), glm::vec4{ 1.0,0.5,0.5,1.0}));
				}																																			 
				else if (item == 5) {																														 
					bricks.push_back(Brick(false, nullptr,false, glm::vec2{ j * stepW ,i * stepH }, size, 0.f, r.GetTexture2D("brick"), glm::vec4{ 0.5,0.5,1.0,1.0}));
				}
				else {
					Brick b = Brick(false, nullptr, true);
					b.Mask();
					bricks.push_back(b);
				}
			}
		}
	};

	inline auto GameLevel::parseLine(std::istringstream& isstream)->std::vector<uint> {
		std::vector<uint> ans;
			uint tmp;
			while (isstream >> tmp) {
				ans.push_back(tmp);
			}
			return ans;
	}


	auto Paddle::addLength(float adder)->void {
		if (this->getSize().x > lengthMax - 1e-6f)
			return;
		float newL = this->getSize().x + adder;
		if (newL > lengthMax) newL = lengthMax;
		paddleLengthAnime((*this), newL);
	}

	auto Bonus_Addvelocity::giveBonus()->void {
		std::vector<Ball>& balls = getGame().getBalls();

		for (auto& ball : balls) {
			ball.addVelocity(adder);
		}
	}

	auto Bonus_Addball::giveBonus()->void {
		Game& g = getGame();
		float randomx = ((rand() % 100) - 50) / 10.0f;
		float randomy = ((rand() % 100) - 50) / 10.0f;
		glm::vec2 offset{ randomx,randomy };
		std::vector<Ball>& balls = g.getBalls();

		int activeSize = 0;
		for (auto& item : balls) {
			if (item.isVisable())
				activeSize++;
		}
		if (activeSize < g.getMaxBallNum()) {
			int tag = -1;
			int first = -1;
			for (int i = 0; i < balls.size(); i++) {
				if (!balls[i].isVisable())
					tag = i;
				else {
					if(first==-1) first = i;
				}
			}
			if (tag != -1)
			{
				balls[tag] = balls[first];
				balls[tag].setPosition(balls[tag].getPosition() + offset);
			}
			else
			{
				if (first == -1) return;
				Ball b = balls[first];
				b.setPosition(balls[first].getPosition() + offset);
				balls.push_back(b);
			}
		}
	}

	auto Bonus_AddpaddleLength::giveBonus()->void {
		Paddle& paddle = getGame().getPaddle();
		paddle.addLength(this->adder);
	}

	auto Ball::updateParticleGenerator(float dt)->void {
		particlegenerator.setPosition(getPosition() + glm::vec2{ getRadius() });
		particlegenerator.update(dt);
		particlegenerator.addParticle();
	}
	auto Ball::InitParticleGenerator(ParticleInitializer* initializer, ParticleUpdater* updater,
		float lifetime, glm::vec4 color, glm::vec3 velocity)->void {
		particlegenerator.setLifeTime(lifetime);
		particlegenerator.setColor(color);
		particlegenerator.setVelocity(velocity);
		particlegenerator.setInitializer(initializer);
		particlegenerator.setUpdater(updater);
	}
}