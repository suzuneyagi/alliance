@lliance

Authors: Suzune Yagi, Meryem Koksal

Description:
Our project is a game titled @lliance. 
@lliance is a collaborative game where players try to reach the goal helping each other on the way.

Players go down five levels as they progress in the game. They automatically fall down when there is 
no wall (floor) under them. Their goal is to reach the goal tile at the bottom level. 

The code reads a map from a file, and displays it. The players move within the map. Players cannot
go through the walls that surround the map.
The map is encoded in an array from the game map file. Each element is a unique enum value that corresponds to the various 
states of the game map. 

Player Red is represented as a red @, and can move by pressing WASD keys. 
Player Blue is represented as a blue @, and can move by arrow keys.

(W/KEY＿UP - jump, A/KEY_LEFT - move left, D/KEY_RIGHT - move right)

Players can use buttons that change the states of different parts of the map, allowing the players to move forward. 
They are supposed to collaborate to find the how to use the buttons. 
For example, the buttons can open the floor for the players to go down, or oepn "doors" to go forward.

If the players touch tiles with a color that is not theirs or collide with the moving objects, they lose, but can 
restart the game by pressing space. 


 You can play @lliance by running the following commands:
 make
 ./alliance game.map
