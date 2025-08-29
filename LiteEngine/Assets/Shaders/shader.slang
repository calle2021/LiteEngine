struct VSInput {
    float2 pos;
    float3 color;
    float2 tex;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
    float2 tex;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.pos, 0.0, 1.0))));
    output.color = input.color;
    output.tex = input.tex;
    return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput vert) : SV_TARGET {
    return texture.Sample(vert.tex);
}