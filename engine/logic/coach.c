#include "coach.h"
#include "core/constants.h"
#include "entities/ball.h"
#include "entities/team.h"
#include "game/scene.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define GOAL_LINE_OFFSET 30
#define GOAL_TOP CENTER_Y - GOAL_HEIGHT / 2
#define GOAL_BOTTOM CENTER_Y + GOAL_HEIGHT / 2

float distance(const Vec2 a, const Vec2 b);
int own_goal_x(const struct Player *self);
int opponent_goal_x(const struct Player *self);
bool is_in_own_half(const struct Player *self);
bool ball_in_own_half(const struct Player *self, const struct Ball *ball);
bool ball_approaching_own_goal(const struct Player *self, const struct Ball *ball);
void move_towards(struct Player *self, const struct Vec2 target, float speed);
float clamp(const float value, const float min, const float max);
int closest_teammate_to_ball(const struct Scene *scene, const struct Player *self);
bool is_closest_to_ball(const struct Scene *scene, const struct Player *self);

// Set to false to let the other team use their own logic (if you implement it)
// Set to true to test your logic on both teams
bool coach_both_teams = true;

/* -------------------------------------------------------------------------
 * Logic Functions
 *  TODO 1: You must implement the following functions in Phase 2.
 *        Each player in each team has its own functions.
 *        You can add new functions, but are NOT ALLOWED to remove
 *        the existing functions or change their structure.
 * ------------------------------------------------------------------------- 
 * ⚠️ STUDENT RULES FOR PHASE 2:
 * You are restricted to modifying ONLY specific variables in each function:
 *
 * 1. MOVEMENT FUNCTIONS (movement_logic_X_Y):
 * Allowed: player->velocity
 * Goal:    Determine the direction and speed of movement.
 *
 * 2. SHOOTING FUNCTIONS (shooting_logic_X_Y):
 * Allowed: ball->velocity
 * Goal:    Determine the direction and power of the kick/pass.
 *
 * 3. CHANGE STATE FUNCTIONS (change_state_logic_X_Y):
 * Allowed: player->state
 * Goal:    Switch between IDLE, MOVING, SHOOTING, or INTERCEPTING.
 *
 * NOTE: Directly modifying any other attributes will be flagged as a violation.
 * Thank you for your attention to this matter!
 * ------------------------------------------------------------------------- */

void movement_logic_goalkeeper(struct Player *self, const struct Scene *scene) {
    const struct Ball *ball = scene->ball;
    float goal_line_x = (self->team == 1) ? PITCH_X + GOAL_LINE_OFFSET : SCREEN_WIDTH - PITCH_X - GOAL_LINE_OFFSET;
    float max_gk_speed = self->talents.agility * MAX_PLAYER_VELOCITY / 10;

    float target_x, target_y;
    target_x = goal_line_x;
    if (ball_in_own_half(self, ball)) {
        target_y = ball->position.y;
        target_y = clamp(target_y, GOAL_TOP - 20.0f, GOAL_BOTTOM + 20.0f); /* Set 20.0f for margin */
    } else {
        /* Ball is far. It's none of goalkeeper's concern */
        target_y = CENTER_Y;
    }

    if (ball_approaching_own_goal(self, ball) && hypotf(ball->position.x, ball->position.y) > 50.0f) {
        float time_to_goal;
        if (self->team == 1 && ball->velocity.x < 0.0f) {
            time_to_goal = (ball->position.x - PITCH_X) / (-ball->velocity.x); /* Time it would take for the ball to reach the goal line */
        } else if (self->team == 2 && ball->velocity.x > 0.0f) {
            time_to_goal = (SCREEN_WIDTH - PITCH_X - ball->position.x) / ball->velocity.x;
        }
        float target_y = ball->position.y + ball->velocity.y * time_to_goal;
        target_y = clamp(target_y, GOAL_TOP - 20.0f, GOAL_BOTTOM + 20.0f);
    }

    struct Vec2 target = {.x = target_x, .y = target_y};
    move_towards(self, target, max_gk_speed);
    /* TODO: Implement separation of players */
}

/* Team 1 movement logic */
void movement_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_5(struct Player *self, const struct Scene *scene) {
    movement_logic_goalkeeper(self, scene);
}

/* Team 2 movement logic */
void movement_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_5(struct Player *self, const struct Scene *scene) {
    movement_logic_goalkeeper(self, scene);
}

