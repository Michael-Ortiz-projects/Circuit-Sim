#pragma once
#include <SFML/System.hpp>



inline float cross(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.y - a.y * b.x;
}

inline sf::Vector2f subtract(const sf::Vector2f& a, const sf::Vector2f& b) {
    return { a.x - b.x, a.y - b.y };
}

inline bool onSegment(const sf::Vector2f& p,
    const sf::Vector2f& q,
    const sf::Vector2f& r) {
    return q.x >= std::min(p.x, r.x) && q.x <= std::max(p.x, r.x) &&
        q.y >= std::min(p.y, r.y) && q.y <= std::max(p.y, r.y);
}

// Checks if two line segments [p1,p2] and [q1,q2] intersect
inline bool segmentsIntersect(const sf::Vector2f& p1,
    const sf::Vector2f& p2,
    const sf::Vector2f& q1,
    const sf::Vector2f& q2)
{
    sf::Vector2f r = p2 - p1;
    sf::Vector2f s = q2 - q1;

    float rxs = cross(r, s);
    float q_pxr = cross(subtract(q1, p1), r);

    // Collinear
    if (rxs == 0.f && q_pxr == 0.f) {
        return onSegment(p1, q1, p2) ||
            onSegment(p1, q2, p2) ||
            onSegment(q1, p1, q2) ||
            onSegment(q1, p2, q2);
    }

    // Parallel, non-intersecting
    if (rxs == 0.f && q_pxr != 0.f)
        return false;

    float t = cross(q1 - p1, s) / rxs;
    float u = cross(q1 - p1, r) / rxs;

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