#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <cstdio>
#include <map>
#include<iostream>


bool show_test_window = false;
float reset ;
const int numCols = 30;
const int numRows = 30;


float data[numCols * numRows * 3];

float radius;

float restDist;

float totaldt;

float A;
float frequency;

float A1;
float frequency1;

float altitude = 5;

bool coll = false;

int collJ, collI;
glm::vec3 velocity;
glm::vec3 gravity = glm::vec3(0, -9.8, 0);
glm::vec3 forces = glm::vec3(0, -9.8, 0);
glm::vec3 fBuoyancy;
float waterDensity;
float submergedV;
glm::vec3 Y = glm::vec3(0.f, 1.f, 0.f);

int buoyancyPoint1;
int buoyancyPoint2;
int buoyancyPoint3;
int buoyancyPoint4;

float dragCoef = 0.47f;

float sphereMass = 1;
glm::vec3 k = glm::vec3(2.f, 0.f, 0.f);
glm::vec3 l = glm::vec3(1.f, 0.f, 1.f);
glm::vec3 vector;
float grad;
struct Particle
{
	glm::vec3 initialPos;
	glm::vec3 pos;

};

Particle* totalParts;
std::map<float, float> dist;

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}

namespace ClothMesh {
	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		/*ImGui::SliderInt("KE", &ke,0,1000);
		ImGui::SliderInt("KD", &kd, 0, 10);
		ImGui::SliderFloat("Initial Rest Distance", &llargada, 0.3f,0.5);*/
		ImGui::SliderFloat("Wave Amplitude 1 ", &A, 0.0f, 0.9f);
		ImGui::SliderFloat("Wave Frequency 1", &frequency, 0.0f, 5.f);

		ImGui::SliderFloat("Wave Amplitude 2", &A1, 0.0f, 0.9f);
		ImGui::SliderFloat("Wave Frequency 2", &frequency1, 0.0f, 5.f);

		ImGui::SliderFloat("Altitude", &altitude, 1.f, 9.f);

		ImGui::SliderFloat("Sphere Mass", &sphereMass, 1.f, 10.f);
		ImGui::SliderFloat("DRAG", &grad, 0.1f, 2.953097094);
		
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}
void dragForce() {
	
}
void calcBuoyancy(glm::vec3 vec, glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3) {

	//vector.y = totalParts[i*numCols + j].pos.y +radius;
	float mitja = (vec.y + vec1.y + vec2.y + vec3.y)/4;
	float distancia = abs((vector.y + radius) - mitja);
	if (distancia > 2 * radius) {
		distancia = 2 * radius;
	}
	submergedV = distancia * distancia * distancia;
	
	fBuoyancy = waterDensity*9.81f*submergedV*Y;
	forces += fBuoyancy;
	
	glm::vec3 fDrag = (-0.5f)*waterDensity*dragCoef*grad* glm::length(velocity)*velocity;
	forces += fDrag;
}
void PhysicsInit() {
	//TODO
	//ke = 50;//duresa de la molla
	//kd = 1;//restriccio del moviment
	reset = 20;
	//sphereMass = 1;
	waterDensity = 5;
	A = 0.3f;
	frequency = 2.5f;
	//altitude = 2.f;
	restDist = 10.f / 29.f;
	A1 = 0.2f;
	frequency1 = 1.f;
	coll = false;
	forces = glm::vec3(0, -9.8, 0);
	buoyancyPoint1 = buoyancyPoint2 = buoyancyPoint3 = buoyancyPoint4 = 0;
	velocity = glm::vec3(0);
	dist.clear();
	
	//printf("%f", radius);
	

	
	totalParts = new Particle[numCols * numRows];
	
	for (int i = 0; i < (numRows); ++i) {
		
		for (int j = 0; j < (numCols); ++j) {
			

			totalParts[i*numCols + j].pos = glm::vec3(j*restDist - 5, altitude, i*restDist - 5 );
			totalParts[i*numCols + j].initialPos = totalParts[i*numCols + j].pos;
			data[3 * (i*numCols + j) + 0] = totalParts[i*numCols + j].initialPos.x;
			data[3 * (i*numCols + j) + 1] = totalParts[i*numCols + j].initialPos.y;
			data[3 * (i*numCols + j) + 2] = totalParts[i*numCols + j].initialPos.z;
			
			
		}
	}
	ClothMesh::updateClothMesh(data);
	radius = 1.0f;
	vector = glm::vec3(((float)rand() / RAND_MAX) * 5 - 5, (((float)rand() / RAND_MAX) * 8)+2.5f, ((float)rand() / RAND_MAX) * 5 - 5);
	if (vector.y >= 10 - radius) {
		vector.y -= radius;
	}
	else if (vector.y <= 0 + radius) {
		vector.y += radius;
	}
	if (vector.x >= 5 - radius) {
		vector.x -= radius;
	}
	else if (vector.x <= -5 + radius) {
		vector.x += radius;
	}
	if (vector.z >= 5 - radius) {
		vector.z -= radius;
	}
	else if (vector.z <= -5 + radius) {
		vector.z += radius;
	}

	Sphere::setupSphere(vector, radius);

	for (int i = 0; i < (numRows); i++) {
		//printf("%d", i);
		for (int j = 0; j < (numCols); j++) {
			glm::vec3 punt = glm::vec3(vector.x, totalParts[i*numCols + j].initialPos.y, vector.z);
			float pointdist = glm::distance(punt, totalParts[i*numCols + j].initialPos);
			dist[pointdist] = i*numCols + j;
		}
	}

	for (auto it = dist.begin(); it!=dist.end();it++)
	{
		//std::cout << it->first;
		buoyancyPoint1 = it->second;
		it++;
		//std::cout << it->first;
		buoyancyPoint2 = it->second;
		it++;
		//std::cout << it->first;
		buoyancyPoint3 = it->second;
		it++;
		//std::cout << it->first;
		buoyancyPoint4 = it->second;
		break;
	}

}

