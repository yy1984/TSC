// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4 -*-
#include "../scripting.h"
#include "mrb_sprite.h"
#include "mrb_moving_sprite.h"
#include "../../objects/movingsprite.h"

/**
 * Class: MovingSprite
 *
 * Parent: [Sprite](sprite.html)
 * {: .superclass}
 *
 * Everything that is moving on the screen is considered by SMC to be a
 * _MovingSprite_. It is the superclass of most on-screen objects and as
 * such the methods defined here are available to most other objects,
 * e.g. the [Player](player.html) or [enemies](enemy.html).
 *
 * This class may not be instanciated directly.
 *
 * Acceleration and velocity
 * -------------------------
 *
 * I hope you didn’t sleep in your physics lessons, because you will
 * need your knowledge now. Remember: Velocity is _not_ the same as
 * acceleration. Velocity means you are currently moving along a given
 * path in a given time. You may for example be moving in your car at
 * 130 km/h, i.e. _in average_ your car makes 130 kilometres in one
 * hour. Acceleration is the process of altering the velocity, either
 * positively or negatively. For example, if you have to brake, your
 * velocity gets _reduced_. This is an acceleration.
 *
 * This difference gets really important when you calculate the
 * distance actually passed by a moving object. The first formula
 * below calculates that distance from a constantly moving, non-accelerating
 * object, whereas the second formula assumes a constant acceleration.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * s = v * t
 * s = (1/2)at²
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The next thing to know is that you don’t have constant acceleration in
 * SMC. Calling methods like #accelerate! give them a one-time acceleration,
 * just as if you throw something up into the air. While it continues to move
 * upwards after it left your hand,  you don’t further accelerate it while it
 * flies. But the gravity does — and does so constantly, always, first eating
 * up the velocity you accelerated the object to, then bringing it to a halt
 * (v = 0), and finally accelerating it in the other direction, i.e. towards
 * the earth’s gravity centre.
 *
 * Keep this in mind when you call the methods on this class. Accelerating
 * something doens’t mean to put it at a specific velocity, but rather to
 * add a delta value to its _current_ velocity. SMC however allows you to
 * cheat physics — rather than having to compute the acceleration you need
 * to get an object to a desired velocity you can do magic and call one of
 * the `velocity=` methods, which skip the acceleration step and directly
 * assign a velocity to an object (no, you can’t do this in reality). After
 * this however, the gravity and other overall forces still apply to the
 * now moving object, deaccelerating and finally stopping it.
 *
 * I’m too lazy at the moment, but it should be possible to look up the
 * value for `g` SMC uses and provide a formula that takes the constant
 * negative acceleration it causes into account. The same could be done
 * for the horizontal resistances, finally emitting a formula that allows
 * you to exactly calculate how a once-accelerated object behaves. If you
 * calculate such a formula, please add it in place of this text to the
 * documentation.
 */

using namespace SMC;
using namespace SMC::Scripting;

// Extern
struct RClass* SMC::Scripting::p_rcMoving_Sprite = NULL;
struct mrb_data_type SMC::Scripting::rtMoving_Sprite = {"MovingSprite", NULL};

static mrb_value Initialize(mrb_state* p_state,  mrb_value self)
{
	mrb_raise(p_state, MRB_NOTIMP_ERROR(p_state), "Cannot create instances of this class.");
	return self; // Not reached
}


/**
 * Method: MovingSprite#accelerate!
 *
 *   accelerate!( xadd, yadd )
 *
 * Add to both the horizontal and the vertical velocity at once.
 *
 * #### Parameters
 * xadd
 * : What to add to the horizontal velocity. May include fractions.
 *
 * yadd
 * : What to add to the vertical velocity. May include fractions.
 */
static mrb_value Accelerate(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_float velx;
	mrb_float vely;
	mrb_get_args(p_state, "ff", &velx, &vely);

	p_sprite->Add_Velocity(velx, vely);

	return mrb_nil_value();
}

/**
 * Method: MovingSprite#direction
 *
 *   direction() → a_symbol
 *
 * The direction the sprite is "looking" in as a symbol.
 *
 * #### Return value
 *
 * This method may return one of the following, self-explanatory symbols:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ruby
 * :undefined
 * :left
 * :right
 * :up
 * :down
 * :up_left
 * :up_right
 * :down_left
 * :down_right
 * :left_up
 * :left_down
 * :horizontal
 * :vertical
 * :all
 * :first
 * :last
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Not all of them are supported by all moving sprites (most only support
 * `:left` and `:right`), but usually you can guess which ones are
 * supported by looking at the images a sprite may use. The last five
 * directions are a bit special, namely `:horizontal`, `:vertical` and
 * `:all` may be returned for objects which support "looking" into more
 * than one direction (e.g. a static enemy may return `:all`), and `:first`
 * and `:last` can only be returned by waypoints on the world map, where
 * scripting isn’t supported yet.
 */
