# CS-GO-Reverse Engineered - Creating an Aimbot
Simple aimbot with cycling entities. Move crosshair to the closest enemy target.

The offsets in the .cpp file are not constantly updated. Please use hazedumper to obtain all needed offsets and pointers.

## Don't forget to add **-insecure** launch parameter before experimenting with the program, otherwise you'll get banned by VAC.

How does it work? Here's a simple math behind the aimbot hack.

For example, suppose we have a player coordinate at (1, 1, 1.5) and an enemy coordinate at (3, 4, 1). I'm going to plot their positions on a three dimensional diagram for better visualization.

![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134367464893984909/image.png)

In CS:GO, there isn't just one enemy. I'm going to create a hack that cycles through all enemy players. To make this feature even better, I'll find the closest enemy so the aimbot won't snap to the farthest enemy player. For better visualization and understanding, I'll draw a prism. We'll need to calculate the distance to every enemy by looping through them.

![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134360347529191424/Distance_PLAYER_to_ENEMY.png)

There are two types of viewing angles: horizontal and vertical. Both of them encompass 360 degrees in total. To automatically move the player's crosshair to the enemy, we'll need to write an angle value to the game memory. Here are some explanations on how to find the angle. First, I'm going to change the graph view to the top view, making it easier to understand.

![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134345140534775938/image.png)
![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134360218868920330/image.png)

Since 'a' and 'b' are known values, we will use the inverse tangent formula to find the horizontal viewing angle.

![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134377326050816010/Distance_PLAYER_to_ENEMY__1.png)

Last but not least, we are going to find the vertical viewing angle. I'm going to change the graph view to the side view. Since we've found the 's' value from the previous step (where 's' refers to the distance between the player and the enemy) and the 'q' value by subtracting the enemy's z-coordinate and the player's z-coordinate, we will use the inverse sine formula to find the vertical viewing angle.

![alt text](https://cdn.discordapp.com/attachments/1041011411658223636/1134365701143003137/image.png)

Here's the demonstration video : https://youtu.be/UWGhfoaJWjs
