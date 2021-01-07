#include <math.h>
#include <stdio.h>

#define BITRATE 44100.0
#define TAU 6.2831853
#define R -127

double sqr(int i){
	return (i%2)?1.0/i:0;
}

double saw(int i){
	return 1.0/i;
}

double tri(int i){
	return (i%2)?( ((i/2)%2)?-1:1) * 1.0/(i*i) :0;
}

double envelope(double p, double a, double d, double s, double h, double r, double l){
	p = fmod(p,l);
	if (p <= a){
		return p/a;
	} else if (p <= d + a){
		return 1 - (1-s)/d*(p-a);
	} else if (p <= a + d + h){
		return s;
	} else if (p <= a + d + h + r){
		return s - (s/r)*(p - a - d - h);
	} else {
		return 0;
	}
}

double organ(int t, double note, double bound, double (*partial)(int)){
	if (note <= -64){
		return 0;
	}
	double input = t*TAU*440*pow(2,note/12.0)/BITRATE;

	double sum = 0;
	double fac = 1.0/(2*bound);
	bound += 1;
	for (int i = 1; i <= (int)bound; ++i){
		sum += fac * ((i==(int)bound)?fmod(bound,1):1) * (*partial)(i)*sin(i*input);
	}

	return sum;
}

int section_1[] =  { 12, 11, 12,  R,  4,  5,  7, -5,  0,  R,  0,  2,  5,  4,  2,-10};
int section_2[] =  { -5, -1,  2,  R,  7,  6,  2,  R, -5, -1,  2,  R,  7,  6,  2, -5};
int section_1A[] = { 12, 11, 12,  R,  4,  5,  7, -5,  0,  R,  0,  2,  5,  4,  2, -7};
int section_3[] =  { -3, -1,  0,  2,  4,  5,  8,  9,  7,  5,  4,  2,  0, -1, -3, -7};
int section_2A[] = {  0,  4,  7,  R, 12, 11,  7,  R,  0,  4,  7,  R, 12, 11,  7, -5};
int coda[] =       {  0,  0,  0,  R,  0,  R,  0,  0,  R,  0,  R,  0,  R,  R,  R,  R};
int rest[] =       {  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R,  R};

int* sections[] = {section_1, section_2, section_1A, section_3, section_1A, section_2A, section_1A, coda, rest};
double (*instruments[])(int) = {saw, tri, saw, sqr, saw, tri, saw, saw};
double section_vol[] = {1, 1.25, 1.5, 2, 1.5, 1.25, 2.2, 1};
int score_len = 16;
int main(){
	for (int t = 0;;++t){
		double beat = 2*t/BITRATE;
		int beat_i = (int)beat;
		int section = (beat_i/64) >= 8 ? 8 : (beat_i/64);
		int * fragment = sections[section];
		double (*instrument)(int) = instruments[section];

		int offset = (((int)(beat/8) % 3)==2?3.5:4)*((int)(beat/8) % 3);

		double voice_1 = envelope(beat, 0.01, 0.25-0.01, 0, 0, 0, 0.25)*organ(t, fragment[(2*beat_i)%score_len],            8*(-cos(TAU*beat/6.0)+1), instrument);
		double voice_2 = envelope(beat, 0.01, 0.5-0.01, 0, 0, 0, 0.5)*organ(t, fragment[beat_i%score_len],                  8*(-cos(TAU*beat/7.0)+1), instrument);
		double voice_3 = envelope(beat, 0.01, 0.25-0.01, 0, 0, 0, 3/4.0)*organ(t, fragment[((int)(4/3.0*beat))%score_len],  8*(-cos(TAU*beat/8.0)+1), instrument);
		double voice_4 = envelope(beat, 0.01, 1-0.01, 0, 0, 0, 1)*organ(t, fragment[((int)(0.25*beat))%score_len] - 12, 8*(-cos(TAU*beat/8.0)+1), instrument);
		double bass = envelope(beat, 0.05, 2-0.05, 0.5, 1.9, 0.1, 4)*organ(t, fragment[((int)(0.25*beat))%score_len] - 36, 12, saw);
		double sample_l = section_vol[section]*0.1*(0.50*voice_1 + 1.25*voice_2 + 1.15*voice_3 + 0.9*voice_4 + 2*bass);
		double sample_r = section_vol[section]*0.1*(1.50*voice_1 + 0.75*voice_2 + 0.85*voice_3 + 1.1*voice_4 + 2*bass);
	
		int output_l = sample_l * 65535;
		int output_r = sample_r * 65535;
    	putchar(output_l >> 8);
    	putchar(output_l);
    	putchar(output_r >> 8);
    	putchar(output_r);
	}
}
