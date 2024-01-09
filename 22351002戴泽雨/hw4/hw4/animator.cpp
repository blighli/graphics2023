#include"animator.h"
#include"Game.h"
#include"game_obj.h"
#include"resource_manager.h"
namespace MyGame {

	//helper
	template<typename T>
	T LinearInterpolate(const T& one, const T& two, float coef) {
		T ans = one * coef + two * (1.0f - coef);
		return ans;
	}
	//end
	
	GameObjState::GameObjState(GameObj& obj) :
		position(obj.getPosition()),
		angle(obj.getAngle()),
		size(obj.getSize()),
		color(obj.getColor()),
		texture(obj.getTexture()),
		visable(obj.isVisable())
	{}
	/*auto Animator::setKeyFrame(const std::vector<float>& time, bool isAccumulate)->void {
		if (time.size() == 0) return;
		if (!isAccumulate) {
			//check
			float tmp = 0.f;
			for (auto& item : time) {
				if (item < 0.f) {
					KeyFrame.clear();
					return;
				}
				tmp += item;
				KeyFrame.push_back(tmp);
			}
			KeyFrame = time;
		}
		else {
			float tmp = 0.f;
			for (auto& item : time) {
				if (item - tmp < 0.f)
					return;
				tmp = item;
			}
		}
	}*/
	const float TimeLine::maxTime = 1e6f;
	const float Animator::maxTime = 1e6f;
	auto Animator::getStatePackage(float dt)->GameObjState& {
		if (state == ANIMATORSTATE::OFF) return ObjStatePackage;
		acTime += dt;
		//std::cout << TextureLineP << std::endl;
		//std::cout << maxAnimeTime << std::endl;
		//std::cout << "444\n";
		//handle the end
		if (acTime >= maxAnimeTime) {
			if (type == ANIMETYPE::LAST) {
				turnOFF();
				return ObjStatePackage;
			}
			else if (type == ANIMETYPE::TEMP) {
				turnOFF();
				return OriginStatePakage;
			}
			else if (type == ANIMETYPE::REPEAT) {
				while(acTime>maxAnimeTime)
					acTime -= maxAnimeTime;
				TimeLineP = PositionLineP = SizeLineP = AngleLineP = TextureLineP = ColorLineP = VisableLineP = 0;
				return ObjStatePackage;
			}
		}
	

		//2步走：
		//1:更新P
		//2:插值

		
		//更新P，P永远指向下一个状态，不会越界

		//std::cout << "555\n";
		//position
		if (PositionChannel == true) {
			while (PositionLineP + 1 <= PositionLine.size()) {
				//try to update P
				float nextT = PositionLine[PositionLineP].first * perFrame;
				if (acTime >= nextT) {
					PositionLineP++;
				}
				else break;
			}
			//插值
			float LastTime = 0.f, coef = 0.f, LaterTime = 0.f;
			glm::vec2 one;
			//当前时间点没有超过最后一帧
			if (PositionLineP != PositionLine.size()) {
				if (PositionLineP >= 1) {
					LastTime = PositionLine[PositionLineP - 1].first * perFrame;
					one = PositionLine[PositionLineP - 1].second;
				}
				else {
					LastTime = 0.f;
					one = OriginStatePakage.position;
				}
				LaterTime = PositionLine[PositionLineP].first * perFrame;
				coef = (LaterTime - acTime) / (LaterTime - LastTime);
				//std::cout << LastTime << " " << acTime << " " << LaterTime << std::endl;
				//std::cout << coef << std::endl;
				ObjStatePackage.position = LinearInterpolate(one, PositionLine[PositionLineP].second, coef);
			}
			else {
				ObjStatePackage.position = PositionLine.back().second;
			}
			
		}


		//size
		if (SizeChannel == true) {
			while (SizeLineP + 1 < SizeLine.size()) {
				//try to update P
				float nextT = SizeLine[SizeLineP].first * perFrame;
				if (acTime >= nextT) {
					SizeLineP++;
				}
				else break;
			}
			//插值
			float LastTime = 0.f, coef = 0.f, LaterTime = 0.f;
			glm::vec2 one;
			if (SizeLineP != SizeLine.size()) {
				if (SizeLineP >= 1) {
					LastTime = SizeLine[SizeLineP - 1].first * perFrame;
					one = SizeLine[SizeLineP - 1].second;
				}
				else {
					LastTime = 0.f;
					one = OriginStatePakage.size;
				}
				LaterTime = SizeLine[SizeLineP].first * perFrame;
				coef = (LaterTime - acTime) / (LaterTime - LastTime);
				ObjStatePackage.size = LinearInterpolate(one, SizeLine[SizeLineP].second, coef);
			}
			else {
				ObjStatePackage.size = SizeLine.back().second;
			}
		}

		//color
		if (ColorChannel == true) {
			while (ColorLineP + 1 < ColorLine.size()) {
				//try to update P
				float nextT = ColorLine[ColorLineP].first * perFrame;
				if (acTime >= nextT) {
					ColorLineP++;
				}
				else break;
			}
			//插值
			float LastTime = 0.f, coef = 0.f, LaterTime = 0.f;
			glm::vec4 one;
			if (ColorLineP != ColorLine.size()) {
				if (ColorLineP >= 1) {
					LastTime = ColorLine[ColorLineP - 1].first * perFrame;
					one = ColorLine[ColorLineP - 1].second;
				}
				else {
					LastTime = 0.f;
					one = OriginStatePakage.color;
				}
				LaterTime = ColorLine[ColorLineP].first * perFrame;
				coef = (LaterTime - acTime) / (LaterTime - LastTime);
				ObjStatePackage.color = LinearInterpolate(one, ColorLine[ColorLineP].second, coef);
			}
			else
				ObjStatePackage.color = ColorLine.back().second;
		}

		//angle
		if (AngleChannel == true) {
			while (AngleLineP + 1 < AngleLine.size()) {
				//try to update P
				float nextT = AngleLine[AngleLineP].first * perFrame;
				if (acTime >= nextT) {
					AngleLineP++;
				}
				else break;
			}
			//插值
			float LastTime = 0.f, coef = 0.f, LaterTime = 0.f;
			float one;
			if (AngleLineP != AngleLine.size()) {
				if (AngleLineP >= 1) {
					LastTime = AngleLine[AngleLineP - 1].first * perFrame;
					one = AngleLine[AngleLineP - 1].second;
				}
				else {
					LastTime = 0.f;
					one = OriginStatePakage.angle;
				}
				LaterTime = AngleLine[AngleLineP].first * perFrame;
				coef = (LaterTime - acTime) / (LaterTime - LastTime);
				ObjStatePackage.angle = LinearInterpolate(one, AngleLine[AngleLineP].second, coef);
			}
			else
				ObjStatePackage.angle = AngleLine.back().second;
		}


		//Texture
		if (TextureChannel == true) {
			while (TextureLineP + 1 < TextureLine.size()) {
				//try to update P
				float nextT = TextureLine[TextureLineP].first * perFrame;
				if (acTime >= nextT) {
					TextureLineP++;
				}
				else break;
			}
			//不需要插值
			//std::cout << TextureLineP << std::endl;
			if (TextureLineP != TextureLine.size()) {
				if (TextureLineP >= 1) {
					ObjStatePackage.texture = TextureLine[TextureLineP - 1].second;
				}
				else
					ObjStatePackage.texture = OriginStatePakage.texture;
			}
			else
				ObjStatePackage.texture = TextureLine.back().second;

		}
	

		//visable
		if (VisableChannel == true) {
			while (VisableLineP + 1 <= VisableLine.size()) {
				//try to update P
				float nextT = VisableLine[VisableLineP].first * perFrame;
				if (acTime >= nextT) {
					VisableLineP++;
				}
				else break;
			}
			//不需要插值
			if (VisableLineP != VisableLine.size()) {
				if (VisableLineP >= 1) {
					ObjStatePackage.visable = VisableLine[VisableLineP - 1].second;
				}
				else
					ObjStatePackage.visable = OriginStatePakage.visable;
			}
			else
				ObjStatePackage.visable = VisableLine.back().second;
		}
		return ObjStatePackage;
	
	}

