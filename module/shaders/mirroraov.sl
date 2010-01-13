 surface mirroraov (output varying color _depth = 0;
    	    	output varying color _color = 0;
		output varying color _colorA = 0;
		output varying color _colorB = 0;
		output varying color _colorC = 0;
		output varying color _colorD = 0;
		output varying color _colorE = 0;
		output varying color _colorF = 0;
		output varying color _colorG = 0;
		output varying color _colorH = 0;
    	    	output varying color _reflection = 0;
		output varying color _refraction = 0;
    	    	output varying color _specular = 0;
		output varying color _specular_fill = 0;
		output varying color _specular_back = 0;
    	    	output varying color _diffuse = 0;
		output varying color _diffuse_fill = 0;
		output varying color _diffuse_back = 0;
    	    	output varying color _rim = 0;
    	    	output varying color _ambient = 0;
		output varying float _occlusion = 0;
		output varying color _objectID = 0;
		output varying color _shadow = 0;
		output varying color _epiderm = 0;
		output varying color _subderm = 0;
		output varying color _backscattering = 0;
		output varying color _normal = 0;
		)
 {
      normal Nn = normalize(N);
      vector In = normalize(I);
      float nsamp = 16;
      float fblur = radians(0.1);
      
      		color adepth = 0;
    	    	color acolor = 0;
		color acolorA = 0;
		color acolorB = 0;
		color acolorC = 0;
		color acolorD = 0;
		color acolorE = 0;
		color acolorF = 0;
		color acolorG = 0;
		color acolorH = 0;
    	    	color areflection = 0;
		color arefraction = 0;
    	    	color aspecular = 0;
		color aspecular_fill = 0;
		color aspecular_back = 0;
    	    	color adiffuse = 0;
		color adiffuse_fill = 0;
		color adiffuse_back = 0;
    	    	color arim = 0;
    	    	color aambient = 0;
		float aocclusion = 0;
		color aobjectID = 0;
		color ashadow = 0;
		color aepiderm = 0;
		color asubderm = 0;
		color abackscattering = 0;
		color anormal = 0;
		
		color tdepth = 0;
    	    	color tcolor = 0;
		color tcolorA = 0;
		color tcolorB = 0;
		color tcolorC = 0;
		color tcolorD = 0;
		color tcolorE = 0;
		color tcolorF = 0;
		color tcolorG = 0;
		color tcolorH = 0;
    	    	color treflection = 0;
		color trefraction = 0;
    	    	color tspecular = 0;
		color tspecular_fill = 0;
		color tspecular_back = 0;
    	    	color tdiffuse = 0;
		color tdiffuse_fill = 0;
		color tdiffuse_back = 0;
    	    	color trim = 0;
    	    	color tambient = 0;
		float tocclusion = 0;
		color tobjectID = 0;
		color tshadow = 0;
		color tepiderm = 0;
		color tsubderm = 0;
		color tbackscattering = 0;
		color tnormal = 0;
      
      uniform float raydepth;
    rayinfo("depth", raydepth);
    if (raydepth == 0) {
      if (Nn.In < 0) {
  	vector dir = normalize(reflect(In,Nn));
  	gather("", P, dir, fblur, nsamp, "surface:_depth", tdepth, 
  	"surface:_color", tcolor, 
  	"surface:_colorA", tcolorA, 
  	"surface:_colorB", tcolorB, 
  	"surface:_colorC", tcolorC, 
  	"surface:_colorD", tcolorD, 
  	"surface:_colorE", tcolorE, 
  	"surface:_colorF", tcolorF, 
  	"surface:_colorG", tcolorG, 
  	"surface:_colorH", tcolorH,
  	"surface:_reflection", treflection,
  	"surface:_refraction", trefraction,
  	"surface:_specular", tspecular,
  	"surface:_specular_fill", tspecular_fill,
  	"surface:_specular_back", tspecular_back,
  	"surface:_diffuse", tdiffuse,
  	"surface:_diffuse_fill", tdiffuse_fill,
  	"surface:_diffuse_back", tdiffuse_back,
  	"surface:_rim", trim,
  	"surface:_ambient", tambient,
  	"surface:_occlusion", tocclusion,
  	"surface:_objectID", tobjectID,
  	"surface:_shadow", tshadow,
  	"surface:_epiderm", tepiderm,
  	"surface:_subderm", tsubderm,
  	"surface:_backscattering", tbackscattering,
  	"surface:_normal", tnormal) {
  		adepth += tdepth;
  		acolor += tcolor;
  		acolorA += tcolorA;
  		acolorB += tcolorB;
  		acolorC += tcolorC;
  		acolorD += tcolorD;
  		acolorE += tcolorE;
  		acolorF += tcolorF;
  		acolorG += tcolorG;
  		acolorH += tcolorH;
  		areflection += treflection;
		arefraction +=  trefraction;
		aspecular += tspecular;
		aspecular_fill += tspecular_fill;
		aspecular_back += tspecular_back;
		adiffuse += tdiffuse;
		adiffuse_fill += tdiffuse_fill;
		adiffuse_back += tdiffuse_back;
		arim += trim;
		aambient += tambient;
		aocclusion += tocclusion;
		aobjectID += tobjectID;
		ashadow += tshadow;
		aepiderm += tepiderm;
		asubderm += tsubderm;
		abackscattering += tbackscattering;
		anormal += tnormal;
  	}
  	adepth /= nsamp;
  	acolor /= nsamp;
  	acolorA /= nsamp;
  	acolorB /= nsamp;
  	acolorC /= nsamp;
  	acolorD /= nsamp;
  	acolorE /= nsamp;
  	acolorF /= nsamp;
  	acolorG /= nsamp;
  	acolorH /= nsamp;
  	areflection /= nsamp;
	arefraction /= nsamp;
	aspecular /= nsamp;
	aspecular_fill /= nsamp;
	aspecular_back /= nsamp;
	adiffuse /= nsamp;
	adiffuse_fill /= nsamp;
	adiffuse_back /= nsamp;
	arim /= nsamp;
	aambient /= nsamp;
	aocclusion /= nsamp;
	aobjectID /= nsamp;
	ashadow /= nsamp;
	aepiderm /= nsamp;
	asubderm /= nsamp;
	abackscattering /= nsamp;
	anormal /= nsamp;
      }
    }
      _depth = adepth;
      _color = acolor;
      _colorA = acolorA;
      _colorB = acolorB;
      _colorC = acolorC;
      _colorD = acolorD;
      _colorE = acolorE;
      _colorF = acolorF;
      _colorG = acolorG;
      _colorH = acolorH;
      _reflection = areflection;
      _refraction = arefraction;
	_specular = aspecular;
	_specular_fill = aspecular_fill;
	_specular_back = aspecular_back;
	_diffuse = adiffuse;
	_diffuse_fill = adiffuse_fill;
	_diffuse_back = adiffuse_back;
	_rim = arim;
	_ambient = aambient;
	_occlusion = aocclusion;
	_objectID = aobjectID;
	_shadow = ashadow;
	_epiderm = aepiderm;
	_subderm = asubderm;
	_backscattering = abackscattering;
	_normal = anormal;

      Ci = Os;
      Oi = Os;
 }
