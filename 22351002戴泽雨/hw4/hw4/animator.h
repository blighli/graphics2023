#pragma once
//#include"Game.h"
#include"texture.h"
#include"sprite_renderer.h"
#include<vector>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<utility>



namespace MyGame {
	enum class ANIMATORSTATE {
		ON,
		OFF
	};
	enum class ANIMETYPE {
		TEMP,
		REPEAT,
		LAST
	};
	using uint = unsigned int;
	using uf = std::pair<uint, float>;
	using uvec2 = std::pair<uint, glm::vec2>;
	using uvec3 = std::pair<uint, glm::vec3>;
	using uvec4 = std::pair<uint, glm::vec4>;
	using uTex2Dp = std::pair<uint, Resource::Texture2D*>;
	using ub = std::pair<uint, bool>;
	
	//helper
	struct Brick;
	struct GameObj;
	struct Paddle;
	auto BrickExplosionBind(Brick& brick)->void;
	auto AliceAnime(GameObj& alice) ->void;
	auto paddleLengthAnime(Paddle& paddle, float newL)->void;
	//end

	struct GameObjState {
		glm::vec2 position, size;
		glm::vec4 color;
		Resource::Texture2D* texture;
		float angle;
		bool visable;
		GameObjState(GameObj& obj);
		GameObjState() :position(0.f),size(0.f),color(0.f),texture(nullptr),angle(0.f),visable(false) {
		};
	};
	//Animator only provide the game obj state,don't do rendering
	//the render part is done by Game

	struct TimeLine {
	public:
		TimeLine():acTime(0.f){}
		auto Init() ->void{
			acTime = 0.f;
		}
		auto accumulate(float dt) {
			acTime += dt;
			if (acTime > maxTime)
				acTime = maxTime;
		}
		auto getTime()->float {
			return acTime;
		}
	private:
		static const float maxTime;
		float acTime;

	};


	//6 channel
	struct Animator {
	public:
		Animator(GameObj&);
		Animator();
		auto isActive()->bool {
			if (state == ANIMATORSTATE::ON)
				return true;
			return false;
		}
		auto turnON(GameObj& obj)->bool {
			if (state == ANIMATORSTATE::ON)
				return true;
			calMaxTime();
			if (maxAnimeTime < 1e-20f) {
				std::cout << "Failed to turn on animator\n";
				return false;
			}
			state = ANIMATORSTATE::ON;
			Init(obj);
			return true;
		}
		auto turnOFF()->void {
			state = ANIMATORSTATE::OFF;
		}
		auto setType(ANIMETYPE atype)->void {
			type = atype;
		}
		//auto setKeyFrame(const std::vector<float>& time, bool isAccumulate = false)->void;
		auto setSizeLine(const std::vector<uvec2>& aSizeLine)->void {
			//do not have to check
			//updateMaxTime(aSizeLine);
			if(checkRationality(aSizeLine))
				SizeLine = aSizeLine;
		}
		auto setPositionLine(const std::vector<uvec2>& aPositionLine)->void {
			//the rationality of Position line need the user to determine
			//updateMaxTime(aPositionLine);
			if (checkRationality(aPositionLine))
				PositionLine = aPositionLine;
		}
		auto setAngleLine(const std::vector<uf>& aAngleLine)->void {
			//do not have to check
			//updateMaxTime(aAngleLine);
			if (checkRationality(aAngleLine))
				AngleLine = aAngleLine;
		}
		auto setTextureLine(const std::vector<uTex2Dp> aTextureLine)->void{
			//updateMaxTime(aTextureLine);
			if (checkRationality(aTextureLine))
				TextureLine = aTextureLine;
		}
		auto setColorLine(const std::vector<uvec4>& aColorLine)->void {
			//updateMaxTime(aColorLine);
			if (checkRationality(aColorLine))
				ColorLine = aColorLine;
		}
		auto setVisableLine(const std::vector<ub>& aVisableLine)->void {
			//updateMaxTime(aVisableLine);
			if (checkRationality(aVisableLine))
				VisableLine = aVisableLine;
		}
		auto PositionChannelActive() ->bool {
			return PositionChannel;
		}
		auto SizeChannelActive() ->bool {
			return SizeChannel;
		}
		auto ColorChannelActive() ->bool {
			return ColorChannel;
		}
		auto AngleChannelActive() ->bool {
			return AngleChannel;
		}
		auto TextureChannelActive() ->bool {
			return TextureChannel;
		}
		auto VisableChannelActive() ->bool {
			return VisableChannel;
		}

		auto ActivatePositionChannel()->void {
			if(PositionLine.size()!=0)
				PositionChannel = true;
		}
		auto ActivateSizeChannel()->void {
			if (SizeLine.size() != 0)
				SizeChannel = true;
		}
		auto ActivateColorChannel()->void {
			if (ColorLine.size() != 0)
				ColorChannel = true;
		}
		auto ActivateAngleChannel()->void {
			if (AngleLine.size() != 0) {
				AngleChannel = true;
			}
		}
		auto ActivateTextureChannel()->void {
			if (TextureLine.size() != 0)
				TextureChannel = true;
		}
		auto ActivateVisableChannel()->void {
			if (VisableLine.size() != 0)
				VisableChannel = true;
		}

		auto allClear()->void {
			PositionLine.clear();
			SizeLine.clear();
			AngleLine.clear();
			TextureLine.clear();
			ColorLine.clear();
			VisableLine.clear();
			PositionChannel = SizeChannel = AngleChannel = TextureChannel = ColorChannel = VisableChannel =false;
		}
		
		auto Init(GameObj&)->void;
		
		auto getStatePackage(float)->GameObjState&;

		auto setPerFrame(float t)->void {
			if (t < 1e-5) return;
			perFrame = t;
		}

	private:
		std::vector<uvec2> PositionLine;
		std::vector<uvec2> SizeLine;
		std::vector<uf> AngleLine;
		std::vector<uTex2Dp> TextureLine;//无需插值
		std::vector<uvec4> ColorLine;
		std::vector<ub> VisableLine;//无需插值
		float maxAnimeTime;
		float perFrame;
		uint TimeLineP, PositionLineP, SizeLineP, AngleLineP, TextureLineP, ColorLineP, VisableLineP;
		bool PositionChannel, SizeChannel, AngleChannel, TextureChannel, ColorChannel, VisableChannel;
		ANIMETYPE type;
		ANIMATORSTATE state;
		//GameObj&
		GameObjState ObjStatePackage;
		GameObjState OriginStatePakage;
		float acTime = 0.f;
		static const float maxTime;
		template<typename T>
		auto updateMaxTime(const T& container)->void {
			if (container.size() == 0)return;
			float tmp = container.back().first * perFrame;
			maxAnimeTime = tmp > maxAnimeTime ? tmp : maxAnimeTime;
		}
		template<typename T>
		auto checkRationality(const T&container)->bool {
			for (uint i = 1; i < container.size(); i++) {
				if (container[i].first <= container[i - 1].first)
					return false;
			}
			return true;
		}
		auto calMaxTime()->void {
			maxAnimeTime = 0.f;
			updateMaxTime(PositionLine);
			updateMaxTime(ColorLine);
			updateMaxTime(SizeLine);
			updateMaxTime(AngleLine);
			updateMaxTime(VisableLine);
			updateMaxTime(TextureLine);
		}

	};
};