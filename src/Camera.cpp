/**@class Camera
 * @file Camera.cpp
 *
 *  Created on: 2/02/2015
 *      Author: vivichrist
 */

#include "Camera.hpp"

namespace R308
{
const float TWOPI = M_PI * 2.0f;
const glm::vec3 Camera::UP( 0.0f, 1.0f, 0.0f );

/**
 * Constructs a camera that faces the usual default direction of -Z.
 * @param feild of view (fov), @param yaw, @param pitch and @param roll
 * are all currently in degrees.
 */
Camera::Camera( const glm::vec3& initpos, int width, int height )
	: yaw(0), pitch(0), roll(0), fov(M_PI/4.0f), z_near(1.0f), z_far(1000.0f)
	, rotation_changed(true), movement_changed(true), view_changed(true)
	, forward(false), backward(false), sleft(false), sright(false), lift(false)
	, descent(false), ublock(0), speed(0.05)
{
	aspect_ratio = (float) width / (float) height;
	look = glm::vec3( 0.0f, 0.0f, -1.0f );
	up = glm::vec3( 0.0f, 1.0f, 0.0f );
	right = glm::vec3( 1.0f, 0.0f, 0.0f );
	position = initpos;
	// view matrix
	view = glm::lookAt( position, position + look, up );
	norm = glm::inverse(glm::transpose(glm::mat3(view)));
	// projection matrix
	proj = glm::perspective( fov, aspect_ratio, z_near, z_far );
	calcFrustumPlanes();
}
Camera::~Camera() {
}

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

void Camera::setAspectRatio( const float width, const float height )
{
	aspect_ratio = (float) width / (float) height;
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

void Camera::setPosLookCenter( const glm::vec3& newPosition )
{
	position = newPosition;
	look = glm::normalize( glm::vec3() - newPosition );
	right = glm::normalize( glm::cross( look, this->UP ) );
	up = UP;
	movement_changed = true;
}

void Camera::setLookCenter()
{
	look = glm::normalize( glm::vec3() - position );
	right = glm::normalize( glm::cross( look, UP ) );
	up = UP;
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

void Camera::rotateOrigX( const float x )
{
	if ( x )
	{
		position = glm::rotate( position, x, right );
		look = glm::normalize( glm::vec3() - position );
		up = glm::rotate( up, x, right );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateOrigY( const float y )
{
	if ( y )
	{
		position = glm::rotate( position, y, UP );
		look = glm::normalize( glm::vec3() - position );
		up = glm::rotate( up, y, UP );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
}

void Camera::rotateOrigZ( const float z )
{
	if ( z )
	{
		glm::vec3 v( look.x, 0, look.z );
		position = glm::rotate( position, z, v );
		look = glm::normalize( glm::vec3() - position );
		right = glm::normalize( glm::cross( look, up ) );
		view_changed = true;
	}
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
		if ( this->yaw > TWOPI ) this->yaw -= TWOPI;
		else if ( this->yaw < 0 ) this->yaw += TWOPI;
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
	return( norm );
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
	for ( Plane p : planes )
	{
		if ( p.distance( point ) < 0 )
			return( false );
	}
	return( true );
}

bool Camera::isSphereInFrustum( const glm::vec3& center, const float radius )
{
	for( Plane p : planes )
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
	for( Plane pl : planes )
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
	for( int i=0; i<6; i++ )
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
		std::cout << "rotation_changed";
		rotation_changed = false;
	}
	if ( forward )
	{
		walkOn( true );
		view_changed = true;
	}
	if ( backward )
	{
		walkOn( false );
		view_changed = true;
	}
	if ( sleft )
	{
		strafeOn( false );
		view_changed = true;
	}
	if ( sright )
	{
		strafeOn( true );
		view_changed = true;
	}
	if ( lift )
	{
		verticalOn( true );
		view_changed = true;
	}
	if ( descent )
	{
		verticalOn( false );
		view_changed = true;
	}
	if (ublock && view_changed )
	{
		view = glm::lookAt( position, position + look, up );
		norm = glm::inverse(glm::transpose(glm::mat3(view)));
		float *mvM = glm::value_ptr( view );
		uint offset = 0u;
		ublock->setUniformData<float>( 4u, 4u, offset, mvM );
		glm::mat4 pvM = proj * view;
		float *pjM = glm::value_ptr( pvM );
		offset = 4u;
		ublock->setUniformData<float>( 4u, 4u, offset, pjM );
		float *nM = glm::value_ptr( norm );
		offset = 8u;
		ublock->setUniformData<float>( 3u, 3u, offset, nM );
		//ends at 10 so next avaliable slot is 11 (vec4 alignment)
		calcFrustumPlanes();
	}
	view_changed = false;
}

void Camera::registerUBO( UniformBlock* ubo )
{
	ublock = ubo;
}

void Camera::zoomIn()
{
	translation += look * 0.5f;
	movement_changed = true;
}

void Camera::zoomOut()
{
	translation += look * -0.5f;
	movement_changed = true;
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

} // end R308 space
