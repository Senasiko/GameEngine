float G_Smith(float roughness, float NoV, float NoL)
{
    float  k = (roughness + 1) * (roughness + 1) / 8;
    return  (NoV / (NoV * (1 - k) + k)) *  (NoL / (NoL * (1 - k) + k));
}

float3 GGXBRDF(float3 lightVec, float3 viewVec, float3 normal, float3 diffuseColor, float3 specularColor, float roughness)
{
    const  float pi = 3.14159;
    lightVec = normalize(lightVec);
    viewVec = normalize(viewVec);
    float3 h = normalize(viewVec + lightVec);

    float NdotL = max(0, dot(normal, lightVec));
    float NdotH = max(0, dot(normal, h));
    float LdotH = max(0, dot(lightVec, h));
    float VdotH = max(0, dot(viewVec, h));
    float NdotV = max(0, dot(normal, viewVec));

    //D
    float alpha = roughness *  roughness;
    float alphaSqr = alpha*alpha;
    float denom = ((NdotH * NdotH) * (alphaSqr - 1.0f) + 1.0f);
    float D = alphaSqr / (pi * denom* denom);
    float FV;

    //fersnel & V
    float F_b = pow((1 - VdotH), 5);
    float F_a = 1;
    float k = alpha / 2;
    float vis = G_Smith(roughness, NdotV, NdotL);
    float2 FV_helper = float2(F_a*vis, F_b*vis);
    float3 col = specularColor*FV_helper.x + (1 - specularColor)*FV_helper.y;

    col = (NdotL*D*col + NdotL*diffuseColor);
	
    return float4(col,1);
}