	Animator::Animator(GameObj& obj) :ObjStatePackage(obj), OriginStatePakage(obj),
		PositionLine(0), SizeLine(0), AngleLine(0),
		TextureLine(0), ColorLine(0), VisableLine(0), TimeLineP(0),
		PositionLineP(0), SizeLineP(0), AngleLineP(0), TextureLineP(0),
		ColorLineP(0), VisableLineP(0), type(ANIMETYPE::TEMP), state(ANIMATORSTATE::OFF),
		acTime(0.f), PositionChannel(false), SizeChannel(false), AngleChannel(false),
		TextureChannel(false), ColorChannel(false), VisableChannel(false), maxAnimeTime(0.f),
		perFrame(0.2f) {
	}
	Animator::Animator():ObjStatePackage(), OriginStatePakage(),
		PositionLine(0), SizeLine(0), AngleLine(0),
		TextureLine(0), ColorLine(0), VisableLine(0), TimeLineP(0),
		PositionLineP(0), SizeLineP(0), AngleLineP(0), TextureLineP(0),
		ColorLineP(0), VisableLineP(0), type(ANIMETYPE::TEMP), state(ANIMATORSTATE::OFF),
		acTime(0.f), PositionChannel(false), SizeChannel(false), AngleChannel(false),
		TextureChannel(false), ColorChannel(false), VisableChannel(false), maxAnimeTime(0.f),
		perFrame(0.2f) {

	}
	auto Animator::Init(GameObj& obj)->void {
		ObjStatePackage = GameObjState(obj);
		OriginStatePakage = GameObjState(obj);
		acTime = 0.f;
		TimeLineP = PositionLineP = SizeLineP = AngleLineP = TextureLineP = ColorLineP = VisableLineP = 0;
	}




















