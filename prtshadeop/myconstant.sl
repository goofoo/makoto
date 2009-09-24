plugin "zPRT";

surface
prtconstant()
{
    Oi = Os;
    
    string shdr = "/home/pan/Public/kitchen.hdr";
    float exposure = -0.5;
    vector vx = vector(1,0,0);
    vector vy = vector(0,1,0); 
    float coeff[16];
    coeff[0]=coeff[1]=coeff[2]=coeff[3]=coeff[4]=coeff[5]=coeff[6]=coeff[7]=coeff[8]=coeff[9]=coeff[10]=coeff[11]=coeff[12]=coeff[13]=coeff[14]=coeff[15] = 0.5;
    normal nf = normal(0,1,0);
    color cfoo = SHLighting(shdr, exposure, vx, vy, nf, coeff[0], coeff[1], coeff[2], coeff[3], coeff[4], coeff[5], coeff[6], coeff[7], coeff[8], coeff[9], coeff[10], coeff[11], coeff[12], coeff[13], coeff[14], coeff[15]);
    Ci = Os * cfoo;
}
