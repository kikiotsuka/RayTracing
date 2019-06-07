#include "raytracing.h"

int main(int argc, char** argv) {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray tracing example");

    drawing = false;
    ray_tracing = false;

    lines = sf::VertexArray(sf::Lines);
    // Add window border lines
    // Top border
    lines.append(sf::Vertex(sf::Vector2f(0, 0)));
    lines.append(sf::Vertex(sf::Vector2f(WINDOW_WIDTH, 0)));
    // Bottom border
    lines.append(sf::Vertex(sf::Vector2f(0, WINDOW_HEIGHT)));
    lines.append(sf::Vertex(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)));
    // Left border
    lines.append(sf::Vertex(sf::Vector2f(0, 0)));
    lines.append(sf::Vertex(sf::Vector2f(0, WINDOW_HEIGHT)));
    // Right border
    lines.append(sf::Vertex(sf::Vector2f(WINDOW_WIDTH, 0)));
    lines.append(sf::Vertex(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)));

    lights = sf::VertexArray(sf::Triangles);

    while (window.isOpen()) {
        // Handle events
        handle_events(window);

        // Draw on screen
        window.clear(sf::Color::White);
        draw_lines(window);
        window.display();
    }

    return 0;
}

void handle_events(sf::RenderWindow &window) {
    sf::Event event;
    sf::Vector2f coord = sf::Vector2f(sf::Mouse::getPosition(window));
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (!ray_tracing) {
                std::cout << "Mouse click: " << coord.x << " " << coord.y << std::endl;
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (drawing) {
                        std::cout << "Committing last line drawn" << std::endl;
                        drawing = false;
                        verify_line();
                    } else {
                        std::cout << "Starting a new line" << std::endl;
                        drawing = true;
                        add_line(coord, false);
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    // Continue drawing a new line from the clicked position
                    if (drawing) {
                        add_line(coord, true);
                    }
                }
            } else {
                ray_trace(coord);
            }
        } else if (event.type == sf::Event::MouseMoved) {
            if (!ray_tracing) {
                // Update the line being currently drawn
                if (drawing) {
                    lines[lines.getVertexCount() - 1].position = coord;
                }
            } else {
                // TODO check if this is too computational heavy and slow
                //ray_trace(coord);
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                if (drawing) {
                    drawing = false;
                    lines.resize(lines.getVertexCount() - 2);
                } else {
                    window.close();
                }
            } else if (event.key.code == sf::Keyboard::R) {
                std::cout << "Flipping ray tracing mode: " << (!ray_tracing ? "True" : "False") << std::endl;
                ray_tracing = !ray_tracing;
                if (drawing) {
                    lines.resize(lines.getVertexCount() - 2);
                }

                // Initialize ray tracing
                if (ray_tracing) {
                    inspection_points.clear();
                    for (int i = 0; i < lines.getVertexCount(); ++i) {
                        inspection_points.insert(lines[i].position);
                    }

                    ray_trace(coord);
                } else {
                    lights.clear();
                }
            }
        }
    }
}

void verify_line() {
    int num_lines = lines.getVertexCount();

    if (num_lines > 2) {
        // Delete the last two points if it's invalid
        if (lines[num_lines - 1].position.x == lines[num_lines - 2].position.x &&
            lines[num_lines - 1].position.y == lines[num_lines - 2].position.y) {
            std::cout << "Last line was a point, removing from list" << std::endl;
            lines.resize(lines.getVertexCount() - 2);
        }
    }
}

void add_line(sf::Vector2f coord, bool continue_drawing) {
    // TODO add logic to snap to nearest point if it's closer than k pixels(?)
    // TODO add logic to prevent line intersection(?)
    verify_line();

    int num_lines = lines.getVertexCount();

    // Start a new line
    if (continue_drawing) {
        lines.append(sf::Vertex(lines[num_lines - 1].position, sf::Color::Black));
    } else {
        lines.append(sf::Vertex(coord, sf::Color::Black));
    }
    lines.append(sf::Vertex(coord, sf::Color::Black));
}

