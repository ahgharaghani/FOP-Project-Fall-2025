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

float distance(const Vec2 a, const Vec2 b);
int own_goal_x(const struct Player *self);
int opponent_goal_x(const struct Player *self);
bool is_in_own_half(const struct Player *self);
bool ball_in_own_half(const struct Player *self, const struct Ball *ball);
bool ball_approaching_own_goal(const struct Player *self, const struct Ball *ball);
void move_towards(struct Player *self, const struct Vec2 target, float speed);
float clamp_speed(const float value, const float min, const float max);
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

/* Team 1 movement logic */
void movement_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_1_5(struct Player *self, const struct Scene *scene) { (void)scene; }

/* Team 2 movement logic */
void movement_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void movement_logic_2_5(struct Player *self, const struct Scene *scene) { (void)scene; }

/* Team 1 shooting logic */
void shooting_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_1_5(struct Player *self, const struct Scene *scene) { (void)scene; }

/* Team 2 shooting logic */
void shooting_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void shooting_logic_2_5(struct Player *self, const struct Scene *scene) { (void)scene; }

/* Team 1 change_state logic */
void change_state_logic_1_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_1_5(struct Player *self, const struct Scene *scene) {
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

/* Team 2 change_state logic */
void change_state_logic_2_0(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_1(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_2(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_3(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_4(struct Player *self, const struct Scene *scene) { (void)scene; }
void change_state_logic_2_5(struct Player *self, const struct Scene *scene) {
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

float clamp_speed(const float value, const float min, const float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int closest_teammate_to_ball(const struct Scene *scene, const struct Player *self) {
    const struct Ball *ball = scene->ball;
    struct Player *players = (self->team == 1) ? scene->first_team->players : scene->second_team->players;
    float d = distance(self->position, ball->position);
    int kit = self->kit;
    for (int i = 0; i < 6; i++) {
        float others_d = distance(players[i].position, ball->position);
        if (others_d < d) {
            d = others_d;
            kit = players[i].kit;
        }
    }
    return kit;
}

bool is_closest_to_ball(const struct Scene *scene, const struct Player *self) {
    return closest_teammate_to_ball(scene, self) == self->kit;
}