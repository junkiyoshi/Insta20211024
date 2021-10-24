#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}
//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	this->face.clear();
	this->frame.clear();

	for (int x = -840; x <= 840; x += 560) {

		for (int y = -840; y <= 840; y += 560) {

			auto noise_seed = glm::vec2(ofRandom(1000), ofRandom(1000));
			this->setRingToMesh(this->face, this->frame, glm::vec3(x, y, 0), 0, 40, 15, 0, 360, noise_seed);
			for (auto radius = 50; radius < 250; radius += 10) {

				for (int deg = 0; deg < 360; deg += 30) {

					this->setRingToMesh(this->face, this->frame, glm::vec3(x, y, 0), radius, 10, 15, deg, deg + 15, noise_seed);
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	ofSetColor(239);
	this->face.draw();

	ofSetColor(39);
	this->frame.drawWireframe();

	this->cam.end();
}

//--------------------------------------------------------------
void ofApp::setRingToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float radius, float width, float height, int deg_start, int deg_end, glm::vec2 noise_seed) {

	auto index = frame_target.getNumVertices();

	auto noise_value = radius != 0 ? 
		glm::vec2(ofNoise(noise_seed.x - radius * 0.003 + ofGetFrameNum() * 0.0085), ofNoise(noise_seed.y - radius * 0.003 + ofGetFrameNum() * 0.0085)) :
		glm::vec2(ofNoise(noise_seed.x - 40 * 0.003 + ofGetFrameNum() * 0.0085), ofNoise(noise_seed.y - 40 * 0.003 + ofGetFrameNum() * 0.0085));

	auto next_noise_value = radius != 0 ?
		glm::vec2(ofNoise(noise_seed.x - (radius + width) * 0.003 + ofGetFrameNum() * 0.0085), ofNoise(noise_seed.y - (radius + width) * 0.003 + ofGetFrameNum() * 0.0085)) :
		glm::vec2(ofNoise(noise_seed.x - 40 * 0.003 + ofGetFrameNum() * 0.0085), ofNoise(noise_seed.y - 40 * 0.003 + ofGetFrameNum() * 0.0085));
	
	auto angle_x = ofMap(noise_value.x, 0, 1, -PI * 2, PI * 2);
	auto angle_y = ofMap(noise_value.y, 0, 1, -PI * 2, PI * 2);

	auto next_angle_x = ofMap(next_noise_value.x, 0, 1, -PI * 2, PI * 2);
	auto next_angle_y = ofMap(next_noise_value.y, 0, 1, -PI * 2, PI * 2);

	auto rotation_x = glm::rotate(glm::mat4(), angle_x, glm::vec3(1, 0, 0));
	auto rotation_y = glm::rotate(glm::mat4(), angle_y, glm::vec3(0, 1, 0));

	auto next_rotation_x = glm::rotate(glm::mat4(), next_angle_x, glm::vec3(1, 0, 0));
	auto next_rotation_y = glm::rotate(glm::mat4(), next_angle_y, glm::vec3(0, 1, 0));

	int deg_span = 3;
	for (int deg = deg_start; deg < deg_end; deg += deg_span) {

		auto face_index = face_target.getNumVertices();

		vector<glm::vec3> vertices;
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), height * -0.5));
		vertices.push_back(glm::vec3(radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD), height * -0.5));
		vertices.push_back(glm::vec3(radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD), height * 0.5));
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), height * 0.5));

		vertices.push_back(glm::vec3((radius + width) * cos(deg * DEG_TO_RAD), (radius + width) * sin(deg * DEG_TO_RAD), height * -0.5));
		vertices.push_back(glm::vec3((radius + width) * cos((deg + deg_span) * DEG_TO_RAD), (radius + width) * sin((deg + deg_span) * DEG_TO_RAD), height * -0.5));
		vertices.push_back(glm::vec3((radius + width) * cos((deg + deg_span) * DEG_TO_RAD), (radius + width) * sin((deg + deg_span) * DEG_TO_RAD), height * 0.5));
		vertices.push_back(glm::vec3((radius + width) * cos(deg * DEG_TO_RAD), (radius + width) * sin(deg * DEG_TO_RAD), height * 0.5));

		for (int i = 0; i < 4; i++) {

			vertices[i] = location + glm::vec4(vertices[i], 0) * rotation_y * rotation_x;
		}

		for (int i = 4; i < 8; i++) {

			vertices[i] = location + glm::vec4(vertices[i], 0) * next_rotation_y * next_rotation_x;
		}

		face_target.addVertices(vertices);

		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2);
		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 3);

		face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5); face_target.addIndex(face_index + 6);
		face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 6); face_target.addIndex(face_index + 7);

		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);
		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 5); face_target.addIndex(face_index + 1);

		face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 7); face_target.addIndex(face_index + 6);
		face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 6); face_target.addIndex(face_index + 2);

		auto frame_index = frame_target.getNumVertices();

		frame_target.addVertices(vertices);

		frame_target.addIndex(frame_index + 0); frame_target.addIndex(frame_index + 1);
		frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 3);
		frame_target.addIndex(frame_index + 4); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 6); frame_target.addIndex(frame_index + 7);
	}

	if (radius != 0) {

		face_target.addIndex(index + 0); face_target.addIndex(index + 3); face_target.addIndex(index + 7);
		face_target.addIndex(index + 0); face_target.addIndex(index + 7); face_target.addIndex(index + 4);

		frame_target.addIndex(index + 0); frame_target.addIndex(index + 3);
		frame_target.addIndex(index + 0); frame_target.addIndex(index + 4);
		frame_target.addIndex(index + 7); frame_target.addIndex(index + 3);
		frame_target.addIndex(index + 7); frame_target.addIndex(index + 4);

		index = frame_target.getNumVertices() - 8;

		face_target.addIndex(index + 1); face_target.addIndex(index + 2); face_target.addIndex(index + 6);
		face_target.addIndex(index + 1); face_target.addIndex(index + 6); face_target.addIndex(index + 5);

		frame_target.addIndex(index + 1); frame_target.addIndex(index + 2);
		frame_target.addIndex(index + 1); frame_target.addIndex(index + 5);
		frame_target.addIndex(index + 6); frame_target.addIndex(index + 2);
		frame_target.addIndex(index + 6); frame_target.addIndex(index + 5);
	}

}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}