void PhysicsUpdate(float dt) {
	forces = gravity *sphereMass;
	reset -= dt;
	totaldt += dt;
	if (reset <= 0) {
		PhysicsInit();
	}
	
	
	//TODO
	for (int i = 0; i < (numRows); i++) {
		//printf("%d", i);
		for (int j = 0; j < (numCols); j++) {
			//ona1
			glm::vec3 pos = (k / glm::length(k)) *A *sin(glm::dot(k, totalParts[i*numCols + j].initialPos) - frequency *totaldt);
			float alt = A *cos(glm::dot(k, totalParts[i*numCols + j].initialPos) - frequency *totaldt);
			//ona2
			pos += (l / glm::length(l)) *A1 *sin(glm::dot(l, totalParts[i*numCols + j].initialPos) - frequency1 *totaldt);
			alt += A1 *cos(glm::dot(l, totalParts[i*numCols + j].initialPos) - frequency1 *totaldt);
			//calculTotal
			totalParts[i*numCols + j].pos = totalParts[i*numCols + j].initialPos - pos;
			totalParts[i*numCols + j].pos.y = alt + altitude;

			

			data[3 * (i*numCols + j) + 0] = totalParts[i*numCols + j].pos.x;
			data[3 * (i*numCols + j) + 1] = totalParts[i*numCols + j].pos.y;
			data[3 * (i*numCols + j) + 2] = totalParts[i*numCols + j].pos.z;

			
		}
	}

	if (glm::distance(totalParts[buoyancyPoint1].pos, vector) <= radius && glm::distance(totalParts[buoyancyPoint2].pos, vector) <= radius && glm::distance(totalParts[buoyancyPoint3].pos, vector) <= radius && glm::distance(totalParts[buoyancyPoint4].pos, vector) <= radius || vector.y + radius <= totalParts[buoyancyPoint1].pos.y) {
		//printf("collisin");
		calcBuoyancy(totalParts[buoyancyPoint1].pos, totalParts[buoyancyPoint2].pos, totalParts[buoyancyPoint3].pos, totalParts[buoyancyPoint4].pos);

	}
	vector += dt * velocity;
	velocity += dt* forces / sphereMass;
	Sphere::updateSphere(vector, radius);
	ClothMesh::updateClothMesh(data);



	}

void PhysicsCleanup() {
	//TODO
	ClothMesh::cleanupClothMesh();
	delete[] totalParts;
}