void shooting_logic_goalkeeper(struct Player *self, const struct Scene *scene) {
    struct Ball *ball = scene->ball;

    /* Find best teammate to pass to */

    struct Player *best_target = NULL;
    float best_distance_to_opponent_goal = INFINITY;
    struct Player **teammates = (self->team == 1) ? scene->first_team->players : scene->second_team->players;
    struct Player **opponnents = (self->team == 1) ? scene->second_team->players : scene->first_team->players;

    for (int i = 0; i < 5; i++) {
        float d = distance(self->position, teammates[i]->position);
        bool opponenet_nearby = false;
        for (int j = 0; j < 6; j++) {
            if (distance(teammates[i]->position, opponnents[j]->position) < 25.0f) opponenet_nearby = true;
        }
        if (!opponenet_nearby && d < best_distance_to_opponent_goal) {
            best_distance_to_opponent_goal = d;
            best_target = teammates[i];
        }
    }

    if (best_target != NULL) {
        float dx = best_target->position.x - self->position.x;
        float dy = best_target->position.y - self->position.y;
        
        float max_ball_speed = self->talents.agility * MAX_BALL_VELOCITY / 10;
        
        ball->velocity.x = (dx / best_distance_to_opponent_goal) * max_ball_speed;
        ball->velocity.y = (dy / best_distance_to_opponent_goal) * max_ball_speed;
    } else {
        /* No good target, simply shoot it forward */
        if (self->team == 1) {
            ball->velocity.x = 50.0f;
            ball->velocity.y = 0;
        } else {
            ball->velocity.x = -50.0f;
            ball->velocity.y = 0;
        }
    }

    if (self->team == 1 && ball->velocity.x < 0) {
        ball->velocity.x = fabsf(ball->velocity.x);
    } else if (self->team == 2 && ball->velocity.x > 0) {
        ball->velocity.x = -fabsf(ball->velocity.x);
    }
}

/* Team 1 shooting logic */
void shooting_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_5(struct Player *self, const struct Scene *scene) {
    shooting_logic_goalkeeper(self, scene);
}

/* Team 2 shooting logic */
void shooting_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_5(struct Player *self, const struct Scene *scene) {
    shooting_logic_goalkeeper(self, scene);
}

void change_state_logic_goalkeeper(struct Player *self, const struct Scene *scene) {
    const struct Ball *ball = scene->ball;
    const struct Player *possessor = ball->possessor;

    if (possessor == self) {
        self->state = SHOOTING; return;
    }
    if (possessor != NULL && possessor->team == self->team) {
        self->state = MOVING; return;
    }

    float distance_from_ball = distance(self->position, ball->position);
    bool in_own_half = ball_in_own_half(self, ball);
    bool ball_in_goal_range = (ball->position.y > CENTER_Y - GOAL_HEIGHT/2 - 40.0f) && (ball->position.y < CENTER_Y + GOAL_HEIGHT/2 + 40.0f);
    bool heading_towards_me = ball_approaching_own_goal(self, ball);
    float ball_speed = hypotf(ball->velocity.x, ball->velocity.y);

    if (distance_from_ball < 60.0f && in_own_half && (heading_towards_me || ball_speed < 20.0f) && ball_in_goal_range) {
        self->state = INTERCEPTING; return;
    }

    self->state = MOVING;
}

/* Team 1 change_state logic */
void change_state_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_5(struct Player *self, const struct Scene *scene) {
    change_state_logic_goalkeeper(self, scene);
}

/* Team 2 change_state logic */
void change_state_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_5(struct Player *self, const struct Scene *scene) {
    change_state_logic_goalkeeper(self, scene);
}

/* -------------------------------------------------------------------------
 * Lookup tables for factory
 * ------------------------------------------------------------------------- */
static PlayerLogicFn team1_movement[6] = {
    movement_logic_1_0, movement_logic_1_1, movement_logic_1_2,
    movement_logic_1_3, movement_logic_1_4, movement_logic_1_5
};

static PlayerLogicFn team2_movement[6] = {
    movement_logic_2_0, movement_logic_2_1, movement_logic_2_2,
    movement_logic_2_3, movement_logic_2_4, movement_logic_2_5
};

static PlayerLogicFn team1_shooting[6] = {
    shooting_logic_1_0, shooting_logic_1_1, shooting_logic_1_2,
    shooting_logic_1_3, shooting_logic_1_4, shooting_logic_1_5
};

static PlayerLogicFn team2_shooting[6] = {
    shooting_logic_2_0, shooting_logic_2_1, shooting_logic_2_2,
    shooting_logic_2_3, shooting_logic_2_4, shooting_logic_2_5
};

static PlayerLogicFn team1_change_state[6] = {
    change_state_logic_1_0, change_state_logic_1_1, change_state_logic_1_2,
    change_state_logic_1_3, change_state_logic_1_4, change_state_logic_1_5
};

static PlayerLogicFn team2_change_state[6] = {
    change_state_logic_2_0, change_state_logic_2_1, change_state_logic_2_2,
    change_state_logic_2_3, change_state_logic_2_4, change_state_logic_2_5
};

/* -------------------------------------------------------------------------
 * Factory functions
 * ------------------------------------------------------------------------- */
PlayerLogicFn get_movement_logic(int team, int kit) {
    if (coach_both_teams) return team1_movement[kit];
    return (team == 1) ? team1_movement[kit] : team2_movement[kit];
}

