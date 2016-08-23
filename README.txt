README

Project description:
kgoedicke.com/Phong-Shader

1. Compile the program with "make"
2. On the command line, type "./shade" followed by each desired shading specification. The options are:
	ambient coefficient: "-ka R G B" replacing R G B with the desired red green blue values 0 to 1
	diffuse coefficient: "-kd R G B" replacing R G B with the desired red green blue values 0 to 1
	specular coefficient: "-ka R G B" replacing R G B with the desired red green blue values 0 to 1
	specular power: "-sp #" replacing # with your desired specular power
	point light: "-pl X Y Z R G B" replacing X Y Z with the desired x y z location of the light in the scene and R G B with the desired red green blue values 0 to 1
	directional light: "-dl X Y Z R G B" replacing X Y Z with the desired x y z location of the light in the scene and R G B with the desired red green blue values 0 to 1
3. User may specify multiple point and directional lights


Example command line inputs for interesting results are:

./shade -ka 0.05 0.05 0.8 -kd .2 .6 .1 -ks .2 .8 .9 -sp 64 -pl 1 2 2 0.1 0.9 .1 -pl 2 2 1 .8 .5 0

./shade -ka 0.05 0.05 0.05 -kd .5 .5 .5 -ks 0 1 .6 -sp 64 -pl 1 2 2 1 .5 .5 -pl 2 2 1 1 .3 .7 -pl 0 -2 2 0 .9 1

./shade -ka 0.05 0.05 0.05 -kd .5 .5 .5 -ks 0 1 .6 -sp 64 -pl 2 2 1 0 .3 .7 -pl 0 -2 2 0 .9 1