	// helper
	auto  BrickExplosionBind(Brick& brick)->void {
		brick.animatorBegin();
		Animator& animator = brick.getAnimator();
		Resource::ResourceManager* r = &Resource::getResourceManager();
		std::vector<uTex2Dp> textureLine;
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
		animator.setTextureLine(textureLine);
		animator.ActivateTextureChannel();

		std::vector<ub> visableLine;
		visableLine.push_back(std::make_pair(9, false));
		animator.setVisableLine(visableLine);
		animator.ActivateVisableChannel();

		animator.setPerFrame(0.1f);

		animator.setType(ANIMETYPE::LAST);
		brick.animatorEnd(false);
	}
	auto AliceAnime(GameObj& alice) ->void{
		alice.animatorBegin();
		Animator& animator = alice.getAnimator();
		Resource::ResourceManager* r = &Resource::getResourceManager();
		std::vector<uTex2Dp> textureLine;
		textureLine.push_back(std::make_pair(1, r->GetTexture2D("alice0")));
		textureLine.push_back(std::make_pair(2, r->GetTexture2D("alice1")));
		textureLine.push_back(std::make_pair(3, r->GetTexture2D("alice2")));
		textureLine.push_back(std::make_pair(4, r->GetTexture2D("alice3")));
		textureLine.push_back(std::make_pair(5, r->GetTexture2D("alice4")));
		textureLine.push_back(std::make_pair(6, r->GetTexture2D("alice5")));
		textureLine.push_back(std::make_pair(7, r->GetTexture2D("alice6")));
		textureLine.push_back(std::make_pair(8, r->GetTexture2D("alice7")));
		textureLine.push_back(std::make_pair(9, r->GetTexture2D("alice8")));
		textureLine.push_back(std::make_pair(10, r->GetTexture2D("alice9")));
		textureLine.push_back(std::make_pair(11, r->GetTexture2D("alice9")));
		animator.setTextureLine(textureLine);
		animator.ActivateTextureChannel();
		animator.setPerFrame(0.1f);
		animator.setType(ANIMETYPE::TEMP);
		alice.animatorEnd(false);

	}
	auto paddleLengthAnime(Paddle& paddle,float newL)->void {
		paddle.animatorBegin();
		Animator& animator = paddle.getAnimator();
		Resource::ResourceManager* r = &Resource::getResourceManager();
		std::vector<uvec2> sizeLine;
		sizeLine.push_back(std::make_pair(4, glm::vec2{ newL,paddle.getSize().y }));
		animator.setSizeLine(sizeLine);
		animator.ActivateSizeChannel();
		animator.setPerFrame(0.1f);
		animator.setType(ANIMETYPE::LAST);
		paddle.animatorEnd();
	}
}