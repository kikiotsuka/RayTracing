#include "helper.h"

segment::segment(sf::Vector2f a, sf::Vector2f b, sf::Vector2f center) {
    start = a;
    end = b;
    if (radial_cmp_aux(a, b, center)) {
        std::swap(start, end);
    }
}

segment::segment(const segment &s2) {
    start = s2.start;
    end = s2.end;
}

segment::segment() {
    start = sf::Vector2f(0, 0);
    end = sf::Vector2f(0, 0);
}

bool segment::operator==(const segment &rhs) {
    return start == rhs.start && end == rhs.end;
}

/*
 * Helper functions for mathematical and geometrical operations
 */

bool intersects(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2) {
    int turn1 = turn_direction(p1, p2, q1);
    int turn2 = turn_direction(p1, p2, q2);
    int turn3 = turn_direction(p1, q1, q2);
    int turn4 = turn_direction(p2, q1, q2);

    return turn1 != turn2 && turn3 != turn4;
}

// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
sf::Vector2f intersection(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f q1, sf::Vector2f q2) {
    sf::Vector2f r = p2 - p1;
    sf::Vector2f s = q2 - q1;

    sf::Vector2f d_qp = q1 - p1;
    float cross_rs = cross_product(r, s);

    float t = cross_product(d_qp, s) / cross_rs;

    return p1 + r * t;
}

double cross_product(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.y - a.y * b.x;
}
 
double determinant(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
    /*
     * | a.x a.y 1.0 |
     * | b.x b.y 1.0 |
     * | c.x c.y 1.0 |
     */
    double det = a.x * b.y + a.y * c.x + b.x * c.y - b.y * c.x - a.y * b.x - a.x * c.y;

    // Floating point error
    if (det < 10e-6 && det > -10e-6)
        return 0.0;
    return det;
}

int turn_direction(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
    double turn = determinant(a, b, c);
    
    if (turn < 0) return -1;
    if (turn > 0) return 1;
    return 0;
}

double compute_distance_squared(sf::Vector2f a, sf::Vector2f b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return dx * dx + dy * dy;
}

sf::Vector2f compute_ray(sf::Vector2f center, sf::Vector2f ray_endpt) {
    // Intersect ray and the window bounding box
    double dx = ray_endpt.x - center.x;
    double dy = ray_endpt.y - center.y;
    // Determines which border we're trying to intersect with
    int border_x = dx > 0 ? WINDOW_WIDTH : 0;
    int border_y = dy > 0 ? WINDOW_HEIGHT : 0;
    // Take the smaller of both ratios
    double r1 = (border_x - center.x) / dx;
    double r2 = (border_y - center.y) / dy;
    double ratio = fmin(r1, r2);

    return center + sf::Vector2f(dx * ratio, dy * ratio);
}

/*
 * Helper functions for sorting vectors
 */

bool vector2f_cmp::operator()(const sf::Vector2f &v1, const sf::Vector2f &v2) {
    if (v1.x < v2.x) return true;
    if (v1.x > v2.x) return false;
    return v1.y < v2.y;
}

distance_cmp::distance_cmp(sf::Vector2f center, sf::Vector2f ray_endpt) {
    this->center = center;

    this->ray_endpt = compute_ray(center, ray_endpt);
}

bool distance_cmp::operator()(const segment &s1, const segment &s2) {
    sf::Vector2f s1_int = intersection(center, ray_endpt, s1.start, s1.end);
    sf::Vector2f s2_int = intersection(center, ray_endpt, s2.start, s2.end);

    double s1_dist = compute_distance_squared(center, s1_int);
    double s2_dist = compute_distance_squared(center, s2_int);

    // Loop in a circle once, then reinspect first point
    // For all line segments hit by the initial ray, swap start and end point then proceed with algo(???)
    // TODO iron out algorithm initialization portion, maybe google other references
    // TODO determine if this is necessary for tie break when line segments share endpoints
    /*
    // Tie in distance, break by angle
    if (fabs(s2_dist - s1_dist) < 10e-6) {
        double comparison_angle = compute_angle(center, ray_endpt);
        double angle1 = compute_angle(s1.start, center);
        double angle2 = compute_angle(s2.start, center);

        if (angle1 > comparison_angle && angle2 > comparison_angle) {
        }
    }
    */

    return s1_dist < s2_dist;
}

radial_cmp::radial_cmp(sf::Vector2f center) {
    this->center = center;
}

bool radial_cmp::operator()(const sf::Vector2f &v1, const sf::Vector2f &v2) {
    return radial_cmp_aux(v1, v2, center);
}

bool radial_cmp_aux(const sf::Vector2f &v1, const sf::Vector2f &v2, sf::Vector2f center) {
    double angle1 = compute_angle(v1, center);
    double angle2 = compute_angle(v2, center);

    return angle1 - angle2 > 0.0;
}

double compute_angle(const sf::Vector2f &p, const sf::Vector2f &q) {
    return atan2(q.y - p.y, q.x - p.x);
}
