#include "PCH.h"
#include "Collision.h"

const bool Collision::SphereInSphere(Sphere _sphere1, Sphere _sphere2)
{
	return Magnitude(_sphere1.GetCenter() - _sphere2.GetCenter())
		<= (_sphere1.GetRadius() + _sphere2.GetRadius());
}

const bool Collision::CircleInCircle(Circle _circle1, Circle _circle2)
{
	return Magnitude(_circle1.GetCenter() - _circle2.GetCenter())
		<= (_circle1.GetRadius() + _circle2.GetRadius());
}

const bool Collision::MeteorInCircle(Meteor _meteor, Circle _circle)
{
	return Magnitude(_meteor.GetCenter() - _circle.GetCenter())
		<= (_meteor.GetRadius() + _circle.GetRadius());
}

const bool Collision::BlackHoleInCircle(BlackHole _blackHole, Circle _circle)
{
	return Magnitude(_blackHole.GetCenter() - _circle.GetCenter())
		<= (_blackHole.GetRadius() + _circle.GetRadius());
}

const bool Collision::PlanetInCircle(Planet _planet, Circle _circle)
{
	return Magnitude(_planet.GetCenter() - _circle.GetCenter())
		<= (_planet.GetRadius() + _circle.GetRadius());
}