static mrb_value Get_Direction(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	std::string dir;
	switch(p_sprite->m_direction){
	case DIR_UNDEFINED:
		dir = "undefined";
		break;
	case DIR_LEFT:
		dir = "left";
		break;
	case DIR_RIGHT:
		dir = "right";
		break;
	case DIR_UP:
		dir = "up";
		break;
	case DIR_DOWN:
		dir = "down";
		break;
	case DIR_UP_LEFT:
		dir = "up_left";
		break;
	case DIR_UP_RIGHT:
		dir = "up_right";
		break;
	case DIR_DOWN_LEFT:
		dir = "down_left";
		break;
	case DIR_DOWN_RIGHT:
		dir = "down_right";
		break;
	case DIR_LEFT_UP:
		dir = "left_up";
		break;
	case DIR_LEFT_DOWN:
		dir = "left_down";
		break;
	case DIR_HORIZONTAL:
		dir = "horizontal";
		break;
	case DIR_VERTICAL:
		dir = "vertical";
		break;
	case DIR_ALL:
		dir = "all";
		break;
	case DIR_FIRST:
		dir = "first";
		break;
	case DIR_LAST:
		dir = "last";
		break;
	default: // Shouldn’t happen
		std::cerr << "Warning: Encountered unknown sprite direction '" << p_sprite->m_direction << "'." << std::endl;
		return mrb_nil_value();
	}
	// A world for a consecutive enum!

	return str2sym(p_state, dir);
}

/**
 * Method: MovingSprite#direction=
 *
 *   set_direction=( dir )
 *
 * Set the "looking" direction of the sprite.
 *
 * #### Parameter
 *
 * dir
 * : One of the looking directions supported by this sprite. See
 * [#direction](#direction) for a list of possible symbols.
 */
static mrb_value Set_Direction(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_sym rdir;
	mrb_get_args(p_state, "n", &rdir);
	std::string dirstr = mrb_sym2name(p_state, rdir);

	ObjectDirection dir;
	if (dirstr == "left")
		dir = DIR_LEFT;
	else if (dirstr == "right")
		dir = DIR_RIGHT;
	else if (dirstr == "up" || dirstr == "top")
		dir = DIR_UP;
	else if (dirstr == "down" || dirstr == "bottom")
		dir = DIR_DOWN;
	else if (dirstr == "up_left" || dirstr == "top_left")
		dir = DIR_UP_LEFT;
	else if (dirstr == "up_right" || dirstr == "top_right")
		dir = DIR_UP_RIGHT;
	else if (dirstr == "down_left" || dirstr == "bottom_left")
		dir = DIR_DOWN_LEFT;
	else if (dirstr == "down right" || dirstr == "bottom_right")
		dir = DIR_DOWN_RIGHT;
	else if (dirstr == "left_up" || dirstr == "left_top")
		dir = DIR_LEFT_UP;
	else if (dirstr == "left_down" || dirstr == "left_bottom")
		dir = DIR_LEFT_DOWN;
	else if (dirstr == "right_up" || dirstr == "right_top")
		dir = DIR_RIGHT_UP;
	else if (dirstr == "right_down" || dirstr == "right_bottom")
		dir = DIR_RIGHT_DOWN;
	else if (dirstr == "horizontal")
		dir = DIR_HORIZONTAL;
	else if (dirstr == "vertical")
		dir = DIR_VERTICAL;
	else if (dirstr == "all")
		dir = DIR_ALL;
	else if (dirstr == "first")
		dir = DIR_FIRST;
	else if (dirstr == "last")
		dir = DIR_LAST;
	else {
		mrb_raisef(p_state, MRB_ARGUMENT_ERROR(p_state), "Invalid direction '%s'.", dirstr.c_str());
		return mrb_nil_value(); // Not reached
	}
	// A world for a consecutive enum!

	p_sprite->Set_Direction(dir);
	return mrb_symbol_value(rdir);
}

/**
 * Method: MovingSprite#velocity_x
 *
 *   velocity_x() → a_float
 *
 * Returns the current horizontal velocity; the return value may include
 * fractions.
 */
static mrb_value Get_Velocity_X(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	return mrb_float_value(p_state, p_sprite->m_velx);
}

/**
 * Method: MovingSprite#velocity_y
 *
 *   velocity_y() → a_float
 *
 * Returns the current vertical velocity; the return value may include
 * fractions.
 */
static mrb_value Get_Velocity_Y(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	return mrb_float_value(p_state, p_sprite->m_vely);
}

/**
 * Method: MovingSprite#velocity
 *
 *   velocity  () → [a_float, another_float]
 *
 * Returns the current horizontal and vertical velocity as a two-element
 * array of form [xvel, yvel]; both values may include fractions.
 */
static mrb_value Get_Velocity(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);

	mrb_value result = mrb_ary_new(p_state);
	mrb_ary_push(p_state, result, mrb_float_value(p_state, p_sprite->m_velx));
	mrb_ary_push(p_state, result, mrb_float_value(p_state, p_sprite->m_vely));

	return result;
}

