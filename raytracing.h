#ifndef RAYTRACING_H_
#define RAYTRACING_H_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <functional>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "helper.h"

// Indicates if a line is being drawn or not
bool drawing;
// Store all lines drawn
sf::VertexArray lines;
bool ray_tracing;

// Store triangles for drawing the ray tracing
sf::VertexArray lights;

// Set of unique points for adding to the event queue later
std::set<sf::Vector2f, vector2f_cmp> inspection_points;
// Stores starting endpoints of line segments
std::map<sf::Vector2f, segment, vector2f_cmp> start_segments;
// Stores ending endpoints of line segments
std::map<sf::Vector2f, segment, vector2f_cmp> end_segments;

// Event handler for event loop
void handle_events(sf::RenderWindow &window);
// TODO see if this is necessary
void draw_lines(sf::RenderWindow &window);

// Verifies the last line in the list that is being committed
void verify_line();
// Adds the newly specified line
void add_line(sf::Vector2f coord, bool continue_drawing);
// Run ray tracing algorithm
// coord represents the coordinate the rays are being fired from
void ray_trace(sf::Vector2f &coord);
// Fire single ray at the coordinate specified by event
void fire_ray(std::vector<segment> &ray_intersecting_lines, sf::Vector2f &coord, sf::Vector2f &event);

#endif // RAYTRACING_H_
