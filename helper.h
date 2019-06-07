#ifndef HELPER_H_
#define HELPER_H_

#include <SFML/Graphics.hpp>

#include <cmath>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

class segment {
public:
    /**
     * Takes in two coordinates, determines which coordinate has a smaller angle
     * and uses that coordinate as the start of this segment
     */
    segment(sf::Vector2f a, sf::Vector2f b, sf::Vector2f center);
    segment(const segment &s2);
    segment();
    sf::Vector2f start;
    sf::Vector2f end;
    bool operator==(const segment &rhs);
};

/*
 * Helper functions for mathematical and geometrical operations
 */

// Returns if the two line segments intersect
bool intersects(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2);
// Returns the coordinate of intersection of the two line segments
sf::Vector2f intersection(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2);
// Takes the cross product of the two vectors
double cross_product(sf::Vector2f a, sf::Vector2f b);

// Returns determinant of 3 points
double determinant(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c);
// Returns orientation of turn
// < 0 for left turn
// = 0 for colinear
// > 0 for right turn
int turn_direction(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c);
// Returns the distance from point a to point b NOT squared. Used for sorting purposes
double compute_distance_squared(sf::Vector2f a, sf::Vector2f b);

// Computes a ray from the given line segment.
// The ray will be represented as a line segment from the center to the window border
sf::Vector2f compute_ray(sf::Vector2f center, sf::Vector2f ray_endpt);

/* 
 * Helper functions for sorting vectors
 */

// Class to sort coordinates
// This is used primarily to give coordinates a sorting order so that it can be stored in a heap
class vector2f_cmp {
public:
    bool operator()(const sf::Vector2f &v1, const sf::Vector2f &v2);
};

// Class to sort segments by visibility distance
class distance_cmp {
public:
    distance_cmp(sf::Vector2f center, sf::Vector2f ray_endpt);
    bool operator()(const segment &s1, const segment &s2);
private:
    sf::Vector2f center;
    sf::Vector2f ray_endpt;
};

// Class to sort points by radial angle
class radial_cmp {
public:
    radial_cmp(sf::Vector2f center);
    bool operator()(const sf::Vector2f &v1, const sf::Vector2f &v2);
private:
    sf::Vector2f center;
};
// Used by radial_cmp to compute if v1 comes before v2 in radial ordering
// Is accessible by other functions that require it
bool radial_cmp_aux(const sf::Vector2f &v1, const sf::Vector2f &v2, sf::Vector2f center);
// Returns the angle of the line segment (p, q)
double compute_angle(const sf::Vector2f &p, const sf::Vector2f &q);

#endif // HELPER_H_