/**
 * Method: MovingSprite#velocity_x=
 *
 *   velocity_x= ( xvel )
 *
 * Set the horizontal velocity.
 *
 * #### Parameter
 *
 * xvel
 * : The new velocity. May include fractions.
 *
 */
static mrb_value Set_Velocity_X(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_float xvel;
	mrb_get_args(p_state, "f", &xvel);

	p_sprite->Set_Velocity(xvel, p_sprite->m_vely); // Keep Y velocity

	return mrb_float_value(p_state, xvel);
}

/**
 * Method: MovingSprite#velocity_y=
 *
 *   velocity_y= ( yvel )
 *
 * Set the vertical velocity.
 *
 * #### Parameter
 *
 * yvel
 * : The new velocity. May include fractions.
 *
 */
static mrb_value Set_Velocity_Y(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_float yvel;
	mrb_get_args(p_state, "f", &yvel);

	p_sprite->Set_Velocity(p_sprite->m_velx, yvel); // Keep X velocity

	return mrb_float_value(p_state, yvel);
}

/**
 * Method: MovingSprite#velocity=
 *
 *   velocity= ( \[ xvel, yvel \] )
 *
 * Set both the horizontal and vertical velocity at once.
 *
 * #### Parameters
 * xvel
 * : The new horizontal velocity. May include fractions.
 *
 * yvel
 * : The new vertical velocity. May include fractions.
 *
 * #### Example
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ruby
 * # Make an object moving very fast towards
 * # the top-right corner.
 * UIDS[12].velocity = [200, -200]
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static mrb_value Set_Velocity(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_value ary;
	mrb_get_args(p_state, "o", &ary);

	if (!mrb_array_p(ary))
		mrb_raisef(p_state, MRB_TYPE_ERROR(p_state), "Not an array: '%s'", mrb_string_value_ptr(p_state, mrb_inspect(p_state, ary)));
	if (mrb_ary_len(p_state, ary) != 2)
		mrb_raise(p_state, MRB_ARGUMENT_ERROR(p_state), "Not a two-element array.");

	mrb_float xvel = mrb_float(mrb_ary_ref(p_state, ary, 0));
	mrb_float yvel = mrb_float(mrb_ary_ref(p_state, ary, 1));

	p_sprite->Set_Velocity(xvel, yvel);

	return mrb_nil_value();
}

/**
 * Method: MovingSprite#accelerate_x!
 *
 *   accelerate_x!( val )
 *
 * Add to the current horizontal velocity.
 *
 * #### Parameter
 * val
 * : The value to add. May include fractions.
 */
static mrb_value Accelerate_X(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_float a;
	mrb_get_args(p_state, "f", &a);

	p_sprite->Add_Velocity_X(a);

	return mrb_float_value(p_state, a);
}

/**
 * Method: MovingSprite#accelerate_y!
 *
 *   accelerate_y!( val )
 *
 * Add to the current vertical velocity.
 *
 * #### Parameter
 * val
 * : The value to add. May include fractions.
 */
static mrb_value Accelerate_Y(mrb_state* p_state,  mrb_value self)
{
	cMovingSprite* p_sprite = Get_Data_Ptr<cMovingSprite>(p_state, self);
	mrb_float a;
	mrb_get_args(p_state, "f", &a);

	p_sprite->Add_Velocity_Y(a);

	return mrb_float_value(p_state, a);
}


void SMC::Scripting::Init_Moving_Sprite(mrb_state* p_state)
{
	p_rcMoving_Sprite = mrb_define_class(p_state, "MovingSprite", p_rcSprite);

	mrb_define_method(p_state, p_rcMoving_Sprite, "initialize", Initialize, ARGS_NONE());
	mrb_define_method(p_state, p_rcMoving_Sprite, "accelerate_x!", Accelerate_X, ARGS_REQ(1));
	mrb_define_method(p_state, p_rcMoving_Sprite, "accelerate_y!", Accelerate_Y, ARGS_REQ(1));
	mrb_define_method(p_state, p_rcMoving_Sprite, "accelerate!", Accelerate, ARGS_REQ(2));
	mrb_define_method(p_state, p_rcMoving_Sprite, "direction", Get_Direction, ARGS_NONE());
	mrb_define_method(p_state, p_rcMoving_Sprite, "direction=", Set_Direction, ARGS_REQ(1));
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity_x", Get_Velocity_X, ARGS_NONE());
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity_y", Get_Velocity_Y, ARGS_NONE());
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity", Get_Velocity, ARGS_NONE());
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity_x=", Set_Velocity_X, ARGS_REQ(1));
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity_y=", Set_Velocity_Y, ARGS_REQ(1));
	mrb_define_method(p_state, p_rcMoving_Sprite, "velocity=", Set_Velocity, ARGS_REQ(1));
}
