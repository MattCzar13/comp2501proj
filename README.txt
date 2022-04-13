# comp2501proj
Group project for COMP2501
1. Game mechanics：
	movement of the aircraft: "W","A","S","D".
	shooting: "SPACE".
	switch type of bullet: "Q", "E".
	debug buttons: "[" and "]" move the player quickly forwards and backwards, "\" gives the player a bunch of shield time
	
	The gameplay requires the player to manage their speed to dodge bullets, as well as prioritizing power-ups over killing enemies.
	The finishing condition is to kill the final boss, which is encountered at the end of the level (end of the progress bar at the bottom). 
	Dying to or failing to reach the final boss will make the player lose the game.

2. Enemies:
	there are 4 types of enemies:
		(1). Fly straight and fire bullets forward.
		(2). Swing left and right to fly and fire bullets forward.
		(3). Spin around and fire bullets around.
		(4). flys forward and shoots rapidly to the side.
		(5). boss, has 10 health.

	Each enemy has a different firing pattern and/or movement pattern, making them all unique to fight.
	For example, enemy #4 forces the player to shoot the enemy and fly through where they were, since they fire bullets rapidly on both sides, creating a wall.
	This, along with all of the other enemy designs, encourage the player to mind their positioning and solve problems quickly.

3. Weapons
	player has 2 types of weapons:
		yellow bullet: bullet fired forward.
		green bullet: Shots fired forward and at a forty-five degree angle.

	These weapons can be switched between by using "Q" and "E".
	There is an indicator in the top-left corner of the screen that shows which weapon is equipped.
	The first weapon is a fast yellow bullet that fires straight.
	The second weapon is a green spread weapon that fires 3 bullets in a trifurcated pattern.
	The weapons look different in their sprites. The yellow bullet has a bit of a tip to it, while the green bullet is more like a laser.

4. Particle systems:
	When the player picks up a shield, they gain four particles that orbit the plane rapidly.
	
5. Collectible items:
	there are 2 types of collectible items:
		health packge: Restores 1 health when the player picks it up.
		shield(power-up item with a timer): When the player picks him up, the player gains a shield that blocks damage for a period of time.

	Power-ups are spawned on a timer, and they can be collected by the player by colliding with them.
	The health power-up restores 1 health to the player. It does nothing if the player is already at full health.
	The shield power-up makes the player invincible for 5 seconds.
	The shield is a temporary power-up that has a timer attached to it. It temporarily makes the player invincible.

6. Movement and transformations
	Most objects are moved with physically based equations
	enemy(3) moves side to side useing a parametric-based equation
	hierarchical transformation used for moveing the shield orbs around the player

	All game objects use matrices to move and rotate around.
	A hierarchical transformation is present when the player picks up a shield. 4 cosmetic orbs orbit the player through hierarchical transformation.
	The orbs are treated as the player's children, and inherit the parent's translation matrix before orbiting around the player.

7. Collision
	all collision is haddeled in collision.cpp 

	All collision is circle-circle collision. Detection and responses are handled in different functions to make collision detection more modular and efficient.
	This allows us to manage collision rules by object tags.
8. Game world:
	There are three different backgrounds, they are transitioned by a progress bar.

	The game has a scrolling tiled background that consists of three different tiles: plains, desert, and volcanic.
	As the player moves forward, the background scrolls accordingly.
	As the player progresses, the background changes to different tiles to signify progress.
	These tilesets are marked on the progress bar as well.

9. UI:
	There are three hearts in the upper right corner of the window to show the player's health.
	The progress bar below the window shows the current game progress.
	A weapond indecator in the top left.

	When the game starts, a title card shows to teach the player the controls of the game.
	The game does not start until the player moves forward enough, and the title card disappears.
	When the game ends from a win or a loss, the according title card is shown, also showing the player how to exit and restart the game.

10. Code readability and organization
	The code is well commented and organized. Most functions are made to be used in as many circumstances as possible.
	For example, the bullet firing function will automatically detect what kind of object is passed in, and it will change the bullet behaviour accordingly.
	If the player is told to fire a bullet, it will make the player fire a player bullet upwards.
	If the boss is told to fire a bullet, it will make the boss fire an enemy bullet downwards.