void ray_trace(sf::Vector2f &coord) {
    // Order coordinates in a priority queue
    typedef std::priority_queue<sf::Vector2f, std::vector<sf::Vector2f>, radial_cmp> radial_pq;
    radial_pq events(radial_cmp{coord});
    std::vector<segment> ray_intersecting_lines;

    // Add all unique points to inspect as an event
    // All points are sorted radially clockwise order
    for (auto const &p : inspection_points) {
        events.push(p);
    }

    // The starting angle to fire the ray
    sf::Vector2f initial_angle = events.top();
    sf::Vector2f horizontal_ray(WINDOW_WIDTH, coord.y);

    // Add all line segments and classify their start/end points
    // If the line segment intersects the horizontal ray, swap the start and end point
    start_segments.clear();
    end_segments.clear();
    for (int i = 0; i < lines.getVertexCount(); i += 2) {
        sf::Vector2f a = lines[i].position;
        sf::Vector2f b = lines[i + 1].position;

        segment s(a, b, coord);

        // Check if current segment is hit by initializing ray
        if (intersects(s.start, s.end, coord, horizontal_ray)) {
            // Swap start and end point so we can readd it later
            std::swap(s.start, s.end);
            ray_intersecting_lines.push_back(s);
        }

        // Insert keys according to start and end point
        if (s.start == a) {
            start_segments[a] = s;
            end_segments[b] = s;
        } else {
            start_segments[b] = s;
            end_segments[a] = s;
        }
    }

    // Inspect all point events
    while (!events.empty()) {
        sf::Vector2f event = events.top();
        events.pop();

        fire_ray(ray_intersecting_lines, coord, event);
    }

    // Handle case of reinspecting first point
    //fire_ray(ray_intersecting_lines, coord, initial_angle);

    // TODO debug remove me
    std::cout << "[";
    for (int i = 0; i < lights.getVertexCount(); ++i) {
        if (i % 3 == 0)
            std::cout << "]\n[";
        printf("(%f, %f), ", lights[i].position.x, lights[i].position.y);
    }
    std::cout << "]\n";
}

void fire_ray(std::vector<segment> &ray_intersecting_lines, sf::Vector2f &coord, sf::Vector2f &event) {
    // If the event is an end point, remove it
    if (end_segments.find(event) != end_segments.end()) {
        std::vector<segment>::iterator itr;
        itr = std::find(ray_intersecting_lines.begin(), ray_intersecting_lines.end(), end_segments[event]);

        if (itr != ray_intersecting_lines.end()) {
            ray_intersecting_lines.erase(itr);
        }
    }

    // TODO FIXME
    // handle case of connected line segments
    // > check if two line segments are touching, if they are add new segment first
    // otherwise add it afterwards as usual

    // Sort by distance, report the closest point
    distance_cmp distance_sorter(coord, event);
    std::sort(ray_intersecting_lines.begin(), ray_intersecting_lines.end(), distance_sorter);

    // If the event is a start point, add it
    // We add the start point after calculating closest point because this segment could be
    // considered the closest when there is in fact a segment behind it
    // TODO potential solution is update distance_sorter heuristic?
    if (start_segments.find(event) != start_segments.end()) {
        ray_intersecting_lines.push_back(start_segments[event]);
    }

    // Calculate intersection of ray and closest point
    sf::Vector2f ray_endpt = compute_ray(coord, event);
    segment closest_segment = ray_intersecting_lines.front();
    sf::Vector2f hit = intersection(coord, ray_endpt, closest_segment.start, closest_segment.end);

    // TODO debug remove me
    lines.append(sf::Vertex(coord, sf::Color::Red));
    lines.append(sf::Vertex(hit, sf::Color::Red));

    // Generate triangle of visibility
    if (lights.getVertexCount() % 3 == 0) {
        lights.append(sf::Vertex(coord, sf::Color::Yellow));
    }

    lights.append(sf::Vertex(hit, sf::Color::Yellow));

    if (lights.getVertexCount() > 3) {
        // Recalculate closest line to start generating the next triangle
        std::sort(ray_intersecting_lines.begin(), ray_intersecting_lines.end(), distance_sorter);
        closest_segment = ray_intersecting_lines.front();
        hit = intersection(coord, ray_endpt, closest_segment.start, closest_segment.end);
        lights.append(sf::Vertex(hit, sf::Color::Yellow));
    }
}

void draw_lines(sf::RenderWindow &window) {
    window.draw(lines);

    window.draw(lights);
}