PlayerLogicFn get_shooting_logic(int team, int kit) {
    if (coach_both_teams) return team1_shooting[kit];
    return (team == 1) ? team1_shooting[kit] : team2_shooting[kit];
}

PlayerLogicFn get_change_state_logic(int team, int kit) {
    if (coach_both_teams) return team1_change_state[kit];
    return (team == 1) ? team1_change_state[kit] : team2_change_state[kit];
}

/* -------------------------------------------------------------------------
 * TALENTS
 *  TODO 2: Replace these default values with your desired skill points.
 * ------------------------------------------------------------------------- */
/* Team 1 */
static struct Talents team1_talents[6] = {
    {2, 5, 5, 8}, // Striker
    {3, 5, 6, 6}, // Attacker
    {4, 6, 5, 5}, // Midfielder
    {7, 5, 4, 4}, // Defender
    {7, 5, 4, 4}, // Defender
    {8, 5, 3, 4}, // GoalKeeper
};

/* Team 2 */
static struct Talents team2_talents[6] = {
    {2, 5, 5, 8}, // Striker
    {3, 5, 6, 6}, // Attacker
    {4, 6, 5, 5}, // Midfielder
    {7, 5, 4, 4}, // Defender
    {7, 5, 4, 4}, // Defender
    {8, 5, 3, 4}, // GoalKeeper
};

struct Talents get_talents(int team, int kit) {
    if (coach_both_teams) return team1_talents[kit];
    return (team == 1) ? team1_talents[kit] : team2_talents[kit];
}


/* -------------------------------------------------------------------------
 * Positioning
 *  TODO 3: Decide players positions at kick-off.
 *        Players must stay on their half, outside the center circle.
 *        Keep in mind that the kick-off team's first player will automatically
 *             be placed at the center of the pitch.
 * ------------------------------------------------------------------------- */
/* Team 1 */
static struct Vec2 team1_positions[6] = {
    {300, CENTER_Y + 140},
    {300, CENTER_Y - 140},
    {330, CENTER_Y},
    {180, CENTER_Y + 100},
    {180, CENTER_Y - 100},
    {PITCH_X + 30, CENTER_Y},
};

/* Team 2 */
static struct Vec2 team2_positions[6] = {
    {SCREEN_WIDTH - PITCH_MARGIN - 300, CENTER_Y + 140},
    {SCREEN_WIDTH - PITCH_MARGIN - 300, CENTER_Y - 140},
    {SCREEN_WIDTH - PITCH_MARGIN - 330, CENTER_Y},
    {SCREEN_WIDTH - PITCH_MARGIN - 180, CENTER_Y + 100},
    {SCREEN_WIDTH - PITCH_MARGIN - 180, CENTER_Y - 100},
    {PITCH_X + PITCH_W - 30, CENTER_Y},
};

struct Vec2 get_positions(int team, int kit) {
    return (team == 1) ? team1_positions[kit] : team2_positions[kit];
}

float distance(const Vec2 a, const Vec2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return hypotf(dx, dy);
}

int own_goal_x(const struct Player *self) {
    if (self->team == 1) return PITCH_X;
    return PITCH_X + PITCH_W; 
}

int opponent_goal_x(const struct Player *self) {
    if (self->team == 1) return PITCH_X + PITCH_W;
    return PITCH_X; 
}

bool is_in_own_half(const struct Player *self) {
    if (self->team == 1) return self->position.x < CENTER_X;
    return self->position.x > CENTER_X;
}

bool ball_in_own_half(const struct Player *self, const struct Ball *ball) {
    if (self->team == 1) return ball->position.x < CENTER_X;
    return ball->position.x > CENTER_X;
}

bool ball_approaching_own_goal(const struct Player *self, const struct Ball *ball) {
    if (self->team == 1) return ball->velocity.x < 0;
    return ball->velocity.x > 0;
}

void move_towards(struct Player *self, const struct Vec2 target, float speed) {
    float dx = self->position.x - target.x;
    float dy = self->position.y - target.y;
    float d = hypotf(dx, dy);
    self->velocity.x = (dx/d) * speed;
    self->velocity.y = (dy/d) * speed;
}

float clamp(const float value, const float min, const float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int closest_teammate_to_ball(const struct Scene *scene, const struct Player *self) {
    const struct Ball *ball = scene->ball;
    struct Player **players = (self->team == 1) ? scene->first_team->players : scene->second_team->players;
    float d = distance(self->position, ball->position);
    int kit = self->kit;
    for (int i = 0; i < 6; i++) {
        float others_d = distance(players[i]->position, ball->position);
        if (others_d < d) {
            d = others_d;
            kit = players[i]->kit;
        }
    }
    return kit;
}

bool is_closest_to_ball(const struct Scene *scene, const struct Player *self) {
    return closest_teammate_to_ball(scene, self) == self->kit;
}