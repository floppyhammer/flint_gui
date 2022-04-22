#include "physics_system.h"

#include "../components/components.h"
#include "../coordinator.h"

#include <thread>
#include <chrono>

#define FLINT_ECS_THREADS 4

namespace Flint {
    void Physics2dSystem::update(double dt) {
        auto coordinator = Coordinator::get_singleton();

        if (false) {
            std::vector<Entity> v(entities.begin(), entities.end());
            auto entity_count = v.size();

            // Parallel.
            auto task = [&v, &entity_count, &coordinator, &dt](uint32_t index_begin) {
                for (uint32_t index = index_begin; index < entity_count; index += FLINT_ECS_THREADS) {
                    auto entity = v[index];
                    auto &rigidBody = coordinator.get_component<RigidBodyComponent>(entity);
                    auto &transform = coordinator.get_component<Transform2dComponent>(entity);
                    auto const &gravity = coordinator.get_component<GravityComponent>(entity);

                    transform.position += rigidBody.velocity.xy() * dt;
                    rigidBody.velocity += gravity.force * dt;

                    if (transform.position.x < 0) {
                        rigidBody.velocity.x = std::abs(rigidBody.velocity.x);
                    } else if (transform.position.x > WIDTH) {
                        rigidBody.velocity.x = -std::abs(rigidBody.velocity.x);
                    }
                    if (transform.position.y < 0) {
                        rigidBody.velocity.y = std::abs(rigidBody.velocity.y);
                    } else if (transform.position.y > HEIGHT) {
                        rigidBody.velocity.y = -std::abs(rigidBody.velocity.y);
                    }
                }
            };

            size_t threads_count = std::min(entity_count, (size_t) FLINT_ECS_THREADS);
            std::vector<std::thread> threads(threads_count);
            for (uint32_t i = 0; i < threads_count; i++) {
                threads[i] = std::thread(task, i);
            }

            for (auto &t: threads) {
                t.join();
            }
        } else {
            for (auto const &entity: entities) {
                auto &rigidBody = coordinator.get_component<RigidBodyComponent>(entity);
                auto &transform = coordinator.get_component<Transform2dComponent>(entity);
                auto const &gravity = coordinator.get_component<GravityComponent>(entity);

                transform.position += rigidBody.velocity.xy() * dt;
                rigidBody.velocity += gravity.force * dt;

                if (transform.position.x < 0) {
                    rigidBody.velocity.x = std::abs(rigidBody.velocity.x);
                } else if (transform.position.x > WIDTH) {
                    rigidBody.velocity.x = -std::abs(rigidBody.velocity.x);
                }
                if (transform.position.y < 0) {
                    rigidBody.velocity.y = std::abs(rigidBody.velocity.y);
                } else if (transform.position.y > HEIGHT) {
                    rigidBody.velocity.y = -std::abs(rigidBody.velocity.y);
                }
            }
        }
    }

    void Physics3dSystem::update(double dt) {
        auto coordinator = Coordinator::get_singleton();

        for (auto const &entity: entities) {
            auto &rigidBody = coordinator.get_component<RigidBodyComponent>(entity);
            auto &transform = coordinator.get_component<Transform3dComponent>(entity);
            auto const &gravity = coordinator.get_component<GravityComponent>(entity);

            transform.position += rigidBody.velocity * dt;
            rigidBody.velocity += gravity.force * dt;
        }
    }
}
