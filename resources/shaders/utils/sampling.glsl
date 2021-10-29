void sampleLight(inout vec3 ro, inout vec3 ld, inout float light_dist, inout float weight, uint seed) {
    Light light = lights_[0];
    const vec3 light_pos = light.corner.xyz + light.v1.xyz * randomFloat(seed) + light.v2.xyz * randomFloat(seed);
    ld = light_pos - ro;
    light_dist = length(ld);
    ld = normalize(ld);
    const float n_dot_l = dot(ray_.normal, ld);
    const float ln_dot_l = -dot(light.normal.xyz, ld);
    const float A = length(cross(light.v1.xyz, light.v2.xyz));
    weight = n_dot_l * ln_dot_l * A / (c_pi * light_dist * light_dist);
//    weight = clamp(weight, 0.0, 1.0);// remove fireflies
}

float luminance ( inout vec3 rgb ){
    return rgb.x * 0.212671f + rgb.y * 0.715160f + rgb.z * 0.072169f;
}

void sampleBRDF(inout vec3 ray_dir, inout vec3 ray_pos, inout vec3 throughput, inout bool specular_bounce, inout uint seed) {
    vec3 rayDirO = ray_dir;
    float pdf = 1.0f;
    float do_specular = 0.0f;
    float do_refraction = 0.0f;

    vec3 tempx = (abs(ray_.normal.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
    vec3 mY = normalize(cross(tempx, ray_.normal));
    vec3 mX = cross(mY, ray_.normal);
    vec3 reflDir = vec3( -dot(-ray_dir, mX), -dot(-ray_dir, mY), dot(-ray_dir, ray_.normal));
    vec3 newRayDir;
    float diffChance = 0.0f;
    float specChance = 0.0f;

    float randX = randomFloat(seed);
    uint aux = uint(randX);
    float randY = randomFloat(aux);
    aux = uint(randX + randY);
    float randZ = randomFloat(aux);
    vec3 bsdfFactor;
    float cosTheta;

    float specular_chance = ray_.mat.specular_chance;
    float refraction_chance = ray_.mat.refraction_chance;
    float event = 0;

    if (specular_chance > 0.0f) {
        specular_chance = fresnelReflectAmount(
        ray_.from_inside ? ray_.mat.IOR : 1.0,
        !ray_.from_inside ? ray_.mat.IOR : 1.0,
        ray_dir,
        ray_.normal,
        ray_.mat.specular_chance,
        1.0f
        );

        float chance_multiplier = (1.0f - specular_chance) / (1.0f - ray_.mat.specular_chance);
        refraction_chance *= chance_multiplier;
    }

    float ray_select_roll = randomFloat(seed);

    if (specular_chance > 0.0f && ray_select_roll < specular_chance) {
        specular_bounce = true;
        do_specular = 1.0f;
        pdf = specular_chance;
        event = 2;
        diffChance = 1 - specular_chance;
    }
    else if (refraction_chance > 0.0f && ray_select_roll < specular_chance + refraction_chance) {
        specular_bounce = true;
        do_refraction = 1.0f;
        pdf = refraction_chance;
        refraction_chance = 1.0f;
        specular_chance = 0.0f;
    }
    else {
        specular_bounce = false;
        pdf = 1.0f - (specular_chance + refraction_chance);
        diffChance = pdf;
        event = 1;
    }

    pdf = max(pdf, 0.001f);


    // calculate new ray direction
    vec3 diffuse_ray_dir = normalize(ray_.normal + randomUnitVector(seed));

    vec3 specular_ray_dir = reflect(ray_dir, ray_.normal);
    specular_ray_dir = normalize(mix(specular_ray_dir, diffuse_ray_dir, ray_.mat.specular_roughness * ray_.mat.specular_roughness));

    vec3 refraction_ray_dir = refract(ray_dir, ray_.normal, ray_.from_inside ? ray_.mat.IOR : 1.0f / ray_.mat.IOR);
    refraction_ray_dir = normalize(mix(refraction_ray_dir, normalize(-ray_.normal + randomUnitVector(seed)), 0));

    ray_dir = mix(diffuse_ray_dir, specular_ray_dir, do_specular);
    ray_dir = mix(ray_dir, refraction_ray_dir, do_refraction);
    
    
    if(ray_.mat.mirror_reflectance == 0 && ray_.mat.phong == 0){   

         // update the ray position
        if (do_refraction == 1.0f ) {
            specular_bounce = true;
            ray_pos = (ray_pos + rayDirO * ray_.t) - ray_.normal * 0.01f;
        }
        else {
            ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
        }
        
        if (ray_.from_inside) {
            throughput *= exp(-ray_.mat.refraction_color);
        }
        if (do_refraction == 0.0f) {
            throughput *= mix(ray_.mat.albedo, ray_.mat.specular_color, do_specular);
        }
        throughput /= pdf;
    }
    if(ray_.mat.mirror_reflectance != 0){
        specular_bounce = true;
        diffChance = 0.0f;
        specular_chance = 0.0f;
        if (ray_.mat.mIOR > 0){
            ray_pos = (ray_pos + rayDirO * ray_.t) - ray_.normal * 0.01f;


            float cosI = dot(-rayDirO ,normalize(ray_.normal));
            float cosT;
            float etaIncOverEtaTrans;
            if(ray_.from_inside){ //from inside
            
                etaIncOverEtaTrans = ray_.mat.IOR;
                cosI = -cosI;
                cosT = 1.0f;
            }
            else{

                etaIncOverEtaTrans = 1.0f/ray_.mat.IOR;
                cosT = -1.0f;
            }

            float sinI2 = 1.0f - cosI*cosI;
            float sinT2 = etaIncOverEtaTrans*etaIncOverEtaTrans * sinI2;

            if(sinT2 < 1.0f){
            
                pdf = 1;

                ray_dir = normalize( refract(rayDirO, ray_.normal, ray_.from_inside ? ray_.mat.mIOR : 1.0f / ray_.mat.mIOR));

                throughput *= vec3(1.0f)/ pdf;  
            }

            

        }
        else{ 
            ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
            pdf = 1;
            ray_dir = normalize(reflect(rayDirO, ray_.normal));
            bsdfFactor = vec3(ray_.mat.mirror_reflectance);
            throughput = vec3(1);
        }
    
    }
    else if(event == 1 && ray_.mat.phong==1){
        //diffuse
        ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
        float term1 = 2.0f * c_pi * randX;
        float term2 = sqrt( 1.0f - randY);

        newRayDir = vec3( cos(term1)*term2, sin(term1)*term2, sqrt(randY));
        pdf = (newRayDir.z / c_pi) * diffChance;
        bsdfFactor = ray_.mat.albedo / c_pi;
        //ray_dir = mX*newRayDir.x + mY*newRayDir.y + normalize(ray_.normal)*newRayDir.z;
        float cosTheta = abs(newRayDir.z);
        throughput *= bsdfFactor * cosTheta / pdf;//pdf;
    }
    else if (ray_.mat.phong == 1){
         //phong
         diffChance = 0;
         specChance = 1; 

        ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;

        float term1 = 2.0f * c_pi * randX;
        float term2 = pow(randY, ( 1.0f/ (ray_.mat.phongExponent + 1)));
        float term3 = sqrt(1.0f - term2*term2);

        newRayDir = vec3( cos(term1)*term3, sin(term1)*term3, term2);

        vec3 tempxRefl = (abs(reflDir.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
        vec3 mYRefl = normalize(cross(tempx, reflDir));
        vec3 mXRefl = cross(mY, reflDir);
        newRayDir = mXRefl * newRayDir.x + mYRefl * newRayDir.y + normalize(reflDir) * newRayDir.z;
        float dot_R_Wi = clamp(dot(reflDir, newRayDir), 0.0f, 1.0f);

        pdf = specChance * (ray_.mat.phongExponent + 1) * (pow(max(0, dot_R_Wi), ray_.mat.phongExponent)) * (1/c_pi) * 0.5f;

        vec3 rho = ray_.mat.specular_color * ( ray_.mat.phongExponent + 2.0f ) * (1/c_pi) * 0.5f;
        
        bsdfFactor = rho * ( pow(dot_R_Wi,  ray_.mat.phongExponent));
        //ray_dir = normalize( mX*newRayDir.x + mY*newRayDir.y + normalize(ray_.normal)*newRayDir.z);
        ray_dir = normalize(reflDir + randomUnitVector(seed));
        float cosTheta = abs(newRayDir.z);
        throughput *= bsdfFactor * cosTheta / pdf;// pdf;
    }
        else if (ray_.mat.phong == 2){
            ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
            specular_bounce = true;
            if (ray_.mat.metal == 0.0f) specular_bounce = false;
            float f = (1.5f - 1.0f) / (1.5f + 1.0f);
            float F0 = f * f;
            vec3 albedo = mix(ray_.mat.specular_color, vec3(0.0f), ray_.mat.metal);
            vec3 specularColor = mix(vec3(F0), ray_.mat.specular_color, ray_.mat.metal);

            if(ray_.mat.phongExponent < 0.01f) 
			    ray_dir = reflect(rayDirO, ray_.normal);
		    else 
			    ray_dir = importanceSampleGGX(seed, ray_.mat.phongExponent * ray_.mat.phongExponent, ray_.normal, rayDirO);

            ray_dir =  normalize( mix(diffuse_ray_dir, ray_dir, ray_.mat.metal));

            vec3  h = normalize(-rayDirO + ray_dir);
	        float wiDotN =  max(dot(ray_dir, ray_.normal), 0.0f);
	        float w0DotN = max(dot(-rayDirO, ray_.normal), 0.0f);
            float hDotN = max(dot(h, ray_.normal), 0.0f);
	        float hDotWi = max(dot(h, ray_dir), 0.0001f);
	        float w0DotH = max(dot(-rayDirO, h), 0.0f);
            float alpha = max(ray_.mat.phongExponent * ray_.mat.phongExponent, 0.0064f);


            float D = DistributionGGX(hDotN, alpha);

            float G = SmithHeightCorrelated(w0DotN, wiDotN, alpha);

            vec3 F = evalFresnelSchlick(ray_.mat.specular_color, vec3(1.0f, 1.0f, 1.0f), w0DotH);

            //pdf = max((D * hDotN / (4.0 * w0DotN) ) * ray_.mat.metal, 0.0001f) ;
            pdf = ((D * hDotN)/ (4.0f * w0DotN)) * ray_.mat.metal;
            bsdfFactor = (D * G * F) / (4.0f * w0DotN);
            bsdfFactor += albedo / c_pi;
            pdf += (clamp(dot(ray_dir, ray_.normal), 0.0f, 1.0f) / c_pi) * (1 - ray_.mat.metal);

            throughput *=  (bsdfFactor / pdf) ;
    }

}



bool isOccluded(in vec3 ro, in vec3 rd, in float dist) {
    shadow_ray_ = true;
    uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
    traceRayEXT(scene_, flags, 0xFF, 0, 0, 1, ro, 0.005, rd, dist, 2);
    return shadow_ray_;
}





float maxIn(inout vec3 vector){
    float max = vector.x;
    if(max < vector.y)  max = vector.y;
    if(max < vector.z)  max = vector.z;

    return max;
}



void sampleBRDF(inout vec3 ray_dir, inout vec3 ray_pos, inout vec3 throughput, inout bool specular_bounce, inout uint seed, inout float pdf, inout float specChance, inout float diffChance, inout vec3 bsdfFactor, inout float isLight ) {
    pdf = 1.0f;
    vec3 rayDirO = ray_dir;
    float do_specular = 0.0f;
    float do_refraction = 0.0f;
    //int event=-1;
    float cosThetaOut;
    
    float doReflection = ray_.mat.mirror_reflectance;
    float specular_chance = ray_.mat.specular_chance;
    float refraction_chance = ray_.mat.refraction_chance;
    int event = 0;
    float randX = randomFloat(seed);
    uint aux = uint(randX);
    float randY = randomFloat(aux);
    aux = uint(randX + randY);
    float randZ = randomFloat(aux);

    //vec3 reflDir = ray_dir - 2 * dot(ray_dir, ray_.normal) * ray_.normal;

    vec3 tempx = (abs(ray_.normal.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
    vec3 mY = normalize(cross(tempx, ray_.normal));
    vec3 mX = cross(mY, ray_.normal);
    //vec3 reflDir = normalize( reflect(ray_dir, ray_.normal));
    vec3 reflDir = vec3( -dot(-ray_dir, mX), -dot(-ray_dir, mY), dot(-ray_dir, ray_.normal));
    vec3 newRayDir;


    if (specular_chance > 0.0f) {
        specular_chance = fresnelReflectAmount(
            ray_.from_inside ? ray_.mat.IOR : 1.0,
            !ray_.from_inside ? ray_.mat.IOR : 1.0,
            ray_dir,
            ray_.normal,
            ray_.mat.specular_chance,
            1.0f
        );

        float chance_multiplier = (1.0f - specular_chance) / (1.0f - ray_.mat.specular_chance);
        refraction_chance *= chance_multiplier;
    }

    float ray_select_roll = randomFloat(seed);

    if (specular_chance > 0.0f && ray_select_roll < specular_chance) {
        event = 2;
        do_specular = 1.0f;
        pdf = specular_chance;        
    }
    else if (refraction_chance > 0.0f && ray_select_roll < specular_chance + refraction_chance) {
        event = 0;
        do_refraction = 1.0f;
        pdf = refraction_chance;

    }
    else {
        specular_bounce = false;
        pdf = 1.0f - (specular_chance + refraction_chance);
        diffChance = pdf;
        event = 1;
    }
    specChance = specular_chance;
    diffChance =max(0, 1.0f - (specular_chance + refraction_chance));
    pdf = max(pdf, 0.001f);
    specular_bounce = false;




    // calculate new ray direction
    vec3 diffuse_ray_dir = normalize(ray_.normal + randomUnitVector(seed));

    vec3 specular_ray_dir = reflect(ray_dir, ray_.normal);
    specular_ray_dir = normalize(mix(specular_ray_dir, diffuse_ray_dir, ray_.mat.specular_roughness * ray_.mat.specular_roughness));

    vec3 refraction_ray_dir = refract(ray_dir, ray_.normal, ray_.from_inside ? ray_.mat.IOR : 1.0f / ray_.mat.IOR);
    refraction_ray_dir = normalize(mix(refraction_ray_dir, normalize(-ray_.normal + randomUnitVector(seed)), 0));

    ray_dir = mix(diffuse_ray_dir, specular_ray_dir, do_specular);
    ray_dir = mix(ray_dir, refraction_ray_dir, do_refraction);
    
    if(ray_.mat.mirror_reflectance == 0 && ray_.mat.phong ==0){   

         // update the ray position
        if (do_refraction == 1.0f ) {
            specular_bounce = true;
            ray_pos = (ray_pos + rayDirO * ray_.t) - ray_.normal * 0.01f;
        }
        else {
            ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
        }
        
        if (ray_.from_inside) {
            bsdfFactor = vec3(1.0f);
            throughput *= exp(-ray_.mat.refraction_color);
        }
        if (do_refraction == 0.0f) {
            bsdfFactor = mix(ray_.mat.albedo, ray_.mat.specular_color, do_specular);
            throughput *= mix(ray_.mat.albedo, ray_.mat.specular_color, do_specular);
        }
        throughput /= pdf;
    }
    if(ray_.mat.mirror_reflectance != 0){
        specular_bounce = true;
        diffChance = 0.0f;
        specChance = 0.0f;

        if (ray_.mat.mIOR > 0){
            ray_pos = (ray_pos + rayDirO * ray_.t) - ray_.normal * 0.01f;


            float cosI = dot(-rayDirO ,normalize(ray_.normal));
            float cosT;
            float etaIncOverEtaTrans;
            if(ray_.from_inside){ //from inside
            
                etaIncOverEtaTrans = ray_.mat.IOR;
                cosI = -cosI;
                cosT = 1.0f;
            }
            else{

                etaIncOverEtaTrans = 1.0f/ray_.mat.IOR;
                cosT = -1.0f;
            }

            float sinI2 = 1.0f - cosI*cosI;
            float sinT2 = etaIncOverEtaTrans*etaIncOverEtaTrans * sinI2;

            if(sinT2 < 1.0f){
            
                pdf = 1;

                ray_dir = normalize( refract(rayDirO, ray_.normal, ray_.from_inside ? ray_.mat.mIOR : 1.0f / ray_.mat.mIOR));

                bsdfFactor =  vec3(etaIncOverEtaTrans * etaIncOverEtaTrans);

                throughput *= bsdfFactor / pdf;  
            }

            

        }
        else{ 
            ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
            pdf = 1;
            ray_dir = normalize(reflect(rayDirO, ray_.normal));
            bsdfFactor = vec3(ray_.mat.mirror_reflectance);
            throughput *= bsdfFactor / pdf;
        }
    
    }
    else if(event == 1 && ray_.mat.phong==1){
        //diffuse
        diffChance = 1.0f;
        specChance = 0.0f;
        ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
        throughput *= pdf;
        float term1 = 2.0f * c_pi * randX;
        float term2 = sqrt( 1.0f - randY);

        newRayDir = vec3( cos(term1)*term2, sin(term1)*term2, sqrt(randY));
        pdf = (dot(ray_dir, ray_.normal) / c_pi) * diffChance;
        bsdfFactor = ray_.mat.albedo / c_pi;
        
        //ray_dir = normalize( mX*newRayDir.x + mY*newRayDir.y + normalize(ray_.normal)*newRayDir.z);

        float cosTheta = abs(newRayDir.z);
        throughput *= bsdfFactor * cosTheta / pdf;


    }
    else if (event == 2 && ray_.mat.phong == 1){
         //phong
         diffChance = 0.0f;
         specChance = 1.0f; 
         ray_dir = normalize(reflDir + randomUnitVector(seed));
        ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;

        float term1 = 2.0f * c_pi * randX;
        float term2 = pow(randY, ( 1.0f/ (ray_.mat.phongExponent + 1)));
        float term3 = sqrt(1.0f - term2*term2);

        newRayDir = vec3( cos(term1)*term3, sin(term1)*term3, term2);

        vec3 tempxRefl = (abs(reflDir.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
        vec3 mYRefl = normalize(cross(tempx, reflDir));
        vec3 mXRefl = cross(mY, reflDir);
        newRayDir = mXRefl * newRayDir.x + mYRefl * newRayDir.y + normalize(reflDir) * newRayDir.z;
        float dot_R_Wi = dot(reflDir, newRayDir);

        pdf = specChance * (ray_.mat.phongExponent + 1) * (pow(max(0, dot_R_Wi), ray_.mat.phongExponent)) * (1/c_pi) * 0.5f;

        vec3 rho = ray_.mat.specular_color * ( ray_.mat.phongExponent + 2.0f ) * (1/c_pi) * 0.5f;
        
        bsdfFactor = rho * ( pow(dot_R_Wi,  ray_.mat.phongExponent));

        //ray_dir = normalize( mX*newRayDir.x + mY*newRayDir.y + normalize(ray_.normal)*newRayDir.z);
        
        float cosTheta = abs(newRayDir.z);
        throughput *= bsdfFactor * cosTheta / pdf;// pdf;
    }
    else if (ray_.mat.phong == 2){
        ray_pos = (ray_pos + rayDirO * ray_.t) + ray_.normal * 0.01f;
        specular_bounce = true;
        if (ray_.mat.metal == 0.0f) specular_bounce = false;
        float f = (1.5f - 1.0f) / (1.5f + 1.0f);
        float F0 = f * f;
        vec3 albedo = mix(ray_.mat.specular_color, vec3(0.0f), ray_.mat.metal);
        vec3 specularColor = mix(vec3(F0), ray_.mat.specular_color, ray_.mat.metal);

        if(ray_.mat.phongExponent < 0.01f) 
			ray_dir = reflect(rayDirO, ray_.normal);
		else 
			ray_dir = importanceSampleGGX(seed, ray_.mat.phongExponent * ray_.mat.phongExponent, ray_.normal, rayDirO);

        ray_dir =  normalize( mix(diffuse_ray_dir, ray_dir, ray_.mat.metal));

        vec3  h = normalize(-rayDirO + ray_dir);
	    float wiDotN =  max(dot(ray_dir, ray_.normal), 0.0f);
	    float w0DotN = max(dot(-rayDirO, ray_.normal), 0.0f);
        float hDotN = max(dot(h, ray_.normal), 0.0f);
	    float hDotWi = max(dot(h, ray_dir), 0.0001f);
	    float w0DotH = max(dot(-rayDirO, h), 0.0f);
        float alpha = max(ray_.mat.phongExponent * ray_.mat.phongExponent, 0.0064f);


        float D = DistributionGGX(hDotN, alpha);

        float G = SmithHeightCorrelated(w0DotN, wiDotN, alpha);

        vec3 F = evalFresnelSchlick(ray_.mat.specular_color, vec3(1.0f, 1.0f, 1.0f), w0DotH);

        //pdf = max((D * hDotN / (4.0 * w0DotN) ) * ray_.mat.metal, 0.0001f) ;
        pdf = ((D * hDotN)/ (4.0f * w0DotN)) * ray_.mat.metal;
        bsdfFactor = (D * G * F) / (4.0f * w0DotN);
        bsdfFactor += albedo / c_pi;
        pdf += (dot(ray_dir, ray_.normal) / c_pi) * (1 - ray_.mat.metal);

        vec3 diffuseFactor = albedo / c_pi;

        throughput *=  (bsdfFactor / pdf) ;
    }

}


/*
void smallVcmBRDF(inout vec3 ray_dir, inout vec3 ray_pos, inout vec3 throughput, inout bool specular_bounce, inout uint seed, inout float pdf, inout float continuationProb, inout float cosThetaOut, inout float isLight){
    
    float reflChance;
    float do_refraction = 0.0f;
    float mReflectCoeff;
    getProbabilities(ray_dir, reflChance, continuationProb, mReflectCoeff);

    
    float randX = randomFloat(seed);
    uint aux = uint(randX);
    float randY = randomFloat(aux);
    aux = uint(randX + randY);
    float randZ = randomFloat(aux);
    int eventType;
    vec3 bsdfFactor;
    vec3 newRayDir;
    
    vec3 tempx = (abs(ray_.normal.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
    vec3 mY = normalize(cross(tempx, ray_.normal));
    vec3 mX = cross(mY, ray_.normal);
    vec3 reflDir = vec3( -dot(-ray_dir, mX), -dot(-ray_dir, mY), dot(-ray_dir, ray_.normal));

    if(randZ < ray_.mat.phong)    eventType = 1;
    else if(randZ < ray_.mat.phong + ray_.mat.specular_chance)    eventType = 2;
    else if(randZ < ray_.mat.phong + ray_.mat.specular_chance + reflChance)    eventType = 3;
    else eventType = 4;

    if(eventType == 1 ){
    //diffuse component
        float term1 = 2.0f * c_pi * randX;
        float term2 = sqrt( 1.0f - randY);

        newRayDir = vec3( cos(term1)*term2, sin(term1)*term2, sqrt(randY));
        pdf = (newRayDir.z / c_pi) * ray_.mat.phong;
        bsdfFactor = ray_.mat.albedo / c_pi;
        
        //phong component
        float cosTheta = max(0.0f, dot(ray_dir, ray_.normal));
        pdf += ray_.mat.specular_chance * (ray_.mat.phongExponent + 1) * (pow(cosTheta, ray_.mat.phongExponent)) * (1/c_pi) * 0.5f;
        
        vec3 rho = ray_.mat.specular_color * ( ray_.mat.phongExponent + 2.0f ) * (1/c_pi) * 0.5f;
        
        float dor_R_Wi = dot(reflDir, newRayDir);
        bsdfFactor += rho * ( pow(dor_R_Wi,  ray_.mat.phongExponent));
    }
    else if(eventType == 2){
    //phong component
        float term1 = 2.0f * c_pi * randX;
        float term2 = pow(randY, ( 1.0f/ (ray_.mat.phongExponent + 1)));
        float term3 = sqrt(1.0f - term2*term2);

        newRayDir = vec3( cos(term1)*term3, sin(term1)*term3, term2);

        vec3 tempxRefl = (abs(reflDir.x) > 0.99f) ? vec3(0,1,0) : vec3(1,0,0);
        vec3 mYRefl = normalize(cross(tempx, reflDir));
        vec3 mXRefl = cross(mY, reflDir);
        newRayDir = mXRefl * newRayDir.x + mYRefl * newRayDir.y + normalize(reflDir) * newRayDir.z;

        float cosTheta = max(0.0f, dot(newRayDir, ray_.normal));
        pdf = ray_.mat.specular_chance * (ray_.mat.phongExponent + 1) * (pow(cosTheta, ray_.mat.phongExponent)) * (1/c_pi) * 0.5f;

        vec3 rho = ray_.mat.specular_color * ( ray_.mat.phongExponent + 2.0f ) * (1/c_pi) * 0.5f;
        float dor_R_Wi = dot(reflDir, newRayDir);
        bsdfFactor = rho * ( pow(dor_R_Wi,  ray_.mat.phongExponent));

        //diffuse component
        pdf += ray_.mat.phong * max(0.0f, newRayDir.z / c_pi);
        bsdfFactor += ray_.mat.albedo / c_pi;
    }
    else if(eventType == 3){
        newRayDir = reflDir;
        pdf = reflChance;
        bsdfFactor = mReflectCoeff * ray_.mat.mirror_reflectance / abs(newRayDir.z);
    }
    else{
        do_refraction = 1;
        bsdfFactor = vec3(0.0f);
        if(ray_.mat.IOR > 0){
        
            float cosI = dot(-ray_dir ,normalize(ray_.normal));
            float cosT;
            float etaIncOverEtaTrans;
            if(cosI < 0.0f){ //from inside
            
                etaIncOverEtaTrans = ray_.mat.IOR;
                cosI = -cosI;
                cosT = 1.0f;
            }
            else{

                etaIncOverEtaTrans = 1.0f/ray_.mat.IOR;
                cosT = -1.0f;
            }

            float sinI2 = 1.0f - cosI*cosI;
            float sinT2 = etaIncOverEtaTrans*etaIncOverEtaTrans * sinI2;

            pdf = 0.0f;

            if(sinT2 < 1.0f){
                
                cosT *= sqrt( max(0.0f, 1.0f -sinT2));
                newRayDir = vec3(-etaIncOverEtaTrans * -reflDir.x, -etaIncOverEtaTrans * -reflDir.y, cosT);
                pdf = ray_.mat.refraction_chance;

                float refractCoeff = 1 - mReflectCoeff;

                if(isLight != 1){
                    bsdfFactor = vec3(refractCoeff *etaIncOverEtaTrans*etaIncOverEtaTrans / abs(cosT));
                }
                else{
                    bsdfFactor = vec3(refractCoeff / abs(cosT));
                }
            }

        }
    
    }
    if (do_refraction == 1.0f) {
        ray_pos = (ray_pos + ray_dir * ray_.t) - ray_.normal * 0.01f;
    }
    else {
        ray_pos = (ray_pos + ray_dir * ray_.t) + ray_.normal * 0.01f;
    }

    ray_dir = mX*newRayDir.x + mY*newRayDir.y + normalize(ray_.normal)*newRayDir.z;
    cosThetaOut = abs(newRayDir.z);
    pdf *= continuationProb;
    throughput *= bsdfFactor * (cosThetaOut / (pdf * continuationProb));
}*/