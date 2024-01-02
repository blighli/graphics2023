Shader "Unlit/test"
{
    SubShader
    {
        Cull Off ZWrite Off ZTest Always
        Pass
        {
            // インスペクタに表示したときにわかりやすいように名前を付けておく
            Name "Update"

            CGPROGRAM
            
            // UnityCustomRenderTexture.cgincをインクルードする
           #include "UnityCustomRenderTexture.cginc"

            // 頂点シェーダは決まったものを使う
           #pragma vertex CustomRenderTextureVertexShader
           #pragma fragment frag

            // v2f構造体は決まったものを使う
            half4 frag(v2f_customrendertexture i) : SV_Target
            {
                return half4(_SinTime.z * 0.5 + 0.5, _CosTime.w * 0.5 + 0.5, _SinTime.w * 0.5 + 0.5, 1);
            }

            ENDCG
        }
    }
}