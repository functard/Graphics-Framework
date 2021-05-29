#pragma once
#include "Sphere.h"
#include "Circle.h"
#include "Helpers.h"
#include "Meteor.h"
#include "BlackHole.h"
#include "Planet.h"

class Collision
{
public:
    const static bool SphereInSphere(Sphere _sphere1, Sphere _sphere2);
    const static bool CircleInCircle(Circle _circle1, Circle _circle2);
    const static bool MeteorInCircle(Meteor _meteor, Circle _circle);
    const static bool BlackHoleInCircle(BlackHole _blackHole, Circle _circle);
    const static bool PlanetInCircle(Planet _planet, Circle _circle);
};

