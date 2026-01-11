#pragma once
#include <SFML/System.hpp>

// Checks if two line segments [p1,p2] and [q1,q2] intersect
inline bool segmentsIntersect(const sf::Vector2f& p1, const sf::Vector2f& p2,
    const sf::Vector2f& q1, const sf::Vector2f& q2)
{
    auto cross = [](const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.y - a.y * b.x;
        };

    sf::Vector2f r = p2 - p1;
    sf::Vector2f s = q2 - q1;
    sf::Vector2f qp = q1 - p1;

    float rxs = cross(r, s);
    float qpxr = cross(qp, r);

    if (rxs == 0.f) {
        if (qpxr == 0.f) {
            // Collinear: check for overlap
            float t0 = (qp.x * r.x + qp.y * r.y) / (r.x * r.x + r.y * r.y);
            float t1 = t0 + (s.x * r.x + s.y * r.y) / (r.x * r.x + r.y * r.y);
            return (t0 >= 0.f && t0 <= 1.f) || (t1 >= 0.f && t1 <= 1.f);
        }
        return false; // Parallel and non-intersecting
    }

    float t = cross(qp, s) / rxs;
    float u = qpxr / rxs;

    return (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f);
}

inline bool segmentIntersectsRect(const sf::Vector2f& p1, const sf::Vector2f& p2, const sf::FloatRect& rect) {
    sf::Vector2f topLeft(rect.left, rect.top);
    sf::Vector2f topRight(rect.left + rect.width, rect.top);
    sf::Vector2f bottomLeft(rect.left, rect.top + rect.height);
    sf::Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    return segmentsIntersect(p1, p2, topLeft, topRight) ||
        segmentsIntersect(p1, p2, topRight, bottomRight) ||
        segmentsIntersect(p1, p2, bottomRight, bottomLeft) ||
        segmentsIntersect(p1, p2, bottomLeft, topLeft);
}