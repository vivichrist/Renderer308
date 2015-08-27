/*
 * Camera.h
 *
 *  Created on: 2/02/2015
 *      Author: vivichrist
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <exception>
#include "Plane.hpp"

namespace vogl
{

class Camera
{
public:
	Camera( const glm::vec3&, int, int );
	virtual ~Camera();
	virtual void setRotation( const float, const float, const float );
	void setupProjection( const float, const float, const float near = 0.1f
						, const float far = 1000.0f );
	void setPosition( const glm::vec3& v );
	void setFOV( const float fov );
	// camera
	const glm::vec3 getPosition() const;
	float getFOV() const;
	float getAspectRatio() const;
	const glm::mat4 getViewMatrix() const;
	const glm::mat4 getProjectionMatrix() const;
	const glm::mat3 getNormalMatrix() const;
	void getFrustumPlanes( glm::vec4[6] );
	void calcFrustumPlanes();
	bool isPointInFrustum( const glm::vec3& );
	bool isSphereInFrustum( const glm::vec3&, const float );
	bool isBoxInFrustum( const glm::vec3&, const glm::vec3& );
	// camera movement
	void rotateX( const float );
	void rotateY( const float );
	void rotateZ( const float );
	void rotateAroundX( const float );
	void rotateAroundY( const float );
	void rotateAroundZ( const float );
	virtual void update();
	void walkOn( const bool );
	void strafeOn( const bool );
	void verticalOn( const bool );
	void walkOff( const bool );
	void strafeOff( const bool );
	void verticalOff( const bool );
	void setTranslation( const glm::vec3& );
	glm::vec3 getTranslation() const;
	void setSpeed( const float );
	float getSpeed() const;
private:
	float yaw, pitch, roll, fov, aspect_ratio, z_near, z_far;
	bool rotation_changed, movement_changed, view_changed
		, forward, backward, sleft, sright, lift, descent;
	static glm::vec3 UP;
	static constexpr float PIx2 = M_PI * 2.0f;
	glm::vec3 look;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 position;
	glm::mat4 view;	//view matrix
	glm::mat4 proj;	//projection matrix
	Plane planes[6]; //Frustum planes
	// camera movement variables
	float speed; //movement speed of camera in m/s
	glm::vec3 translation;
};

} // end vogl namespace
#endif /* CAMERA_HPP_ */
