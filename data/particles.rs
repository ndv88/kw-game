Texture tp1
{
	filename="data\particles\particles.png"
}

Sprite sp1
{
	texture=tp1
	rect=40,0,32,32
	hotspot=16,16
	blendmode=COLORMUL,ALPHAADD,NOZWRITE
}

Sprite sp2
{
	texture=tp1
	rect=0,32,32,32
	hotspot=16,16
	blendmode=COLORMUL,ALPHAADD,NOZWRITE
}

Sprite sp3
{
	texture=tp1
	rect=0,0,32,32
	hotspot=16,16
	blendmode=COLORMUL,ALPHAADD,NOZWRITE
}

Sprite detonation1
{
	texture=tp1
	rect=0,0,32,32
	hotspot=16,16
	blendmode=COLORMUL,ALPHAADD,NOZWRITE
}

Particle particle_trail1
{
	filename="data\particles\trail1.psi"
	sprite=sp1
	fps=100.0
}

Particle particle_trail2
{
	filename="data\particles\trail2.psi"
	sprite=sp2
	fps=100.0
}

Particle particle_trail3
{
	filename="data\particles\trail3.psi"
	sprite=sp3
	fps=100.0
}

Particle particle_detonation1
{
	filename="data\particles\detonation1.psi"
	sprite=detonation1
	fps=100.0
}

Particle particle_detonation2
{
	filename="data\particles\detonation2.psi"
	sprite=detonation1
	fps=100.0
}