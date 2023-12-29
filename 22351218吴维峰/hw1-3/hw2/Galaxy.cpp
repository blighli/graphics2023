#include "Galaxy.h"
Galaxy::Galaxy(int iGalaxy) {
	this->iGalaxy = iGalaxy;
	
}

void Galaxy::setTimeStart(double timeStart) {
	this->timeStart = timeStart;
}

void Galaxy::initGalaxy(float xStar, float yStar, float zStar) {
	srand((int)time(0));
	iGalaxy = (int(rand() * iGalaxy)) % 10;
	P[0].setStarJudge(true);

	int possibility = (rand() + iGalaxy) % 100;
	if (possibility < 10) {
		n = 1;
	}
	else if(possibility < 25){
		n = 2;
	}
	else if (possibility < 50) {
		n = 3;
	}
	else if (possibility < 70) {
		n = 4;
	}
	else if (possibility < 90) {
		n = 5;
	}
	else {
		n = 6;
	}
	std::cout << n << "\n";
	this->xStar = xStar;
	this->yStar = yStar;
	this->zStar = zStar;

	xRotation = zRotation = rand() % 3 + 1 + iGalaxy;
	yRotation = ((rand() % 2) + 1) * (xRotation + zRotation);
	xSpin = zSpin = 0.0;
	ySpin = 1.0;

	
	R[0] = 16.0;
	rRevolution[0] = 0;
	tex[0] = 0;
	char tex0[] = "res/0.bmp";
	P[0].init(tex0, R[0]);
	P[0].setTRevolutionTSpin(10, 100);

	int revolution = 0,
		gasPossibilityPlus = 0,
		texrand;
	numBig = 0;
	for (int i = 1; i <= n; i++) {
		int gasPossibility = rand() % 100;
		bool gasIf = false;
		
		gasPossibility += gasPossibilityPlus;
		if (n > 4) {
			if (i > 1 && numBig <= 2 && gasPossibility >= 80) {
				gasIf = true;
				numBig++;
			}
		}
		else {
			if (i > 1 && numBig == 0 && gasPossibility >= 80) {
				gasIf = true;
				numBig++;
			}
		}
		if (gasIf) {
			
			revolution = revolution + 80 + (rand() + i * 15) % 40;
			rRevolution[i] = revolution;
			std::cout << "dis:" << rRevolution[i] << "\n";
			texrand = (rand() + i + iGalaxy) % 4;
			std::cout << texrand << "\n";
			tex[i] = 6 + texrand;
			R[i] = 10.0;
			TRevolution[i] = revolution * (3 + (rand() + i * 20) % 2);
			
			P[i].setTRevolutionTSpin(TRevolution[i], 10 + rand() % 10);
		}
		else {
			gasPossibilityPlus += 10;
			revolution = revolution + 70 + (rand() + i * 10) % 40;
			rRevolution[i] = revolution;
			std::cout << "dis:" << rRevolution[i] << "\n";
			texrand = (rand() + i + iGalaxy) % 4;
			std::cout << texrand << "\n";
			tex[i] = 1 + texrand;
			R[i] = 4.0;
			TRevolution[i] = revolution * (3 + (rand() + i * 20) % 2);
			
			P[i].setTRevolutionTSpin(TRevolution[i], 5 + rand() % 10);
		}
		char texture_name1[100] = "res/";
		char texture_name2[100];
		char texture_name3[] = ".bmp";
		_itoa_s(tex[i], texture_name2, 10);
		strcat_s(texture_name1, 100, texture_name2);
		strcat_s(texture_name1, 100, texture_name3);
		P[i].init(texture_name1, R[i]);
	}
	
	{
		int possibilityS = rand() % 100;
		nS = 0;
		if (numBig == 0) {
			if (possibilityS < 50) {
				nS = 0;
			}
			else {
				nS = 1;
			}
		}
		else if (numBig == 1 && n <= 4) {
			if (possibilityS < 30) {
				nS = 1;
			}
			else if (possibilityS < 90) {
				nS = 2;
			}
			else {
				nS = 3;
			}
		}
		else if (numBig == 1 && n > 4) {
			if (possibilityS < 25) {
				nS = 1;
			}
			else if (possibilityS < 75) {
				nS = 2;
			}
			else {
				nS = 3;
			}
		}
		else {
			if (possibilityS < 50) {
				nS = 2;
			}
			else {
				nS = 3;
			}
		}
		
	}
	{
		int labelGas, random = rand() % 5;
		for (int i = 0; i < nS; i++) {
			if (i == 0 && numBig > 0) {
				for (int j = 1; j <= n; j++) {
					if (R[j] == 10.0) {
						labelGas = j;
						labelPlanet[i] = j;
						break;
					}
				}
				if (rand() % 2 == 1) {
					RS[i] = 4.0;
					rSRevolution[i] = 5.0 * i + 25 + random;
				}
				else {
					RS[i] = 3.0;
					rSRevolution[i] = 4.0 * i + 25 + random;
				}
				S[i].setTRevolutionTSpin(TRevolution[labelPlanet[i]], rSRevolution[i] * 0.75,
					7.5 + rand() % 5);
			}
			else if (i == 1 && numBig == 2) {
				for (int j = 1; j <= n; j++) {
					if (R[j] == 10.0 && j != labelGas) {
						labelPlanet[i] = j;
						break;
					}
				}
				if (rand() % 2 == 1) {
					RS[i] = 4.0;
					rSRevolution[i] = 5.0 * i + 25 + random;
				}
				else {
					RS[i] = 3.0;
					rSRevolution[i] = 4.0 * i + 25 + random;
				}
				S[i].setTRevolutionTSpin(TRevolution[labelPlanet[i]], rSRevolution[i] * 0.75,
					7.5 + rand() % 5);
			}
			else {
				labelPlanet[i] = 1 + int((rand() + i) % n);
				if (R[labelPlanet[i]] == 10.0) {
					if (rand() % 2 == 1) {
						RS[i] = 4.0;
						rSRevolution[i] = 5.0 * i + 25 + random;
					}
					else {
						RS[i] = 2.0;
						rSRevolution[i] = 4.0 * i + 16 + random;
					}
					S[i].setTRevolutionTSpin(TRevolution[labelPlanet[i]], rSRevolution[i] * 2,
						7.5 + rand() % 5);
				}
				else {
					RS[i] = 2.0;
					rSRevolution[i] = 3.0 * i + 16 + random;
					S[i].setTRevolutionTSpin(TRevolution[labelPlanet[i]], rSRevolution[i] * 2,
						7.5 + rand() % 5);
				}
			}
			int texrand = (rand() + i + iGalaxy) % 4;
			texS[i] = 1 + texrand;

			xSRotation[i] = rand() % 3;
			ySRotation[i] = 1 + rand() % 2;
			zSRotation[i] = rand() % 3;

			char texture_name1[100] = "res/";
			char texture_name2[100];
			char texture_name3[] = ".bmp";
			_itoa_s(texS[i], texture_name2, 10);
			strcat_s(texture_name1, 100, texture_name2);
			strcat_s(texture_name1, 100, texture_name3);
			S[i].init(texture_name1, RS[i]);
			
		}
	}
	
	for (int i = 0; i < nS; i++) {
		std::cout << "labelPlanet" << i << ":" << labelPlanet[i] << "\n";
	}
	
}

void Galaxy::setTime(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos) {
	time0 = glfwGetTime() - timeStart;
	for (int i = 0; i <= n; i++) {
		
		P[i].setModel(time0, rRevolution[i],
			xStar, yStar, zStar,
			xRotation, yRotation, zRotation,
			xSpin, ySpin, zSpin);
		if (i == 0) {
			P[i].setStarLighting(viewPos, 1.0, 1.0, 1.0,
				1.0, 1.0, 1.0,
				0.0, 0.0, 0.0);
		}
		else {
			P[i].setStarLighting(viewPos, 0.1, 0.1, 0.1,
				0.8, 0.8, 0.8,
				0.1, 0.1, 0.1);
		}
		P[i].draw(projection, view);
	}
	
	for (int i = 0; i < nS; i++) {

		S[i].setModel1(time0,
			rRevolution[labelPlanet[i]], rSRevolution[i],
			xStar, yStar, zStar, xRotation, yRotation, zRotation,
			xSRotation[i], xSRotation[i], xSRotation[i]);
		S[i].setStarLighting(viewPos, 0.1, 0.1, 0.1,
			0.8, 0.8, 0.8,
			0.1, 0.1, 0.1);
		S[i].draw(projection, view);
	}
	
}

