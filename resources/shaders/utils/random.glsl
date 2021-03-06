const float c_pi = 3.14159265359f;
const float c_twopi = 2.0f * c_pi;

float remap(float s, float a1, float a2, float b1, float b2) {
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

vec3 remap(vec3 v, float a1, float a2, float b1, float b2) {
    v.x = remap(v.x, a1, a2, b1, b2);
    v.y = remap(v.y, a1, a2, b1, b2);
    v.z = remap(v.z, a1, a2, b1, b2);
    return v;
}

float fresnelReflectAmount(float n1, float n2, vec3 normal, vec3 incident, float f0, float f90) {
    // Schlick aproximation
    if(n1 == -1.0 || n2 == -1.0f)
        return f0;
    float r0 = (n1-n2) / (n1+n2);
    r0 *= r0;
    float cosX = -dot(normal, incident);
    if (n1 > n2)
    {
        float n = n1/n2;
        float sinT2 = n*n*(1.0-cosX*cosX);
        // Total internal reflection
        if (sinT2 > 1.0)
        return f90;
        cosX = sqrt(1.0-sinT2);
    }
    float x = 1.0-cosX;
    float ret = r0+(1.0-r0)*x*x*x*x*x;

    // adjust reflect multiplier for object reflectivity
    return mix(f0, f90, ret);
}

uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

uint initRandom(vec2 v, uint frame) {
    return uint(uint(v.x) * uint(1973) + uint(v.y) * uint(9277) + frame * uint(26699)) | uint(1);
}

int randomInt(inout uint seed, int min, int max) {
    int i = int(wang_hash(seed));
    if (i < 0)
        i *= -1;
    return (i % (max - min + 1)) + min;
}

float randomFloat(inout uint seed) {
    return float(wang_hash(seed)) / 4294967296.0;
    //return float(wang_hash(seed));// / 67296.0;
}

vec3 randomUnitVector(inout uint seed) {
    float z = randomFloat(seed) * 2.0f - 1.0f;
    float a = randomFloat(seed) * c_twopi;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

vec3 cosWeightedRandomHemisphereDirection(const vec3 n, inout uint seed) {
    vec2 r = vec2(randomFloat(seed), randomFloat(seed));

    vec3  uu = normalize(cross(n, vec3(0.0, 1.0, 1.0)));
    vec3  vv = cross(uu, n);

    float ra = sqrt(r.y);
    float rx = ra*cos(6.2831*r.x);
    float ry = ra*sin(6.2831*r.x);
    float rz = sqrt(1.0-r.y);
    vec3  rr = vec3(rx*uu + ry*vv + rz*n);

    return normalize(rr);
}

float DistributionGGX(float NdotH, float alpha )
{
	float a2 =  alpha * alpha;

	float d = (NdotH * NdotH) * (a2-1.0f) + 1.0f;
    return a2 / (d * d * c_pi);
}

float smithLambda(float wDotN, float alpha) 
{
    float wDotN2 = wDotN * wDotN;
    float tanSqrd = max((1-wDotN2),0.0f) / wDotN2;

    return 0.5f * (-1 + sqrt(1 + alpha * tanSqrd));
}

float SmithHeightCorrelated(float w0DotN, float wiDotN, float alpha) 
{
    if (w0DotN <= 0 || wiDotN <= 0) 
        return 0;

    return 1.0/(1.0+smithLambda(w0DotN,alpha)+smithLambda(wiDotN,alpha));
}

vec3 evalFresnelSchlick(vec3 f0, vec3 f90, float cosTheta)
{
    return f0 + (f90 - f0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f); // Clamp to avoid NaN if cosTheta = 1+epsilon
}

vec3 importanceSampleGGX(inout uint seed, float alpha, vec3 normal, vec3 w0) 
{
    vec2 Xi = vec2(randomFloat(seed), randomFloat(seed));
	float phi = 2.0 * c_pi * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	//from spherical coordinates to cartesian coordinates
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// Tangent space
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangentX = normalize(cross(up, normal));
	vec3 tangentY = normalize(cross(normal, tangentX));

	// Convert to world Space
	vec3 h = normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
	
	vec3 wi = reflect(w0, h);

	if(dot(wi, normal) > 0.0f) {
        return wi;
    }
	else {
		return vec3(0.0f);
	}
}