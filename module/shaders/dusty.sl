surface
dusty(float Kd = 0.5;string hdrname = "/home/pan/Public/kitchen.hdr";string spacename = "object";float exposure = -4;
varying color keysh0 = color(1,1,1);
varying color keysh1 = color(1,1,1);
varying color keysh2 = color(1,1,1);
varying color keysh3 = color(1,1,1);
varying color keysh4 = color(1,1,1);
varying color keysh5 = color(1,1,1);
varying color keysh6 = color(1,1,1);
varying color keysh7 = color(1,1,1);
varying color keysh8 = color(1,1,1);
varying color backsh0 = color(1,1,1);
varying color backsh1 = color(1,1,1);
varying color backsh2 = color(1,1,1);
varying color backsh3 = color(1,1,1);
varying color backsh4 = color(1,1,1);
varying color backsh5 = color(1,1,1);
varying color backsh6 = color(1,1,1);
varying color backsh7 = color(1,1,1);
varying color backsh8 = color(1,1,1);)
{
	Oi = Os;
	Ci = Cs;
	
	vector side = vector(1,0,0);
	vector up = vector(0,1,0);
	
	if(spacename != "") {
		side = normalize(vtransform (spacename, "world", side));
		up = normalize(vtransform (spacename, "world", up));
	}
	color iblkey = SHLighting(hdrname, exposure, side, up, keysh0, keysh1, keysh2, keysh3, keysh4, keysh5, keysh6, keysh7, keysh8);
	color iblback = SHLighting(hdrname, exposure, side, up, backsh0, backsh1, backsh2, backsh3, backsh4, backsh5, backsh6, backsh7, backsh8);
	Ci = iblkey +iblback;
	Ci*=Oi;
}
