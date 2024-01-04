Shader "Unlit/test"
{
    SubShader
    {
        Cull Off ZWrite Off ZTest Always
        Pass
        {
            Name "Update"

            CGPROGRAM
            
           #include "UnityCustomRenderTexture.cginc"
		   
           #pragma vertex CustomRenderTextureVertexShader
           #pragma fragment frag

            half4 frag(v2f_customrendertexture i) : SV_Target
            {
                return half4(_SinTime.z * 0.5 + 0.5, _CosTime.w * 0.5 + 0.5, _SinTime.w * 0.5 + 0.5, 1);
            }

            ENDCG
        }
    }
}