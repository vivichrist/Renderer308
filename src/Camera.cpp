/**@class Camera
 * @file Camera.cpp
 *
 *  Created on: 2/02/2015
 *      Author: vivichrist
 */

#include "Camera.hpp"
#include <iostream>

namespace vogl
{

/**
 * Constructs a camera that faces the usual default direction of -Z.
 * @param feild of view (fov), @param yaw, @param pitch and @param roll
 * are all currently in degrees.
 */
Camera::Camera( const glm::vec3 &initpos, int width, int height )
: yaw(0), pitch(0), roll(0), fov(M_PI/4.0f), z_near(1.0f), z_far(1000.0f)
, rotation_changed(true), movement_changed(true), view_changed(true)
, forward(false), backward(false), sleft(false), sright(false), lift(false)
, descent(false), speed(0.05)
{
	aspect_ratio = (float) width / (float) height;
	look = glm::vec3( 0.0f, 0.0f, -1.0f );
	up = glm::vec3( 0.0f, 1.0f, 0.0f );
	right = glm::vec3( 1.0f, 0.0f, 0.0f );
	position = initpos;
	// view matrix
	view = glm::lookAt( position, position + look, up );
	// projection matrix
	proj = glm::perspective( fov, aspect_ratio, z_near, z_far );
	calcFrustumPlanes();
}
Camera::~Camera() {}

/**
 * Reassign all the parameters of the camera frustum and projection.
 * And recalculate the projection matrix and the fustrum containment planes.
 */
void Camera::setupProjection( const float fovy, const float aspectRatio
		, const float near, const float far )
{
	fov = fovy; z_near = near; z_far = far;
	aspect_ratio = aspectRatio;
	proj = glm::perspective( fov, aspect_ratio, z_near, z_far );
	view_changed = true;
}

/**
 * move camera
 */
void Camera::setPosition( const glm::vec3& newPosition )
{
	position = newPosition;
	movement_changed = true;
}

void Camera::setFOV( const float nfov )
{
	if ( nfov < 0.0f or nfov > M_PI )
	{
		std::cout << "fov Should be in Radians";
		throw; // error!
	}
	fov = nfov;
	proj = glm::perspective( fov, aspect_ratio, z_near, z_far );
	view_changed = true;
}
///////////////////////////////////////////////////////////////////////////////
//                             Camera Rotation                               //
///////////////////////////////////////////////////////////////////////////////
/**
 * Re-orient the camera
 */
void Camera::setRotation( const float yaw, const float pitch, const float roll )
{
	if (yaw) this->yaw = yaw;
	if (pitch) this->pitch = pitch;
	if (roll) this->roll = roll;
	rotation_changed = true;
}

void Camera::rotateX( const float pitch )
{
	if ( pitch )
	{
		this->pitch = pitch * speed;
		if ( this->pitch > M_PI_2 ) this->pitch = M_PI_2;
		else if ( this->pitch < -M_PI_2 ) this->pitch = -M_PI_2;
		glm::mat4 R = glm::rotate( this->pitch, right );
		look = glm::vec3( R * glm::vec4( look, 0 ) );
		view_changed = true;
	}
}

void Camera::rotateY( const float yaw )
{
	if ( yaw )
	{
		this->yaw = yaw * speed;
		if ( this->yaw > PIx2 ) this->yaw -= PIx2;
		else if ( this->yaw < 0 ) this->yaw += PIx2;
		glm::mat4 R = glm::rotate( this->yaw, up );
		look = glm::vec3( R * glm::vec4( look, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateZ( const float roll )
{
	if ( roll )
	{
		this->roll = roll * speed;
		glm::mat4 R = glm::rotate( this->roll, look );
		up = glm::vec3( R * glm::vec4( up, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateAroundX( const float x )
{
	if ( x && -M_PI_2 < x && x < M_PI_2 )
	{
		glm::mat4 R = glm::yawPitchRoll( 0.0f, x, 0.0f );
		position = glm::vec3( R * glm::vec4( position, 0 ) );
		look = glm::vec3( R * glm::vec4( look, 0 ) );
		up = glm::vec3( R * glm::vec4( up, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateAroundY( const float y )
{
	if ( y && -M_PI_2 < y && y < M_PI_2 )
	{
		glm::mat4 R = glm::yawPitchRoll( y, 0.0f, 0.0f );
		position = glm::vec3( R * glm::vec4( position, 0 ) );
		look = glm::vec3( R * glm::vec4( look, 0 ) );
		up = glm::vec3( R * glm::vec4( up, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateAroundZ( const float z )
{
	if ( z && -M_PI_2 < z && z < M_PI_2 )
	{
		glm::mat4 R = glm::yawPitchRoll( 0.0f, 0.0f, z );
		position = glm::vec3( R * glm::vec4( position, 0 ) );
		look = glm::vec3( R * glm::vec4( look, 0 ) );
		up = glm::vec3( R * glm::vec4( up, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
//                                 Matrices                                  //
///////////////////////////////////////////////////////////////////////////////
/**
 * @return a matrix to transform vertices from worldspace to eyespace. The
 * Model translations are handled elsewhere.
 */
const glm::mat4 Camera::getViewMatrix() const
{
	return( view );
}

/**
 * @return a matrix to transform vertices from eyespace to clipspace
 */
const glm::mat4 Camera::getProjectionMatrix() const
{
	return( proj );
}

/**
 * @return a matrix to transform normal vectors from worldspace to eyespace
 * while conserving their vector qualities.
 */
const glm::mat3 Camera::getNormalMatrix() const
{
	return( glm::inverse( glm::transpose( glm::mat3(view) ) ) );
}

const glm::vec3 Camera::getPosition() const
{
	return( position );
}

float Camera::getFOV() const
{
	return( fov );
}

float Camera::getAspectRatio() const
{
	return( aspect_ratio );
}
///////////////////////////////////////////////////////////////////////////////
//                          Frustum Intersection                             //
///////////////////////////////////////////////////////////////////////////////
/**
 * Note that the frustum planes are oriented in such a way that their normals
 * point inside the viewing frustum.
 */
void Camera::calcFrustumPlanes()
{
	// center points of near and far planes
	glm::vec3 centerNear = position + look * z_near;
	glm::vec3 centerFar = position + look * z_far;
	//frustum points
	glm::vec3 farPts[4];
	glm::vec3 nearPts[4];

	float heightOfNearPlane =
			2.0f * glm::tan( glm::radians( fov / 2.0f ) ) * z_near;
	float heightOfFarPlane =
			2.0f * glm::tan( glm::radians( fov / 2.0f ) ) * z_far;
	float widthOfNearPlane = heightOfNearPlane * aspect_ratio;
	float widthOfFarPlane  = heightOfFarPlane  * aspect_ratio;
	float hHnear = heightOfNearPlane / 2.0f;
	float hWnear = widthOfNearPlane / 2.0f;
	float hHfar = heightOfFarPlane / 2.0f;
	float hWfar = widthOfFarPlane / 2.0f;

	farPts[0] = centerFar + up * hHfar - right * hWfar;
	farPts[1] = centerFar - up * hHfar - right * hWfar;
	farPts[2] = centerFar - up * hHfar + right * hWfar;
	farPts[3] = centerFar + up * hHfar + right * hWfar;

	nearPts[0] = centerNear + up * hHnear - right * hWnear;
	nearPts[1] = centerNear - up * hHnear - right * hWnear;
	nearPts[2] = centerNear - up * hHnear + right * hWnear;
	nearPts[3] = centerNear + up * hHnear + right * hWnear;

	planes[0] = Plane::fromPoints( nearPts[3], nearPts[0], farPts[0] );
	planes[1] = Plane::fromPoints( nearPts[1], nearPts[2], farPts[2] );
	planes[2] = Plane::fromPoints( nearPts[0], nearPts[1], farPts[1] );
	planes[3] = Plane::fromPoints( nearPts[2], nearPts[3], farPts[2] );
	planes[4] = Plane::fromPoints( nearPts[0], nearPts[3], nearPts[2] );
	planes[5] = Plane::fromPoints(  farPts[3],  farPts[0], farPts[1] );
}

bool Camera::isPointInFrustum( const glm::vec3& point )
{
	for ( Plane p: planes )
	{
		if ( p.distance( point ) < 0 )
			return( false );
	}
	return( true );
}

bool Camera::isSphereInFrustum( const glm::vec3& center, const float radius )
{
	for( Plane p: planes )
	{
		float d = p.distance( center );
		if ( d < -radius)
			return( false );
	}
	return( true );
}

bool Camera::isBoxInFrustum( const glm::vec3& min, const glm::vec3& max )
{
	glm::vec3 p, n, norm;
	for( Plane pl: planes )
	{
		p=min; n=max;
		norm = pl.normal();
		if(norm.x>=0) {
			p.x = max.x;
			n.x = min.x;
		}
		if(norm.y>=0) {
			p.y = max.y;
			n.y = min.y;
		}
		if(norm.z>=0) {
			p.z = max.z;
			n.z = min.z;
		}
		if ( pl.distance(p) < 0 ) {
			return( false );
		}
	}
	return( true );
}

void Camera::getFrustumPlanes( glm::vec4 fp[6] )
{
	calcFrustumPlanes();
	for( int i=0;i<6;i++ )
		fp[i] = planes[i].plane();
}
///////////////////////////////////////////////////////////////////////////////
//                              Camera Movement                              //
///////////////////////////////////////////////////////////////////////////////

void Camera::update()
{
	if ( movement_changed )
	{
		position += translation;
		translation = glm::vec3( 0 ); // reset translation
		movement_changed = false;
	}
	if ( rotation_changed )
	{
		glm::mat4 R = glm::yawPitchRoll( yaw, pitch, roll );

		look = glm::normalize( glm::vec3( R * glm::vec4( look, 0 ) ) );
		// up = glm::vec3( R * glm::vec4( up, 0 ) );
		right = glm::normalize( glm::cross( look, up ) );
		// reset rotation values
		cout << "rotation_changed";
		rotation_changed = false;
	}
	if ( forward )
		walkOn( true );
	if ( backward )
		walkOn( false );
	if ( sleft )
		strafeOn( false );
	if ( sright )
		strafeOn( true );
	if ( lift )
		verticalOn( true );
	if ( descent )
		verticalOn( false );
	view = glm::lookAt( position, position + look, up );
	view_changed = false;
	calcFrustumPlanes();
}

void Camera::walkOn( const bool north )
{
	translation += north ? look * speed : look * -speed;
	if (north) forward = true;
	else backward = true;
	movement_changed = true;
}

void Camera::walkOff( const bool north )
{
	if (north) forward = false;
	else backward = false;
	movement_changed = false;
}

void Camera::strafeOn( const bool west )
{

	translation += west ? right * speed : right * -speed;
	if (west) sright = true;
	else sleft = true;
	movement_changed = true;
}

void Camera::strafeOff( const bool west )
{
	if (west) sright = false;
	else sleft = false;
	movement_changed = false;
}

void Camera::verticalOn( const bool ascent )
{
	translation += ascent ? up * speed : up * -speed;
	if (ascent) lift = true;
	else descent = true;
	movement_changed = true;
}

void Camera::verticalOff( const bool ascent )
{
	if (ascent) lift = false;
	else descent = false;
	movement_changed = false;
}

glm::vec3 Camera::getTranslation() const
{
	return( translation );
}

float Camera::getSpeed() const
{
	return( speed );
}
void Camera::setTranslation( const glm::vec3& t )
{
	translation += t;
	if (t == glm::vec3() ) return;
	movement_changed = true;
}

void Camera::setSpeed( const float speed )
{
	this->speed = speed;
}

} // end vogl space
