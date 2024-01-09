#pragma once
#include"resource_manager.h"
#include"animator.h"
#include"particle.h"
#include<vector>
namespace MyGame {
	using uint = unsigned int;
	struct GameObj {
	public:
		//position scale angle texture ambientcolor
		GameObj(const glm::vec2& p = glm::vec2{ 0.0,0.0 }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0,1.0,1.0,1.0 }) :
			position(p), size(s), angle(ag), texture(tex), color(acolor) , visable(true),animator(),useAnime(false),oriTexture(tex){
		}


		auto render()const ->void;
		auto setSize(const glm::vec2& asize) ->void {
			size = asize;
		}
		auto setSize(const float& x, const float& y) ->void {
			size.x = x;
			size.y = y;
		}
		auto setPosition(const glm::vec2& aPos) ->void {
			position = aPos;
		}
		auto setPosition(const float& x, const float& y) ->void {
			position.x = x;
			position.y = y;
		}
		auto setPositionX(const float& x)->void {
			position.x = x;
		}
		auto setPositionY(const float& y)->void {
			position.y = y;
		}
		auto setAngle(const float& aangle) {
			angle = aangle;
		}
		auto getSize()const ->const glm::vec2& {
			return size;
		}
		auto getPosition()const ->const glm::vec2& {
			return position;
		}
		auto getTexture()const ->Resource::Texture2D* {
			return texture;
		}
		auto getAngle()const ->float {
			return angle;
		}
		auto getColor()const->const glm::vec4& {
			return color;
		}
		auto Visualize()->void{
			visable = true;
		}
		auto Mask()->void {
			visable = false;
		}
		auto isVisable()->bool {
			return visable;
		}
		auto isUseAnime()->bool {
			return useAnime;
		}
		auto UseAnime()->void {
			useAnime = true;
		}
		auto getAnimator()->Animator& {
			return animator;
		}
		auto updateByAnimator(float dt)->void;

