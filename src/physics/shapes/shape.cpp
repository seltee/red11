// SPDX-FileCopyrightText: 2024 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "shape.h"

Shape::Shape()
{
    mass = 1.0f;
}

Shape::~Shape()
{
}

std::string Shape::getTypeName(ShapeCollisionType type)
{
    switch (type)
    {
    case ShapeCollisionType::None:
        return "None";
    case ShapeCollisionType::Sphere:
        return "Sphere";
    case ShapeCollisionType::OBB:
        return "OBB";
    case ShapeCollisionType::Convex:
        return "Convex";
    case ShapeCollisionType::Plain:
        return "Plain";
    case ShapeCollisionType::Mesh:
        return "Mesh";
    case ShapeCollisionType::Capsule:
        return "Capsule";
    case ShapeCollisionType::Combined:
        return "Combined";
    case ShapeCollisionType::Amount:
        return "Amount???";
    default:
        return "Unknown";
    }
}
