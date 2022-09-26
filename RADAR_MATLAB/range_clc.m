r_max=300;
dis_res=1;
c=3*10^8;
T_chirp=5.5*2*r_max/c;
B_sweep=c/(2*dis_res);

fb1=0;
fb2=1.1*10^6;
fb3=13*10^6;
fb4=24*10^6;
beat_frequency= [0, 1.1e6, 13e6, 24e6];

calculated_range= (c/2)*(beat_frequency*T_chirp/B_sweep);
disp(calculated_range);