		auto animatorBegin() ->void {
			if(!useAnime)
				UseAnime();
			animator.allClear();
		}
		auto animatorEnd(bool turnon=true)->void {
			if(turnon)
				animator.turnON(*this);
		}
		auto turnONanimator()->void{
			animator.turnON(*this);
		}
		auto turnOFFanimator()->void {
			animator.turnOFF();
		}
		auto recoverTex()->void {
			texture = oriTexture;
		}
	private:
		bool visable;
		//position(pixils)
		glm::vec2 position;
		//scale size
		glm::vec2 size;
		//rotate angle
		float angle;
		//texture
		Resource::Texture2D* texture;
		Resource::Texture2D* oriTexture;
		//albedo color
		glm::vec4 color;
		//animator
		Animator animator;
		bool useAnime;
	};

	struct Paddle final :GameObj {
	public:
		//position scale angle texture ambientcolor
		Paddle(const glm::vec2& p = glm::vec2{ 0.,0. }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 })
			:GameObj(p, s, ag, tex, acolor) {}
		auto getPositionX()const ->float{ 
			return getPosition().x;
		}
		auto getPositionY()const ->float {
			return getPosition().y;
		}
		auto addLength(float adder)->void;
	private:
		static float lengthMax;
	};


	struct Ball final :GameObj {
	public:
		//radius velocity position angle texture color
		Ball(float aradius = 1.0, const glm::vec2& avelocity = glm::vec2{ 0.,-0.1 },bool astuck=true, const glm::vec2& p = glm::vec2{ 0.,0. },
			 float ag = 0, Resource::Texture2D* tex = nullptr,
			glm::vec4 acolor = glm::vec4{ 1.0 }) :GameObj(p, glm::vec2{aradius * 2, aradius * 2}, ag, tex, acolor), 
			stuck(astuck),radius(aradius),velocity(avelocity),particlegenerator(500,0.f) {}
		auto setVelocity(const glm::vec2& v)->void {
			velocity = v;
		}
		auto setVelocity(const float& x,const float&y)->void {
			velocity.x = x;
			velocity.y = y;
		}
		auto addVelocity(float adder)->void {
			glm::vec2 dir = glm::normalize(velocity);
			velocity += dir * adder;
			if (glm::length(velocity) > velocityMax)
				velocity = dir*velocityMax;
		}
		auto multVelocity(float multer = 1.5) {
			velocity *= multer;
		}
		auto getVelocity()const ->const glm::vec2&{
			return velocity;
		}
		auto getRadius()const->float {
			return radius;
		}
		auto setStuck(bool st)->void {
			stuck = st;
		}
		auto isStuck()->bool {
			return stuck;
		}
		auto getParticleGenerator()-> ParticleGenerator& {
			return particlegenerator;
		}
		//feng zhuang 
		auto InitParticleGenerator(ParticleInitializer* initializer, ParticleUpdater* updater,
			float lifetime, glm::vec4 color = glm::vec4{ 1.0f }, glm::vec3 velocity = glm::vec3{ 0.f })->void;
		auto updateParticleGenerator(float dt)->void;
		
	private:
		static float velocityMax;
		// r
		float radius;
		// v
		glm::vec2 velocity;
		//stuck
		bool stuck;
		//particle generator
		ParticleGenerator particlegenerator;
	};

	struct Bonus :GameObj {
	public:
		Bonus(const glm::vec2& p = glm::vec2{ 0.0,0.0 }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 }) :GameObj(p, s, ag, tex, acolor),
		downVelocity(200.0f){
		}
		virtual auto giveBonus()->void = 0;
		auto setDownVelocity(float v) ->void{
			downVelocity = v;
		}
		auto fall(float dt)->void {
			setPosition(getPosition() + glm::vec2{ 0.,1. } * downVelocity * dt);
		}
	private:
		float downVelocity;
	};

	struct Bonus_Addvelocity final :Bonus {
	public:
		//adder position scale angle texture ambientcolor
		Bonus_Addvelocity(float aadder = 200.0f, const glm::vec2& p = glm::vec2{ 0.0,0.0 }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 }) :Bonus(p, s, ag, tex, acolor), adder(aadder) {
		}
		auto giveBonus()->void;
		auto setAdder(float ad)->void {
			if (ad < 1e-6f)
				return;
			adder = ad;
		}
	private:
		float adder;
	};

	struct Bonus_Addball final :Bonus{
	public:
		Bonus_Addball(int aadder = 1, const glm::vec2& p = glm::vec2{ 0.0,0.0 }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 }) :Bonus(p, s, ag, tex, acolor), adder(aadder) {
		}
		auto giveBonus()->void;
	private:
		int adder;
	};

	struct Bonus_AddpaddleLength final:Bonus {
	public:
		Bonus_AddpaddleLength(float aadder = 30.0f, const glm::vec2& p = glm::vec2{ 0.0,0.0 }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 }) :Bonus(p, s, ag, tex, acolor), adder(aadder) {
		}
		auto giveBonus()->void;
	private:
		float adder;
	};

	struct Brick final :GameObj {
	public:
		auto isSolid()->bool {
			return Solid;
		}
		//solid,position scale angle texture ambientcolor
		Brick(bool aSolid = false, Bonus* abonus = nullptr,bool adestoryed = false, const glm::vec2& p = glm::vec2{ 0.,0. }, const glm::vec2& s = glm::vec2{ 1.0,1.0 }, float ag = 0,
			Resource::Texture2D* tex = nullptr, glm::vec4 acolor = glm::vec4{ 1.0 })
			:GameObj(p, s, ag, tex, acolor), Solid(aSolid), bonus(abonus),destoryed(adestoryed) {}
		auto setBonus(Bonus* bns)->void {
			bonus = bns;
		}
		auto isDestoryed()->bool {
			return destoryed;
		}
		auto Destory()->void {
			destoryed = true;
		}
		auto Recover()->void {
			destoryed = false;
		}
		auto getBonus()->Bonus* {
			return bonus;
		}
	private:
		bool Solid;
		Bonus* bonus;
		bool destoryed;
	};
	
	struct GameLevel {
	public:
		GameLevel():bricks(0),width(0),height(0){}
		auto LoadFromFile(const std::string&)->void;
		auto getBricks()->std::vector<Brick>&{
			return bricks;
		}
		auto getBonuses()->std::vector<Bonus*>& {
			return bonuses;
		}
	private:
		auto badMessage(int a)->void {
			switch (a) {
				case 0:std::cout << "ERROR::GAMELEVEL:Failed to load\n"; break;
				case 1:std::cout << "ERROR::GAMELEVEL:Bad format\n"; break;
				case 2:std::cout << "ERROR::GAMELEVEL:Bad format:there is no [end]\n"; break;
				case 3:std::cout << "ERROR::GAMELEVEL:Bad format:width not match\n"; break;
				case 4:std::cout << "ERROR::GAMELEVEL:Bad format:multi [bricks] block\n"; break;
			}
		}
		//parse a line read from file,no matter it is bricks'line or bonuses'line
		auto parseLine(std::istringstream& isstream)->std::vector<uint>;

		//parse a bricks canvas
		auto parseBricksCanvas(const std::vector<std::vector<uint>> &bricksCanvas)->void;
		auto parseBonusesCanvas(const std::vector<std::vector<uint>>& bonusesCanvas)->void;
		//parse a bonuses line
		//auto parseBonusesLine(const std::vector<uint>& bricksLine)->void;

	
		auto GetBrick(const uint&x,const uint&y)->Brick&{
			uint n = x * width + y;
			return bricks[n];
		}
		
		auto Clear() {
			for (auto item : bonuses) {
				delete item;
			}
			bonuses.clear();
			bricks.clear();
			width = height = 0;
		}
		std::vector<Brick> bricks;
		std::vector<Bonus*> bonuses;
		uint width, height;
	};
	
}